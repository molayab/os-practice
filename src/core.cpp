#include "core.h"

void * kernel(void * f) {
  args_t args = *((args_t *)f);

  size_t len = shm_size("mem_size", NULL);
  config_init_t * mem = (config_init_t *)shm_context(args.memory, len);
  config_init_t * shm_config = mem;

  aux_entrie_var_t * auxes = (aux_entrie_var_t *)mem + 1;

  for (;;) {
    //pthread_mutex_lock(args.mutex);
    sem_wait(&auxes[args._id].full);
    sem_wait(&auxes[args._id].mutex);

    sample_t a;

    sample_t * samples = (sample_t *)((shm_config + 1) + shm_config->entries);
    a.queue = samples[(args._id * shm_config->queue_input_length) + auxes[args._id].out].queue;
    a.kind = samples[(args._id * shm_config->queue_input_length) + auxes[args._id].out].kind;
    a.quantity = samples[(args._id * shm_config->queue_input_length) + auxes[args._id].out].quantity;

    //Seccion critica - Consumidor
    std::cout << "Recv: {" << a.kind << "} " << a.queue << std::endl;

    auxes[args._id].out++;
    if (auxes[args._id].out >= shm_config->queue_input_length) {
      auxes[args._id].out = 0;
    }


    //pthread_mutex_unlock(args.mutex);

    aux_entrie_var_t * aux_inn = (aux_entrie_var_t *)((shm_config + 1) + (shm_config->entries) + (shm_config->entries * shm_config->queue_input_length));
    sample_t * inner_samples = (sample_t *) aux_inn + 3;

    switch(a.kind) {
      case 'B':
      sem_wait(&aux_inn[0].empty);
      sem_wait(&aux_inn[0].mutex);

      inner_samples[aux_inn[0].in + (shm_config->queue_sample_length*0)] = a;
      aux_inn[0].in++;
      if (aux_inn[0].in >= shm_config->queue_sample_length) {
        aux_inn[0].in = 0;
      }

      sem_post(&aux_inn[0].mutex);
      sem_post(&aux_inn[0].full);

      break;
      case 'D':
      sem_wait(&aux_inn[1].empty);
      sem_wait(&aux_inn[1].mutex);

      inner_samples[aux_inn[1].in + (shm_config->queue_sample_length*1)] = a;
      aux_inn[1].in++;
      if (aux_inn[1].in >= shm_config->queue_sample_length) {
        aux_inn[1].in = 0;
      }

      sem_post(&aux_inn[1].mutex);
      sem_post(&aux_inn[1].full);
      break;
      default:
      sem_wait(&aux_inn[2].empty);
      sem_wait(&aux_inn[2].mutex);

      inner_samples[aux_inn[2].in + (shm_config->queue_sample_length*2)] = a;
      aux_inn[2].in++;
      if (aux_inn[2].in >= shm_config->queue_sample_length) {
        aux_inn[2].in = 0;
      }

      sem_post(&aux_inn[2].mutex);
      sem_post(&aux_inn[2].full);
      break;
    }

    //Fin seccion critica - consumidor
    sem_post(&auxes[args._id].mutex);
    sem_post(&auxes[args._id].empty);
  }
  return NULL;
}

void * kernel_inn (void * f) {
  args_t args = *((args_t *)f);

  std::cout << args._id << " creado_inn" << std::endl;

  size_t len = shm_size("mem_size", NULL);
  config_init_t * mem = (config_init_t *)shm_context(args.memory, len);
  config_init_t * shm_config = mem;

  aux_entrie_var_t * auxes = (aux_entrie_var_t *)mem + 1;
  sample_t * samples = (sample_t *)((shm_config + 1) + shm_config->entries);
  aux_entrie_var_t * aux_inn = (aux_entrie_var_t *)((shm_config + 1) + (shm_config->entries) + (shm_config->entries * shm_config->queue_input_length));

  for(;;){
    sem_wait(&aux_inn[args._id].full);
    sem_wait(&aux_inn[args._id].mutex);

    sample_t * inner_samples = (sample_t *) aux_inn + 3;
    sample_t a = inner_samples[(args._id*shm_config->queue_sample_length)+aux_inn[args._id].out];

    std::cout << "Recv_inn: {" << a.kind << "} " << a.queue << std::endl;

    aux_inn[args._id].out++;
    if (aux_inn[args._id].out >= shm_config->queue_sample_length) {
      aux_inn[args._id].out = 0;
    }

    //productor out
    aux_entrie_var_t * aux_out = (aux_entrie_var_t *)((shm_config + 1) + (shm_config->entries) + (shm_config->entries * shm_config->queue_input_length) + 3 + (3 * shm_config->queue_sample_length));
    sample_t * out_samples = (sample_t *) aux_out + 1;

    sem_wait(&aux_out[0].empty);
    sem_wait(&aux_out[0].mutex);

    std::cout << a.kind << std::endl;

    out_samples[aux_out[0].in] = a;
    aux_out[0].in++;
    if (aux_out[0].in >= shm_config->queue_sample_length) {
      aux_out[0].in = 0;
    }

    sem_post(&aux_out[0].mutex);
    sem_post(&aux_out[0].full);
    //fin productor out

    sem_post(&aux_inn[args._id].mutex);
    sem_post(&aux_inn[args._id].empty);

  }
  return NULL;
}

void * regi_out(void * f) {
  args_t args = *((args_t *)f);

  std::cout << args._id << " creado_out" << std::endl;

  size_t len = shm_size("mem_size", NULL);
  config_init_t * mem = (config_init_t *)shm_context(args.memory, len);
  config_init_t * shm_config = mem;

  aux_entrie_var_t * auxes = (aux_entrie_var_t *)mem + 1;
  sample_t * samples = (sample_t *)((shm_config + 1) + shm_config->entries);
  aux_entrie_var_t * aux_inn = (aux_entrie_var_t *)((shm_config + 1) + (shm_config->entries) + (shm_config->entries * shm_config->queue_input_length));
  sample_t * inner_samples = (sample_t *) aux_inn + 3;
  aux_entrie_var_t * aux_out = (aux_entrie_var_t *)((shm_config + 1) + (shm_config->entries) + (shm_config->entries * shm_config->queue_input_length) + 3 + (3 * shm_config->queue_sample_length));


  for(;;) {
    sem_wait(&aux_out[0].full);
    sem_wait(&aux_out[0].mutex);

    sample_t * out_samples = (sample_t *) aux_out + 1;
    sample_t a = out_samples[aux_out[0].out];

    std::cout << "Recv_out: {" << a.kind << "} " << a.queue << std::endl;

    aux_out[0].out++;
    if (aux_inn[0].out >= shm_config->queue_output_length) {
      aux_inn[0].out = 0;
    }

    sem_post(&aux_out[0].mutex);
    sem_post(&aux_out[0].empty);
  }

  return NULL;

}
