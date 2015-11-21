#include "shared_data.h"

// Crear la memoria compartida... hace el truncacate
int shm_create(const char * n, size_t size) {
  int fd = shm_open(n, O_CREAT | O_RDWR, 0664);

  if (fd < 0) {
    perror("fd init");
    exit(1);
  }

  if (ftruncate(fd, size) < 0) {
    perror("ftruncate");
    exit(1);
  }

  return fd;
}
// trae la memoria compartida, se le pasa un size_t
void * shm_context(const char * n, size_t size) {
  int fd = shm_open(n, O_RDWR, 0664);

  if (fd < 0) {
    perror("fd");
    exit(1);
  }

  void * c;

  if ((c = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("map");
    exit(1);
  }

  return c;
}
// Calcula el tamaño de la memoria compartida.
size_t shm_size(const char * name, config_init_t * conf) {
  int fd = shm_open(name, O_CREAT | O_RDWR, 0664);
  if (fd < 0) {
    perror("fd init");
    exit(1);
  }

  ftruncate(fd, sizeof(size_t));

  size_t * c;

  if ((c = (size_t *)mmap(NULL, sizeof(size_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("map size");
    exit(1);
  }

  if (conf != NULL) {

    size_t s_entries = (sizeof(sample_t) * conf->queue_input_length * conf->entries);
    size_t s_aux_ent = (sizeof(aux_entrie_var_t) * conf->entries);
    size_t s_aux_inn = (sizeof(aux_entrie_var_t) * 3);
    size_t s_samples = (sizeof(sample_t) * conf->queue_sample_length * 3);

    size_t len = (sizeof(config_init_t) + s_aux_ent + s_entries + s_aux_inn + s_samples);
    *c = len;
  }

  return *c;
}
