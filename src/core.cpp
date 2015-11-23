#include "core.h"

void * kernel(void * f) {
  args_t args = *((args_t *)f);
  std::cout << args._id << " Created. Process" << std::endl;

for(;;) {

  size_t len = shm_size("mem_size", NULL);
  void * mem = shm_context(args.memory, len);

  config_init_t * shm_config = (config_init_t *)mem;

  config_init_t c_shm_config = *shm_config;

  aux_entrie_var_t * auxes = (aux_entrie_var_t *)shm_config + 1;

  aux_entrie_var_t * afg = auxes;

  sample_t * samples = (sample_t *)afg + shm_config->entries;

int pos;

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

  aux_entrie_var_t * inner_auxes = (aux_entrie_var_t *)samples + (shm_config->entries * shm_config->queue_input_length);
  sample_t * inner_samples = (sample_t *)inner_auxes + 3;

  switch(samples[pos].kind) {
     case 'B':
     // TODO Meter en cola B
     sem_wait(&inner_auxes[0].empty);
     sem_wait(&inner_auxes[0].mutex);

     inner_samples[(0 * shm_config->queue_sample_length)+inner_auxes[0].in] = samples[pos];
     inner_auxes[0].in++;
     if (inner_auxes[0].in >= shm_config->queue_sample_length) {
       inner_auxes[0].in = 0;
     }

     sem_post(&inner_auxes[0].mutex);
     sem_post(&inner_auxes[0].full);

     break;
     case 'D':
     // TODO Meter en cola D
     sem_wait(&inner_auxes[1].empty);
     sem_wait(&inner_auxes[1].mutex);

     inner_samples[(1 * shm_config->queue_sample_length)+inner_auxes[1].in] = samples[pos];

     inner_auxes[1].in++;
     if (inner_auxes[1].in >= shm_config->queue_sample_length) {
       inner_auxes[1].in = 0;
     }

     sem_post(&inner_auxes[1].mutex);
     sem_post(&inner_auxes[1].full);

     break;
     default:
     // TODO Meter en cola S
     sem_wait(&inner_auxes[2].empty);
     sem_wait(&inner_auxes[2].mutex);

     inner_samples[(2 * shm_config->queue_sample_length)+inner_auxes[2].in] = samples[pos];

     inner_auxes[2].in++;
     if (inner_auxes[2].in >= shm_config->queue_sample_length) {
       inner_auxes[2].in = 0;
     }

     sem_post(&inner_auxes[2].mutex);
     sem_post(&inner_auxes[2].full);

     break;
  }

  //Fin seccion critica - consumidor
  sem_post(&auxes[args._id].mutex);
  sem_post(&auxes[args._id].empty);
}

  // SIGNAL

  return NULL;
}

void * kernel_inn(void * f) {
  args_t args = *((args_t *)f);
  std::cout << args._id << " Created. Process_inn" << std::endl;

  for (;;) {
    size_t len = shm_size("mem_size", NULL);
    void * mem = shm_context(args.memory, len);

    config_init_t * shm_config = (config_init_t *)mem;

    config_init_t c_shm_config = *shm_config;

    aux_entrie_var_t * auxes = (aux_entrie_var_t *)shm_config + 1;

    aux_entrie_var_t * afg = auxes;

    sample_t * samples = (sample_t *)afg + shm_config->entries;

    aux_entrie_var_t * inner_auxes = (aux_entrie_var_t *)samples + (shm_config->entries*shm_config->queue_input_length);

    sample_t * samples_inn = (sample_t *)inner_auxes + 3;

    int pos;

    sem_wait(&inner_auxes[args._id].full);
    sem_wait(&inner_auxes[args._id].mutex);

    pos = ((args._id*shm_config->queue_sample_length)+inner_auxes[args._id].out);

    //TODO: tiempo de procesamiento y validacion de resultados
    std::cout << "ha sacado: " << samples_inn[pos].kind << std::endl;

    inner_auxes[args._id].out++;
    if (inner_auxes[args._id].out >= c_shm_config.queue_sample_length) {
      inner_auxes[args._id].out = 0;
    }

    aux_entrie_var_t * aux_out = (aux_entrie_var_t *)samples_inn +(3*shm_config->queue_sample_length);
    sample_output_t * sample_output = (sample_output_t *)aux_out +1;

    sem_wait(&aux_out[0].empty);
    sem_wait(&aux_out[0].mutex);

    // Seccion critica
    sample_output_t s;
    s.sample = samples_inn[pos];
    s.inf = 'N';

    sample_output[aux_out[0].in] = s;

    aux_out[0].in++;
    if (aux_out[0].in >= shm_config->queue_output_length) {
      aux_out[0].in = 0;
    }

    sem_post(&aux_out[0].mutex);
    sem_post(&aux_out[0].full);

    sem_post(&inner_auxes[args._id].mutex);
    sem_post(&inner_auxes[args._id].empty);


  }
}
