#include "command.h"

#include <assert.h>
#include <glib-2.0/glib.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

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
  
    size_t total_length = 0;
    for (unsigned int i = 0; i < scommand_quantity; i++) {
      total_length += strlen((char*)g_queue_peek_nth(self->queue, i)) + 1;
    }
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
  
    return string_output;
}

struct pipeline_s {
    GQueue* commands; //Acá se almacenan comandos simples
    bool wait;
}

pipeline pipeline_new(void) { 
    pipeline result = malloc(sizeof(struct pipeline_s));
    assert (result != NULL);
    p->commands = g_queue_new();
    p->wait = true;
    assert (pipeline_is_empty(result) && pipeline_get_wait(result));
    return result;
}

pipeline pipeline_destroy(pipeline self) { 
    assert (self != NULL);
    if (!pipeline_is_empty(self)) {
        g_queue_free_full(self->commands, scommand_destroy);
    }
    free(self);
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
    return g_queue_is_empty(self->command);
}

unsigned int pipeline_length(const pipeline self) {
    assert(self!=NULL);
    unsigned int length = g_queue_get_length(self->command):
    assert((length == 0) == pipeline_is_empty(self));
    return length;
}

scommand pipeline_front(const pipeline self) {
    assert((self!=NULL) && !pipeline_is_empty(self));
    //return g_queue_peek_head(self->commands);
    return self->commands->head;
}

bool pipeline_get_wait(const pipelinne self) {
    assert(self!=NULL);
    return self->wait;
}

char * pipeline_to_string(const pipeline self) {
    assert(self!=NULL);

    if (pipeline_is_empty(self)==0) {
        return calloc(1, sizeof(char)); //devuelve el puntero a la memoria asignada para el caracter '\0'
    }

    size_t length_s = pipeline_length(self);
    char** commd_str = calloc(length_s, sizeof(*char));

    unsgned int length_res = 0u;
    GQueue* scomm = self->commands;
    for (unsigned int i = 0; i < length_res; ++i) {
        commd_str[i] = scommand_to_string[scomm->head];
        scomm = scomm->head;
        length_s += strlen(commd_str[i]);
    }
    
    
    assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(result)>0);
}

