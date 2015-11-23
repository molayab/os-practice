#include "console.h"
#include "helper.h"
#include "shared_data.h"

Console::Console(console_type_t kind, std::string context) : kind(kind), context(context) { }

Console::~Console() { }

void Console::start() {
  int p_context;

  p_context = pthread_create(
    &this->thread,
    NULL,
    Console::run,
    this
  );

  pthread_join(this->thread, NULL);
}

void Console::control() {
  std::cout << "Memoria compartida: " << context << std::endl;
  std::cout << "Usted ingreso a control: " << line << std::endl;
  // TODO
}

void Console::registe() {
  if (validarReg(this->line)) {
    std::vector<std::string> v;

    split(this->line, ' ', v);

    sample_t sample;
    sample.queue = atoi(v[0].c_str());
    sample.kind = v[1].c_str()[0];
    sample.quantity = atoi(v[2].c_str());

    size_t len = shm_size("mem_size", NULL);
    void * mem = shm_context(context.c_str(), len);

    config_init_t * shm_config = (config_init_t *)mem;

    config_init_t c_shm_config = *shm_config;

    aux_entrie_var_t * auxes = (aux_entrie_var_t *)shm_config + 1;

    aux_entrie_var_t * afg = auxes;

    sample_t * samples = (sample_t *)afg + shm_config->entries;

    int pos;
    std::cout << "queue sample: " << sample.queue << std::endl;

    sem_wait(&auxes[sample.queue].empty);
    sem_wait(&auxes[sample.queue].mutex);

    //Seccion critica - Productor
    pos = ((sample.queue*c_shm_config.queue_input_length)+auxes[sample.queue].in);
    samples[pos] = sample;
    std::cout << "Sample: " << samples[pos].kind << std::endl;
    auxes[sample.queue].in++;
    if (auxes[sample.queue].in >= c_shm_config.queue_input_length) {
      auxes[sample.queue].in = 0;
    }
    // Fin seccion critica - Productor
    sem_post(&auxes[sample.queue].mutex);
    //std::cout << "mutex: " << samples[pos].kind << std::endl;
    sem_post(&auxes[sample.queue].full);

  }
}

void * Console::run(void * data) {
  Console * context = (Console *) data;

  for (;;) {
    // Prompt
    std::cout << "> ";

    getline(std::cin, context->line);

    if (context->line.empty()) break;

    switch (context->kind) {
      case CONTROL:
      context->control();
      break;
      case REGISTER:
      context->registe();
      break;
      default: break;
    }
  }
}
