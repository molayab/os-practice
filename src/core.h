#ifndef CORE_H
#define CORE_H

#include <iostream>
// C
#include <pthread.h>
#include <stdlib.h>

typedef struct args_t {
  unsigned short _id;
} args_t;

void * kernel(void *);

#endif CORE_H
