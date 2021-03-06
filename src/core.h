#ifndef CORE_H
#define CORE_H

#include <iostream>
// C
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#include "shared_data.h"

typedef struct args_t {
   int _id;
   char * memory;
   pthread_mutex_t * mutex;
} args_t;

void * kernel(void *);
void * kernel_inn(void *);
void * regi_out(void *);

#endif
