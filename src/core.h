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
} args_t;

void * kernel(void *);

#endif
