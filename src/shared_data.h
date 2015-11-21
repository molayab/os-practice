#ifndef SHM_H
#define SHM_H
#define SHM_CONTEXT "camocontext"

#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include "config.h"

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct sample_t {
  int queue;
  char kind;
  int quantity;
} sample_t;

typedef struct aux_entrie_var_t {
  int in;
  int out;
  sem_t mutex;
  sem_t empty;
  sem_t full;
} aux_entrie_var_t;

// Crear la memoria compartida... hace el truncacate
int shm_create(const char *, size_t);
// trae la memoria compartida, se le pasa un size_t
void * shm_context(const char *, size_t);
// Calcula el tamaño de la memoria compartida.
size_t shm_size(const char *, config_init_t *);

#endif
