#include "core.h"

void * kernel(void * f) {
  args_t args = *((args_t *)f);


  size_t len = shm_size("mem_size", NULL);
  void * mem = shm_context(args.memory, len);

  config_init_t * shm_config = (config_init_t *)mem;

  config_init_t c_shm_config = *shm_config;

  shm_config += 1;

  aux_entrie_var_t * auxes = (aux_entrie_var_t *)shm_config;

  auxes += shm_config->entries;

  sample_t * samples = (sample_t *)auxes;

  int pos;

for(;;) {
  sem_wait(&auxes[args._id].full);
  sem_wait(&auxes[args._id].mutex);

  pos = ((args._id*c_shm_config.queue_input_length)+auxes[args._id].out);
  std::cout << "args._id: " << args._id << "pos: " << pos << std::endl;

  //Seccion critica - Consumidor
  std::cout << "Recv: " << samples[pos].kind << std::endl;
  auxes[args._id].out++;
  if (auxes[args._id].out >= c_shm_config.queue_input_length) {
    auxes[args._id].out = 0;
  }

  //Fin seccion critica - consumidor
  sem_post(&auxes[args._id].mutex);
  sem_post(&auxes[args._id].empty);

}


  samples += shm_config->entries * shm_config->queue_input_length;
  aux_entrie_var_t * inner_auxes = (aux_entrie_var_t *)samples;

  inner_auxes += 3;
  sample_t * inner_samples = (sample_t *)inner_auxes;

  switch(samples[pos].kind) {
     case 'B':
     // TODO Meter en cola B
     inner_samples[(inner_auxes[0].in * 3) + 0] = samples[pos];
     inner_auxes[0].in++;
     if (inner_auxes[0].in >= shm_config->queue_sample_length) {
       inner_auxes[0].in = 0;
     }
     break;
     case 'D':
     // TODO Meter en cola D
     inner_samples[(inner_auxes[1].in * 3) + 1] = samples[pos];
     inner_auxes[1].in++;
     if (inner_auxes[1].in >= shm_config->queue_sample_length) {
       inner_auxes[1].in = 0;
     }
     break;
     default:
     // TODO Meter en cola S
     inner_samples[(inner_auxes[2].in * 3) + 2] = samples[pos];
     inner_auxes[2].in++;
     if (inner_auxes[2].in >= shm_config->queue_sample_length) {
       inner_auxes[2].in = 0;
     }
     break;
  }

  // SIGNAL

  return NULL;
}
