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

const unsigned int INTERNAL_COMMANDS_SIZE = 3u;
const char *INTERNAL_COMMANDS[INTERNAL_COMMANDS_SIZE] = {"cd", "help", "exit"};

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    bool is_internal = is_string_in_array(scommand_front(cmd), INTERNAL_COMMANDS,
                                          INTERNAL_COMMANDS_SIZE);
    return is_internal;
}

// TODO José
bool builtin_alone(pipeline p);

static builtin_cd(scommand cmd) {
    // TODO Selien, implementar los argumentos -L y -P
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

void builtin_run(scommand cmd) {
    assert(builtin_is_internal(cmd));
    char *command = scommand_front(cmd);

    if (!strcmp(command, "cd")) { //strcmp es 0 cuando s1==s2
        builtin_cd(cmd);
    }

    /*
    TODO: José
    if (strcmp(command, "help")) {
    }

    if (strcmp(command, "exit")) {
    }
    */
}

#endif
