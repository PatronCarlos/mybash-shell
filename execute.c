#include <assert.h>
#include <errno.h>
#include <glib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "builtin.h"
#include "command.h"
#include "strextra.h"

char **parse_cmd_to_exec(scommand cmd) {
    char **argv = NULL;
    unsigned int cmd_length = scommand_length(cmd);
    
    argv = calloc(cmd_length + 1, sizeof(char *));
    if (argv == NULL) {
        exit(EXIT_FAILURE);
    }

    for (unsigned int i = 0u; i < cmd_length; i++) {
        argv[i] = scommand_front(cmd);
        scommand_pop_front(cmd);
    }
    argv[cmd_length] = NULL;  // Terminate the argv list with NULL
    return argv;
}

static void execute_cmd(scommand cmd) {
    char **parsed_cmd = parse_cmd_to_exec(cmd);

    // Redirecciones si existen
    char *redir_in = scommand_get_redir_in(cmd);
    char *redir_out = scommand_get_redir_out(cmd);

    if (redir_in != NULL) {
        freopen(redir_in, "r", stdin);
    }
    if (redir_out != NULL) {
        freopen(redir_out, "w", stdout);
    }

    // Ejecutar el comando
    if (execvp(parsed_cmd[0], parsed_cmd) == -1) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    free(parsed_cmd);
}

void execute_pipeline(pipeline apipe) {
    assert(apipe != NULL);

    if (pipeline_is_empty(apipe)) {
        return;
    }

    unsigned int app_length = pipeline_length(apipe);
    pid_t *pid_childs = malloc(app_length * sizeof(pid_t));
    if (pid_childs == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int pipefd[2];  // Pipe for communication between processes
    int prev_pipefd[2];  // Previous pipe

    for (unsigned int i = 0u; i < app_length; i++) {
        scommand cmd = pipeline_front(apipe);
        pipeline_pop_front(apipe);

        if (i != app_length - 1) {
            // Crear la pipe para todos los procesos excepto el último
            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid_t rc = fork();
        if (rc < 0) {
            // Falló el fork()
            fprintf(stderr, "ERROR: fork failed!\n");
            exit(EXIT_FAILURE);
        } else if (rc == 0) {
            // Proceso hijo

            // Redirección de entrada y salida entre pipes
            if (i > 0) {
                dup2(prev_pipefd[0], STDIN_FILENO);  // Leer de la pipe previa
                close(prev_pipefd[0]);
                close(prev_pipefd[1]);
            }

            if (i != app_length - 1) {
                dup2(pipefd[1], STDOUT_FILENO);  // Escribir en la pipe actual
                close(pipefd[0]);
                close(pipefd[1]);
            }

            // Ejecutar el comando
            execute_cmd(cmd);
            exit(EXIT_SUCCESS);
        } else {
            // Proceso padre

            if (i > 0) {
                close(prev_pipefd[0]);
                close(prev_pipefd[1]);
            }

            if (i != app_length - 1) {
                prev_pipefd[0] = pipefd[0];
                prev_pipefd[1] = pipefd[1];
            }

            pid_childs[i] = rc;
        }
    }

    // El padre debe esperar a todos los hijos si es necesario
    if (pipeline_get_wait(apipe)) {
        for (unsigned int i = 0u; i < app_length; i++) {
            waitpid(pid_childs[i], NULL, 0);
        }
    }

    free(pid_childs);
}

