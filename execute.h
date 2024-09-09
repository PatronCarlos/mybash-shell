/* Ejecuta comandos simples y pipelines.
 * No toca ningún comando interno.
 */

#ifndef EXECUTE_H
#define EXECUTE_H

#include "command.h"

// Declara el prototipo de la función parse_cmd_to_exec
char **parse_cmd_to_exec(scommand cmd);

// Declara el prototipo de la función execute_pipeline
void execute_pipeline(pipeline apipe);
/*
 * Ejecuta un pipeline, identificando comandos internos, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */

#endif /* EXECUTE_H */
