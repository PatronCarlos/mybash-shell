#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

#include "obfuscated.h"

static void show_prompt(void) {
    printf ("mybash> ");
    fflush (stdout);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);

    while (!quit) {
        ping_pong_loop(NULL);  // Llama a la función obfuscada.
        show_prompt();         // Muestra el prompt.
        pipe = parse_pipeline(input);  // Analiza el input y crea el pipeline.

        if (pipe != NULL) {
            execute_pipeline(pipe);   // Ejecuta el pipeline.
            pipe = pipeline_destroy(pipe);  // Destruye el pipeline.
        }

        /* Hay que salir luego de ejecutar? */
        quit = parser_at_eof(input);  // Verifica si se debe salir.
    }

    parser_destroy(input);  // Destruye el parser.
    input = NULL;

    return EXIT_SUCCESS;    // Fin del programa.
}