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
    config_init_t * mem = (config_init_t *)shm_context(context.c_str(), len);
    config_init_t * shm_config = mem;

    aux_entrie_var_t * auxes = (aux_entrie_var_t *)mem + 1;

    sem_wait(&auxes[sample.queue].empty);
    sem_wait(&auxes[sample.queue].mutex);

    sample_t * samples = (sample_t *)((shm_config + 1) + shm_config->entries);
    //samples[(sample.queue * shm_config->entries) + auxes[sample.queue].in] = sample;
    samples[(sample.queue * shm_config->queue_input_length) + auxes[sample.queue].in].queue = sample.queue;
    samples[(sample.queue * shm_config->queue_input_length) + auxes[sample.queue].in].kind = sample.kind;
    samples[(sample.queue * shm_config->queue_input_length) + auxes[sample.queue].in].quantity = sample.quantity;

    auxes[sample.queue].in++;
    if (auxes[sample.queue].in >= shm_config->queue_input_length) {
      auxes[sample.queue].in = 0;
    }

    // Fin seccion critica - Productor
    sem_post(&auxes[sample.queue].mutex);
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
