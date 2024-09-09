#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"
#include "errno.h"
#include "obfuscated.h"
#include <unistd.h>
#include <string.h>
#include "asciicolors.h"

static void restore_asciicolor(void) {
    printf(RESET);
}

static void show_username(void) {
    char *username = getlogin();
    if (username != NULL) {
        printf(GREEN "%s@", username);
    } else {
        printf("%s\n", strerror(errno));
    }
    restore_asciicolor();
}

static void show_hostname(void) {
    char hostname[256];
    int error = gethostname(hostname, sizeof(hostname)) == 0;
    if (error < 0) printf("%s\n", strerror(errno));
    printf(BLUE "%s", hostname);
    restore_asciicolor();
}

static void show_relative_path(void) {
    char cwd_buffer[4096];
    char *working_directory = getcwd(cwd_buffer, sizeof(cwd_buffer));
    char *user_name = getlogin();
    if (user_name == NULL) {
        printf("%s\n", strerror(errno));
    }
    char home_dir[1024];
    snprintf(home_dir, sizeof(home_dir), "/home/%s", user_name);

    if (working_directory != NULL) {
        char *position = strstr(working_directory, home_dir);
        if (position == working_directory) {
            printf("~%s", working_directory + strlen(home_dir));
        } else {
            printf("%s", working_directory);
        }
    } else {
        printf("Error: %s\n", strerror(errno));
    }
}

static void show_prompt(void) {
    show_username();
    show_hostname();
    printf(YELLOW " ");
    show_relative_path();
    restore_asciicolor();
    printf("$ ");
    fflush (stdout);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);

    while (!quit) {
        ping_pong_loop("YellowFlamingo");  // Llama a la función obfuscada.
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