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

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    bool is_internal = is_string_in_array(scommand_front(cmd), INTERNAL_COMMANDS,
                                          INTERNAL_COMMANDS_SIZE);
    return is_internal;
}

// TODO
bool builtin_alone(pipeline p);

static void builtin_cd(scommand cmd) {
    // TODO: Implementar los argumentos -L y -P
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

    /*
    TODO:
    
    if (strcmp(command, "help")) {
    }

    if (strcmp(command, "exit")) {
    }
    */

    if (!strcmp(command, "pwd")) {
        builtin_pwd(cmd);
    }
}

#endif
