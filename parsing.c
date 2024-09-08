#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "parsing.h"
#include "parser.h"
#include "command.h"
#include "string.h"
static scommand parse_scommand(Parser p) {
    scommand cmd = scommand_new();  
    arg_kind_t arg_type;
    char *arg = NULL;
    bool error = false;

     while (!error && !parser_at_eof(p)) {
        //parser_skip_blanks(p);
        arg = parser_next_argument(p, &arg_type);
        if (arg == NULL) { 
            break;
        }
        if (arg_type == ARG_INPUT) { //Comprueba si el arg es redir de entrada
            if (scommand_get_redir_in(cmd) != NULL) {//Comprueba si ya había redirección de entrada
                error = true;  
            } else {
                scommand_set_redir_in(cmd, arg); //Si no dio error, setea la redirección de entrada.
            }
        } else if (arg_type == ARG_OUTPUT) {//Comprueba si es redirección de salida
            if (scommand_get_redir_out(cmd) != NULL) {//Comprueba si ya había redirección de salida
                error = true;
            } else {
                    scommand_set_redir_out(cmd, arg);//Setea si no hay redirección de salida previa
                }
        } else if (arg_type == ARG_NORMAL) {
            scommand_push_back(cmd, arg);//agrego a cmd el arg normal
        }
    }
    if (error || scommand_is_empty(cmd)) {// Si ocurrió un error o cmd está vacío, limpiar y devolver NULL
        scommand_destroy(cmd);
        cmd = NULL;
    }
    return cmd;
}

pipeline parse_pipeline(Parser p) {
    assert ((p != NULL) && (!parser_at_eof (p)));
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe=true;

    cmd = parse_scommand(p);
    if (!strcmp(scommand_front(cmd),"\n")) {
        printf("comando vacio\n");
    }
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
        char *garbage_str = parser_last_garbage(p);
        if (garbage_str != NULL) {
                printf("Basura encontrada: %s\n", garbage_str); 
        }
        printf("ERROR: El comando ingresado es inválido.\n");
        pipeline_destroy(result);
        return NULL;
    }
    //Si la longitud del pipeline es 1, pero la longitud del primer comando simple es cero. Tambien puede ser un comando simple sin argumentos(inválido).
    if ((pipeline_length(result) == 1) && (scommand_length(pipeline_front(result))==0)) {
        printf("ERROR: No se ha ingresado un comando válido ó el comando ingresado no contiene argumentos o.\n");
        pipeline_destroy(result);
        return NULL;
    }
    return result;
}
