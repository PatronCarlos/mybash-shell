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
    char *cmdstr = NULL;
    cmdstr = scommand_to_string(cmd);
    unsigned int cmd_length = strlen(cmdstr);
    free(cmdstr);

    char **argv = NULL;

    argv = calloc(cmd_length + 1, sizeof(char *));
    if (argv == NULL) {
        exit(EXIT_FAILURE);
    }

    for (unsigned int i=0u; i < cmd_length; i++ ) {
        argv[i] = scommand_front(cmd);
        scommand_pop_front(cmd);
    }
    argv[cmd_length] = NULL;
    return argv;
}


static void execute_cmd(scommand cmd) {
    char *parsed_cmd = parsed_cmd_to_exec(cmd);

    char *redir_in = scommand_get_redir_in(cmd);
    char *redir_out = scommand_get_redir_out(cmd);

    if (redir_in) {
        int fd_in = open(redir_in, O_RDONLY, S_IRUSR, S_IWUSR, S_IXUSR);
        close(STDIN_FILENO);
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }

    if (redir_out) {
        int fd_out = open(redir_out, O_CREAT|O_WRONLY, S_IRUSR, S_IWUSR, S_IXUSR);
        close(STDOUT_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }

    if (execvp(parsed_cmd[cmd], parse_cmd) == 1) {
        fprintf(stderr, "ERROR: %s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    parse_cmd_to_exect(cmd);
}

void execute_pipeline(pipeline apipe) {
    assert(apipe != NULL);

    //Caso en el que apipe es NULL
    if (pipeline_is_empty(apipe)) {
        exit(1);
    }

    //El pipeline tiene un solo comando simple
    scommand cmd = NULL;
    if (builtin_alone(apipe)) {
        cmd = pipeline_front(apipe);
        if (builtin_is_internal(cmd)) {}
            builtin_run(cmd);
        }
        exit(1);
    }
    /*
     * Evito la creación de procesos zombies ignorando la señal que le envia el proceso hijo al padre cuando este termina.
     * Esto lo hago en caso de que el pipeline no deba esperar.
     */
     if (!pipeline_get_wait(apipe)) {
         signal(SIGCHLD, SIG_IGN);
     }

    //El pipeline tiene mas de un comando simple -> (conección de una o mas tuberias)
    app_length = pipeline_length(apipe);
    pid_t pid_childs = malloc(app_length * sizeof(pid_t)); //Reservo memoria para los PID's de los hijos del proceso
    if (pid_childs == NULL) {
        perror("ERROR: no se reservo memoria de manera correcta.\n");
        exit(EXIT_FAILURE);
    }
    int pipefd[2];
    int ptpm[2];

    for (unsigned int i = 0u; i < app_length; i++) {

        //Creo el canal de datos unidireccional para comunicar los procesos del pipeline
        if (i != 0) {
            // hay mas de un comando simple en el pipeline
            pipefd[0] = ptmp[0]; //Extremo de lectura del pipe
            pipefd[1] = ptmp[1]; //Extremo de escritura del pipe
        }
        //Ahora, mientra no me encuentr en el extremo del pipeline, conceto la tuberia entre los comandos simples
        if (i != app_length - 1) {
            int res = pipe(pipefd);
        }
        if (res == -1) {
           exit(EXIT_FAILURE);
        }
        pid_t rc = fork();

        if (rc < 0) {
            //El fork() falló
            fprintf(stderrm "ERROR: fork failed!\n");
            exit(1);
        } else if (rc == 0) {
            //Proceso hijo
            if (i != app_length - 1) {
            close(pipefd[0]); //cerramos el lado de lectura del pipe
            close(STDOUT_FILENO); //Cierro y Redirecciono la salida estandar del archivo
            dup(pipefd[1]);
            }
            // En caso de que el poceso ya no sea el hijo
            if (rc != 0) {
                close(ptmp[1]); // Cerramos el lado de escitura del pipe
                close(STDIN_FILENO); // Cierro u redirecciono la entrada estandar del archivo
                dup(ptmp[0]);
                close(ptmp[0]);// Cerramos el lado de lectura del piepe
            }

            char ** parse_cmd = parse_cmd_to_exec(cmd);
            int execute = execvp(parse_cmd[0], parsed_cmd);
            if (execute == -1) {
                fprintf(stderr, "ERROR: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        } else {
            //Vuelvo al Proceso Padre - Cierro los descriptores de archivos
            if (i != 0) {
                close(ptmp[0]);
                close(ptmp[1]);
            }
            pid_childs(i) = rc;
            pipeline_pop_front(apipe);
        }
        /*En caso de que el pipeline tenga que esperar,
         * se suspende la ejecución del proceso invocador hasta que un hijo,
         * el cual se especifica con pid_child[i], haya cambiado de estado.
         */
        if (pipeline_get_wait(apipe)) {
            for (unsigned int i=0; i < app_length; i++) {
                waitpid(pid_child[i], NULL, 0);
            }
        }
    }
    free(pid_childs);
}
