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
        
        arg = parser_next_argument(p, &arg_type);
        if (arg == NULL) { 
            break;
        }
        if (arg_type == ARG_INPUT) { //Comprueba si el arg es redir de entrada.
            if (scommand_get_redir_in(cmd) != NULL) {//Comprueba si ya había redirección de entrada.
                error = true;  
            } else {
                scommand_set_redir_in(cmd, arg); //Si no dio error, setea la redirección de entrada.
            }
        } else if (arg_type == ARG_OUTPUT) {//Comprueba si es redirección de salida.
            if (scommand_get_redir_out(cmd) != NULL) {//Comprueba si ya había redirección de salida.
                error = true;
            } else {
                    scommand_set_redir_out(cmd, arg);//Setea si no hay redirección de salida previa.
                }
        } else if (arg_type == ARG_NORMAL) {
            scommand_push_back(cmd, arg);//agrego a cmd el arg normal.
        }
        parser_skip_blanks(p);
    }
    if (error || scommand_is_empty(cmd)) {// Si ocurrió un error o cmd está vacío, limpiar y devolver NULL.
        scommand_destroy(cmd);
        cmd = NULL;
    }
    return cmd;
}



pipeline parse_pipeline(Parser p) {
    pipeline pipe = pipeline_new();
    bool is_pipe = false;
    bool has_op_background = false;
    
    while (!parser_at_eof(p)) {
        scommand cmd = parse_scommand(p);
        if (cmd == NULL) {
            bool garbage;
        parser_garbage(p, &garbage);
        if (garbage) {
            char* garbage_str = parser_last_garbage(p);
            if (garbage_str != NULL) {
                printf("Basura encontrada: %s\n", garbage_str); 
            }
            pipeline_destroy(pipe); //Si se encuentra basura, puede ser un error, así que se destruye el pipe.
            return NULL;
        } else {
            printf ("No hay comando\n");
            pipeline_destroy(pipe); //Si se encuentra basura, puede ser un error, así que se destruye el pipe.
            return NULL;
        }
        }
        pipeline_push_back(pipe, cmd);
        parser_op_pipe(p, &is_pipe); //Esto verifica si hay un | en el comando, es decir si es una pipe.
        if (is_pipe) {
            continue; 
        }
        parser_op_background(p, &has_op_background); //Y esto verifica si hay un operador &, es decir de background.
        if (has_op_background) {
            pipeline_set_wait(pipe, false); //Si hay operador de background, no espera.
        } else {
            pipeline_set_wait(pipe, true); 
        }
        parser_skip_blanks(p);
        
        bool garbage;
        parser_garbage(p, &garbage);
        if (garbage) {
            char* garbage_str = parser_last_garbage(p);
            if (garbage_str != NULL) {
                printf("Basura encontrada: %s\n", garbage_str); 
            }
            pipeline_destroy(pipe); //Si se encuentra basura, puede ser un error, así que se destruye el pipe.
            return NULL;
        }

        break; //Salir del bucle si no hay más comandos u operadores de pipe.
    }
    
    return pipe;
}

