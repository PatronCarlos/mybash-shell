#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#include <assert.h>
#include <stdbool.h>

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

bool builtin_alone(pipeline p);
/*
 * Indica si el pipeline tiene solo un elemento y si este se corresponde a un
 * comando interno.
 *
 * REQUIRES: p != NULL
 *
 * ENSURES:
 *
 * builtin_alone(p) == pipeline_length(p) == 1 &&
 *                     builtin_is_internal(pipeline_front(p))
 *
 *
 */

void builtin_run(scommand cmd);
/*
 * Ejecuta un comando interno
 *
 * REQUIRES: {builtin_is_internal(cmd)}
 *
 */

#endif
