#include "core.h"

void * kernel(void * f) {
  args_t args = *((args_t *)f);

  std::cout << "HELLO" << args._id << std::endl;

  return NULL;
}
