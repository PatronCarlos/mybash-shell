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