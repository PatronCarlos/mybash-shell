#include <stdlib.h>
#include <stdbool.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
    scommand cmd = scommand_new();  
    arg_kind_t arg_type;
    char *token = parser_next_argument(p, &arg_type);
    bool error = false;
    while (token != NULL && !error) {
        if (arg_type == ARG_INPUT) { //Comprueba si el token es redir de entrada
            if (scommand_get_redir_in(cmd) != NULL) {//Comprueba si ya había redirección de entrada
                error = true;  
            } else {
                token = parser_next_argument(p, &arg_type); // Obtiene el nombre del archivo.
                if (token == NULL || arg_type != ARG_INPUT) {//Error si no hay archivo luego de la redirección o tiene tipo incorrecto.
                    error=true; 
                } else {
                    scommand_set_redir_in(cmd, token); //Si no dio error, setea la redirección de entrada.
                }
            }
        } else if (arg_type == ARG_OUTPUT) {//Comprueba si es redirección de salida
            if (scommand_get_redir_out(cmd) != NULL) {//Comprueba si ya había redirección de salida
                error = true;
            } else {
                token = parser_next_argument(p, &arg_type); // Obtiene el nombre del archivo.
                if (token == NULL || arg_type != ARG_OUTPUT) {//Error si no hay archivo luego de la redirección o tiene tipo incorrecto.
                    error=true; 
                } else {
                    scommand_set_redir_out(cmd, token);
                }
            }
        } else if (arg_type == ARG_NORMAL) {
            scommand_push_back(cmd, token);//agrego a cmd el arg normal
        }
        token = parser_next_argument(p, &arg_type);
    }
    if (error || scommand_is_empty(cmd)) {// Si ocurrió un error o cmd está vacío, limpiar y devolver NULL
        scommand_destroy(cmd);
        cmd = NULL;
    }
    return cmd;
}

pipeline parse_pipeline(Parser p) {
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe=true;

    cmd = parse_scommand(p);
    error = (cmd==NULL); /* Comando inválido al empezar */
    while (another_pipe && !error) {
        /*
         * COMPLETAR
         *
         */
    }
    /* Opcionalmente un OP_BACKGROUND al final */
    }
    /*
     *
     * COMPLETAR
     *
     */

    /* Tolerancia a espacios posteriores */
    /* Consumir todo lo que hay inclusive el \n */
    /* Si hubo error, hacemos cleanup */

    return NULL; // MODIFICAR
}

