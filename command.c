#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#include "string.h"

struct scommand_s {
    GQueue* queue;
    char* redir_in;
    char* redir_out;
};

scommand scommand_new() {
    scommand command = malloc(sizeof(struct scommand_s));
    assert(command != NULL);
    command->queue = g_queue_new();
    command->redir_in = NULL;
    command->redir_out = NULL;
    assert(command != NULL && scommand_is_empty(command) &&
         scommand_get_redir_in(command) == NULL &&
         scommand_get_redir_out(command) == NULL);
    return command;
}

scommand scommand_destroy(scommand self) {
    assert(self != NULL);
    if (!scommand_is_empty(self)) {
      g_queue_free_full(self->queue, free);
    }
    free(self->redir_in);
    self->redir_in = NULL;
    free(self->redir_out);
    self->redir_out = NULL;
    free(self);
    self = NULL;
    assert(self == NULL);
    return NULL;
}

void scommand_push_back(scommand self, char* argument) {
    assert(self != NULL && argument != NULL);
    g_queue_push_tail(self->queue, argument);
    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self) {
    assert(self != NULL && !scommand_is_empty(self));
    free(g_queue_pop_head(self->queue));
}

void scommand_set_redir_in(scommand self, char* filename) {
    assert(self != NULL);
    self->redir_in = filename;
}

void scommand_set_redir_out(scommand self, char* filename) {
    assert(self != NULL);
    self->redir_out = filename;
}

bool scommand_is_empty(const scommand self) {
    assert(self != NULL);
    return g_queue_is_empty(self->queue);
}

unsigned int scommand_length(const scommand self) {
    assert(self != NULL);
    unsigned int length = g_queue_get_length(self->queue);
    assert((length == 0) == scommand_is_empty(self));
    return length;
}

char* scommand_front(const scommand self) {
    assert(self != NULL && !scommand_is_empty(self));
    char* front = g_queue_peek_head(self->queue);
    assert(front != NULL);
    return front;
}

char* scommand_get_redir_in(const scommand self) {
    assert(self != NULL);
    return self->redir_in;
}

char* scommand_get_redir_out(const scommand self) {
    assert(self != NULL);
    return self->redir_out;
}

char* scommand_to_string(const scommand self) {
    char* string_output = NULL;
    unsigned int scommand_quantity = scommand_length(self);
  
    // Cuento la cantidad de caracteres que ocupan los comandos
    size_t total_length = 0;
    for (unsigned int i = 0; i < scommand_quantity; i++) {
      total_length += strlen((char*)g_queue_peek_nth(self->queue, i)) + 1;
    }

    // Agrego el espacio para los < y > que correspondan
    if (scommand_get_redir_in(self) != NULL) {
        total_length += strlen(scommand_get_redir_in(self)) + 3; // 3 para ' < ' 
    }

    if (scommand_get_redir_out(self) != NULL) {
        total_length += strlen(scommand_get_redir_out(self)) + 3; // 3 para ' > ' 
    }

    // Concateno en orden todos los comandos y redirecciones de entrada/salida
    string_output = malloc(sizeof(char) * total_length);
    assert(string_output != NULL);
    string_output[0] = '\0';
    for (unsigned int i = 0; i < scommand_quantity; i++) {
      string_output =
          strcat(string_output, (char*)g_queue_peek_nth(self->queue, i));
      if (i < scommand_quantity - 1) {
        string_output = strcat(string_output, " ");
      }
    }

    if (scommand_get_redir_in(self) != NULL) {
        string_output = strcat(string_output, " < "); 
        string_output = strcat(string_output, scommand_get_redir_in(self)); 
    }

    if (scommand_get_redir_out(self) != NULL) {
        string_output = strcat(string_output, " > "); 
        string_output = strcat(string_output, scommand_get_redir_out(self)); 
    }

    return string_output;
}

struct pipeline_s {
    GQueue* commands; //Acá se almacenan comandos simples
    bool wait;
};

pipeline pipeline_new(void) { 
    pipeline p = malloc(sizeof(struct pipeline_s));
    assert (p != NULL);
    p->commands = g_queue_new();
    p->wait = true;
    assert (pipeline_is_empty(p) && pipeline_get_wait(p));
    return p;
}

static void pipeline_full_free(void *self) {
    scommand_destroy((scommand)self);
}

pipeline pipeline_destroy(pipeline self) { 
    assert (self != NULL);
    if (!pipeline_is_empty(self)) {
        g_queue_free_full(self->commands, pipeline_full_free);
    }
    free(self);
    self = NULL;
    assert (self == NULL);
    return self;
}

void pipeline_push_back(pipeline self, scommand sc) { 
    assert (self!=NULL && sc !=NULL);
    g_queue_push_tail(self->commands, sc);
    assert (!pipeline_is_empty(self));
}

void pipeline_pop_front(pipeline self) { 
    assert (self!=NULL && !pipeline_is_empty(self));
    scommand cmd = g_queue_pop_head(self->commands);
    scommand_destroy(cmd);
}

void pipeline_set_wait(pipeline self, const bool w) {
    assert(self!=NULL);
    self->wait = w;
}

bool pipeline_is_empty(const pipeline self) {
    assert(self!=NULL);
    return g_queue_is_empty(self->commands);
}

unsigned int pipeline_length(const pipeline self) {
    assert(self!=NULL);
    unsigned int length = g_queue_get_length(self->commands);
    assert((length == 0) == pipeline_is_empty(self));
    return length;
}

scommand pipeline_front(const pipeline self) {
    assert((self!=NULL) && !pipeline_is_empty(self));
    return g_queue_peek_head(self->commands);
}

bool pipeline_get_wait(const pipeline self) {
    assert(self!=NULL);
    return self->wait;
}

char * pipeline_to_string(const pipeline self) {
    assert(self!=NULL);
    if (pipeline_is_empty(self)) {
        return calloc(1, sizeof(char)); // puntero a memoria asignada para el caracter '\0', sino printf imprimiría (null)
    }
     
    // p_length para el '\0' de cada scommand,
    //  (p_length - 1) * 3 para los (n = scommand - 1) ' | ' que incluye el pipeline,
    //  1 para el '\0' final.
    size_t p_length = pipeline_length(self); // cantidad de scommands en pipeline
    unsigned int length_res = p_length + (p_length - 1) * 3 + 1;
    for (unsigned int i = 0; i < p_length; i++) { // La sumatoria de #caracteres de c/scommand
        char *temp_scommand_str = scommand_to_string((scommand)g_queue_peek_nth(self->commands, i));
        length_res += strlen(temp_scommand_str);
        free(temp_scommand_str);  // Libera la memoria  que creo con scommand_to_string en c/iteración
    }
    char *pipeline_string = calloc(length_res, sizeof(char));
    assert(pipeline_string != NULL);
    
    // Itero sobre el pipeline, convirtiendo c/scommand en una cadena.
    for (unsigned int i = 0; i < p_length; i++) {
        char *temp_scommand_str = scommand_to_string((scommand)g_queue_peek_nth(self->commands, i));
        pipeline_string = strcat(pipeline_string, temp_scommand_str);
        free(temp_scommand_str );  // Libera la memoria  que creo con scommand_to_string en c/iteración
        if (i < p_length - 1) {
            pipeline_string = strcat(pipeline_string, " | ");
        }
    }

    // En caso de que se este ejecutando en segundo plano, agrego (" &")
    if(!pipeline_get_wait(self)) {
        length_res += 3;
        pipeline_string = realloc(pipeline_string, length_res * (sizeof(char)));
        assert(pipeline_string != NULL);
        pipeline_string = strcat(pipeline_string, " &");
    }
    assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(pipeline_string) > 0);
    return pipeline_string;
}