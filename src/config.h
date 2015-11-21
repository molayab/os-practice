#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "default.h"

typedef struct config_init_t {
  config_init_t() :
      entries(5),
      queue_input_length(6),
      queue_output_length(10),
      queue_sample_length(6),
      blood_reactive(100),
      skin_reactive(100),
      detrit_reactive(100),
      _id(MEM_DFID) {}
  unsigned int entries;
  unsigned int queue_input_length;
  unsigned int queue_output_length;
  unsigned int queue_sample_length;
  int blood_reactive;
  int skin_reactive;
  int detrit_reactive;
  char * _id;
} config_init_t;

#endif
