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

    size_t size_mem = *(shm_context_size());

    mem_shared_t * mem;
    mem = read_shm(context.c_str(), size_mem);

    std::cout << "tamano: " << size_mem << std::endl;
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
