#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#define INIT_CMD "init"
#define CTRL_CMD "ctrl"
#define REPO_CMD "rep"
#define REGI_CMD "reg"

using namespace std;

typedef struct config_init_t {
  config_init_t() :
      entries(5),
      queue_input_length(6),
      queue_output_length(10),
      blood_reactive(100),
      skin_reactive(100),
      detrit_reactive(100),
      _id("evaluator") {}
  unsigned int entries;
  unsigned int queue_input_length;
  unsigned int queue_output_length;
  int blood_reactive;
  int skin_reactive;
  int detrit_reactive;
  string _id;
} config_init_t;

// Para recordar: tipo (*funcion)(arg1, arg2, argn) <- para hacer callbacks
int parse_opts(int argc, char ** argv, void (*init_callback)(config_init_t *)) {
  if (argc < 1) return -1; // Numero invalido de argumentos para el programa min[1]

  if (strcmp(argv[1], INIT_CMD) == 0) {
    config_init_t * preset = new config_init_t;

    for (int opts = argc - 2; opts > 0; opts-=2) {
      // PARA SABER COMO SE IMPRIME
      cout << " [" << argv[opts] << "]: " << argv[opts + 1] << endl;

      if (strcmp(argv[opts], "-i") == 0 && argv[opts + 1] != NULL) {
        int value = atoi(argv[opts + 1]);
        if (value < 0) return -2; // Valor de argumento invalido.

        preset->entries = (unsigned int)value;
      }
      if (strcmp(argv[opts], "-ie") == 0 && argv[opts + 1] != NULL) {
        int value = atoi(argv[opts + 1]);
        if (value < 0) return -2; // Valor de argumento invalido.

        preset->queue_input_length = (unsigned int)value;
      }
      if (strcmp(argv[opts], "-oe") == 0 && argv[opts + 1] != NULL) {
        int value = atoi(argv[opts + 1]);
        if (value < 0) return -2; // Valor de argumento invalido.

        preset->queue_output_length = (unsigned int)value;
      }
      if (strcmp(argv[opts], "-n") == 0 && argv[opts + 1] != NULL) {
        string str(argv[opts + 1]);
        preset->_id = str;
      }
      if (strcmp(argv[opts], "-b") == 0 && argv[opts + 1] != NULL) {
        preset->blood_reactive = atoi(argv[opts + 1]);
      }
      if (strcmp(argv[opts], "-d") == 0 && argv[opts + 1] != NULL) {
        preset->detrit_reactive = atoi(argv[opts + 1]);
      }
      if (strcmp(argv[opts], "-s") == 0 && argv[opts + 1] != NULL) {
        preset->skin_reactive = atoi(argv[opts + 1]);
      }
    }

    // Callback a una funcion que se encargue de INIT
    (*init_callback)(preset);
  } else if (strcmp(argv[1], CTRL_CMD) == 0) {
    //config->mode = config_t::CTRL;
  } else if (strcmp(argv[1], REPO_CMD) == 0) {
    //config->mode = config_t::REPO;
  } else if (strcmp(argv[1], REGI_CMD) == 0) {
    //config->mode = config_t::REGI;
  } else {
    return -1; // Comando invalido
  }

  return 0;
}

void init(config_init_t * preset) {
  cout << "Entradas: " << preset->entries << endl;
  cout << "Colas de entrada (tamano): " << preset->queue_input_length << endl;
  cout << "Colas de salida (tamano): " << preset->queue_output_length << endl;
  cout << "Name _id: " << preset->_id << endl;
  cout << "Skin react: " << preset->skin_reactive << endl;
  cout << "Blood react: " << preset->blood_reactive << endl;
  cout << "Detrite react: " << preset->detrit_reactive << endl;

  delete preset;
}

int main(int argc, char* argv[]) {
  int p = parse_opts(argc, argv, init);

  if (p < 0) {
    // Error en el parseo:
    if (p == -1) {
      cerr << "Error en el formato de los argumentos, ver Readme.txt" << endl;
    } else if (p == -2) {
      cerr << "Error en el valor de un argumento, ver Readme.txt" << endl;
    }

    return 1;
  }

  return 0;
}
