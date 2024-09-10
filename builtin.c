#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "builtin.h"
#include "command.h"
#include "strextra.h"
#include "execute.h"

#define PATH_MAX 4096
#define INTERNAL_COMMANDS_SIZE 4

const char *INTERNAL_COMMANDS[INTERNAL_COMMANDS_SIZE] = {"cd", "help", "exit", "pwd"};

void print_help (void) {
    printf("MM bash versión 1.0, creada por los estudiantes:\n"
        "Carlos Patrón \n"
        "Selien Xavier Yorbandi\n"
        "Matías Kühn \n"
        "José María Beretta \n"
        "Las siguientes órdenes están definidas internamente.  Teclee «help» para\n"
        "ver esta lista.\n"
        "cd [dir] -> redirige del directorio actual al directorio en [dir].\n"
        "pwd -> imprime el nombre del directorio actual.\n"
        "exit -> cierra el terminal.\n"
        "fg -> Mueve el proceso al primer plano.\n"
        "bg -> Mueve el proceso a segundo plano.\n"
        );
}

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    bool is_internal = is_string_in_array(scommand_front(cmd), INTERNAL_COMMANDS,
                                          INTERNAL_COMMANDS_SIZE);
    return is_internal;
}

bool builtin_alone(pipeline p) {
    assert(p!=NULL);
    bool one_internal_cmd = false;
    if (pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p))) {
        one_internal_cmd = true;
    }
    return one_internal_cmd;
}

void builtin_cd(scommand cmd) {
    scommand_pop_front(cmd);
    if (scommand_is_empty(cmd)) {
        chdir(getenv("HOME"));
    } else {
        int error = chdir(scommand_front(cmd));
        if (error != 0) {
            printf("%s\n", strerror(errno));
            exit(0);
        }
    }
}

void builtin_pwd(bool breakline) {
    char cwd_buffer[PATH_MAX];
    char *working_directory = getcwd(cwd_buffer, PATH_MAX);
    if (working_directory != NULL) {
        printf("%s%s", working_directory, breakline ? "\n" : "");
    } else {
        printf("%s\n", strerror(errno));
    }
}

void builtin_run(scommand cmd) {
    assert(builtin_is_internal(cmd));
    char *command = scommand_front(cmd);
    if (!strcmp(command, "cd")) { //strcmp es 0 cuando s1==s2
        builtin_cd(cmd);
    } else if (!strcmp(command, "help")) {
        print_help();
    } else if (!strcmp(command, "exit")) {
        terminate_bg_ps();
        close(STDIN_FILENO); // Mybash.c saldrá del bucle cuando el parser encuentre EOF
        exit(EXIT_SUCCESS); //To Do: Una vez creada bg, modificar esta función.
    } else if (!strcmp(command, "pwd")) {
        builtin_pwd(true);
    }
}