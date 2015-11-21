#ifndef CONSOLE_H
#define CONSOLE_H

// C++
#include <string>
#include <iostream>

// C
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

typedef enum console_type_t {
  CONTROL,
  REGISTER
} console_type_t;

class Console {
protected:
  std::string context;

private:
  std::string line;
  console_type_t kind;
  pthread_t thread;

  static void * run(void *);
  void control();
  void registe();

public:
  Console(console_type_t, std::string);
  ~Console();

  void start();
};

#endif
