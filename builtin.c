#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "command.h"
#include "string.h"
#include "strextra.c"

#define PATH_MAX 4096

const unsigned int INTERNAL_COMMANDS_SIZE = 4u;
const char *INTERNAL_COMMANDS[INTERNAL_COMMANDS_SIZE] = {"cd", "help", "exit", "pwd"};

static print_help (void) {
    printf("MM bash versión 1.0, creada por los estudiantes:\n"
        "Carlos Patrón \n"
        "Selien Xavier \n"
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

static void builtin_cd(scommand cmd) {

    scommand_pop_front(cmd);
    if (scommand_is_empty(cmd)) {
        chdir(getenv("HOME"));
    } else {
        int error = chdir(scommand_front(cmd));
        if (error != 0) {
            printf("%s\n", strerror(errno));
        }
    }
}

static void builtin_pwd() {
    char cwd_buffer[PATH_MAX];
    char *working_directory = getcwd(cwd_buffer, PATH_MAX);
    if (working_directory != NULL) {
        printf("%s\n", working_directory);
    } else {
        printf("%s\n", strerror(errno));
    }
    // TODO: Redireccionar la salida al siguiente comando en el pipeline
}



void builtin_run(scommand cmd) {
    assert(builtin_is_internal(cmd));
    char *command = scommand_front(cmd);
    if (!strcmp(command, "cd")) { //strcmp es 0 cuando s1==s2
        builtin_cd(cmd);
    }
    if (strcmp(command, "help")) {
        print_help();
    }
    if (strcmp(command, "exit")) {
        EXIT_SUCCESS; //To Do: Una vez creada bg, modificar esta función.
    }
    if (!strcmp(command, "pwd")) {
        builtin_pwd(cmd);
    }
}

#endif
