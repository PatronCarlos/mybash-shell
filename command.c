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