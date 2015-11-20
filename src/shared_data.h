#ifndef SHM_H
#define SHM_H
#define SHM_CONTEXT "camocontext"

#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include "config.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
/*
 * En este archivo se encuentran todas las estructuras que hacen
 * parte de la memoria compartida del programa, asi como las
 * operaciones de estas. Asi mismo, en este archivo se encuentran
 * las operaciones de memoria compartida, por lo que solo es
 * necesario llamar a las funciones necesarias, la memoria
 * sera devuelta en su respectiva estructura. Note que no se hace
 * uso de clases ni POO. Todos los llamados a esta seccion del
 * codigo son de bajo nivel y orientados a estructuras y func.
 * codigo C con conponentes de C++.
 *
 * Mateo Olaya Bernal (2015).
 */

typedef struct sample_t {
    int queue;
    char kind;
    int quantity;
} sample_t;

typedef struct queue_t {
    sample_t * buffer;
    size_t size;
    unsigned int pointer;
    unsigned int head;
} queue_t;

typedef struct mem_shared_t {
    queue_t output_queue;
    queue_t * input_queue;
    size_t queues;

    queue_t queue_b;
    queue_t queue_d;
    queue_t queue_s;
    // Semaforos y variables de ayuda en la memoria compartida;
} mem_shared_t;

typedef const char * mem_id;

// Operaciones de colas.
int queue_create(queue_t *, unsigned int);
int queue_append(queue_t *, sample_t);
const sample_t * dequeue(queue_t *);
void queue_delete(queue_t *);
// Fin;
// Operaciones de memoria compartida.
size_t create_shm(config_init_t *);
mem_shared_t * read_shm(const char *, size_t);

size_t * shm_context_size();
// Fin;

#endif
