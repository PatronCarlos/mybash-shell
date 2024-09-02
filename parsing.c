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
        //Si el parsr es nulo devielvo un mensaje de error y luego lo destruyo
        if (cmd == NULL) {
            printf("[ERROR]: No existe archivo de entrada o salida!\n"
                              "Asegurece de ingresar un archivo valido.\n");

            pipeline_destroy(result);
            return NULL;
        }
        //Salteo los espacios en blaco en que caso de que los haya
        parser_skip_blanks(p);
        //Añado el comando simple al pipeline
        pipeline_push_back(result, cmd);
        //Busco un nuevo pipe. Si lo hay, lo consumo.
        parser_op_pipe(p, &another_pipe);
    }


    /* Tolerancia a espacios posteriores */
    /* Consumir todo lo que hay inclusive el \n */
    /* Si hubo error, hacemos cleanup */
    bool p_garbage; //Determinara si se encontraron símbolos distintos de espacios en blanco en el procesamiento
    parser_garbage(p, &p_garbage);
    //Si se encuentran caracteres inválidos despues del último comando de la secuencia
    if (p_garbage) {
        printf("ERROR: El comando ingresado es inválido.\n");
        pipeline_destroy(result);
        return NULL;
    }
    //Si al longitud del pipeline es 1, pero la longitud del primer comando simple es cero. Tambien puede ser un comando simple sin argumentos(inválido).
    if ((pipeline_length(result) == 1) && (scommand_length(pipeline_front(result))==0)) {
        printf("ERROR: No se ha ingresado un comando válido ó el comando ingresado no contiene argumentos o.\n");
        pipeline_destroy(result);
        return NULL;
    }
    return result;
}
