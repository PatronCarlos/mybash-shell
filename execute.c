#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <glib.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>

#include "command.h"
#include "tests/syscall_mock.h"
#include "execute.h"
#include "builtin.h"

#define MAX_BACKGROUND_PROCESSES 100 // Arbitrariamente
pid_t background_pids[MAX_BACKGROUND_PROCESSES];
int n_bg_pids = 0;

static void add_bg_pid(pid_t pid) {
    if (n_bg_pids < MAX_BACKGROUND_PROCESSES) {
        background_pids[n_bg_pids++] = pid;
    }
}

void terminate_bg_ps() {
    for (int i = 0; i < n_bg_pids; i++) {
        if (kill(background_pids[i], SIGTERM) == 0) {
            waitpid(background_pids[i], NULL, 0);
        }
    }
    n_bg_pids = 0;
}

static void handle_input_redirection(scommand cmd) {
    char* filename_in = scommand_get_redir_in(cmd);
    if (filename_in) {
        int in_redir = open(filename_in, O_RDONLY, S_IRWXU);
        if (in_redir == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        close(STDIN_FILENO);
        dup(in_redir);
        close(in_redir);
    }
}

static void handle_output_redirection(scommand cmd) {
    char* filename_out = scommand_get_redir_out(cmd);
    if (filename_out) {
        int out_redir = open(filename_out, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        if (out_redir == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        close(STDOUT_FILENO);
        dup(out_redir);
        close(out_redir);
    }
}

static char** parse_cmd_to_exec(scommand cmd, unsigned int cmd_length) {
    char** argv = calloc(cmd_length + 1, sizeof(char*));
    if (argv == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    unsigned int i = 0;
    while (!scommand_is_empty(cmd)) {
        unsigned int length_com = strlen(scommand_front(cmd)) + 1;
        argv[i] = malloc(length_com * sizeof(char));
        if (argv[i] == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        strcpy(argv[i], scommand_front(cmd));
        scommand_pop_front(cmd);
        ++i;
    }

    argv[cmd_length] = NULL;
    return argv;
}

static void execute_command(scommand cmd) {
    assert(cmd != NULL);

    unsigned int cmd_length = scommand_length(cmd);
    char **argv = parse_cmd_to_exec(cmd, cmd_length);

    handle_input_redirection(cmd);
    handle_output_redirection(cmd);

    if (execvp(argv[0], argv) == -1) {
        if(errno == ENOENT) {
            fprintf(stderr, "mybash : instruccion no encontrada.\n");
        } else {
            fprintf(stderr, "%s\n", strerror(errno));
        }
        exit(EXIT_FAILURE);
    }
    
    for (unsigned int i = 0; i < cmd_length; ++i) {
        free(argv[i]);
    }
    free(argv);
}

void execute_pipeline(pipeline apipe) {
    assert(apipe != NULL);

    if (pipeline_is_empty(apipe))
        return;

    if (builtin_alone(apipe)) {
        builtin_run(pipeline_front(apipe));
        return;
    }
    /*
     * Evito la creación de procesos zombies ignorando la señal que le envia el proceso hijo al padre cuando este termina.
     * Esto lo hago en caso de que el pipeline no deba esperar.
    */
    if (!pipeline_get_wait(apipe)) {
        signal(SIGCHLD, SIG_IGN);
    }

    //El pipeline tiene mas de un comando simple -> (conección de una o mas tuberias)
    unsigned int app_length  = pipeline_length(apipe);
    pid_t* pid_childs = malloc(app_length  * sizeof(pid_t));
    
    int pipefd[2];
    int ptpm[2];

    for (unsigned int i = 0; i < app_length ; ++i) {
        //Creo el canal de datos unidireccional para comunicar los procesos del pipeline
        if (i != 0) {
            // hay mas de un comando simple en el pipeline
            ptpm[0] = pipefd[0]; //Extremo de lectura del pipe
            ptpm[1] = pipefd[1]; //Extremo de escritura del pipe
        }
        //Ahora, mientras no me encuentre en el extremo del pipeline, conceto la tuberia entre los comandos simples
        if (i != app_length  - 1) {
            if(pipe(pipefd) == -1) {
                exit(EXIT_FAILURE);
            }
        }

        pid_t rc = fork();

        if (!pipeline_get_wait(apipe)) {
            add_bg_pid(rc);
        }
    
        if (rc < 0) {
            //El fork() falló
            fprintf(stderr, "%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (rc == 0) {

            if (i != app_length  - 1) {
                close(pipefd[0]);
                close(STDOUT_FILENO);
                dup(pipefd[1]);
                close(pipefd[1]);
            }

            if (i != 0) {
                close(ptpm[1]);
                close(STDIN_FILENO);
                dup(ptpm[0]);
                close(ptpm[0]);
            }

            execute_command(pipeline_front(apipe));    
        } else {
            if (i != 0) {
                close(ptpm[0]);
                close(ptpm[1]);
            }
            pid_childs[i] = rc;
            pipeline_pop_front(apipe);
        }
    }
     /*En caso de que el pipeline tenga que esperar,
         * se suspende la ejecución del proceso invocador hasta que un hijo,
         * el cual se especifica con pid_child[i], haya cambiado de estado.
    */
    if (pipeline_get_wait(apipe)) {
        for (unsigned int i = 0; i < app_length ; ++i)
            waitpid(pid_childs[i], NULL, 0);
    }
    
    free(pid_childs);
}