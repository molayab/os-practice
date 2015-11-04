#include "console.h"

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
