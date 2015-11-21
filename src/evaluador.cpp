#include "helper.h"
#include "console.h"
#include "config.h"
#include "default.h"
#include "shared_data.h"
#include "core.h"

using namespace std;

// Para recordar: tipo (*funcion)(arg1, arg2, argn) <- para hacer callbacks
int parse_opts(
    int argc,
    char ** argv,
    void (* init_callback)(config_init_t *),
    void (* ctrl_callback)(string),
    void (* regi_callback)(string, vector<string>, bool))
  {
  if (argc < 1) return -1; // Numero invalido de argumentos para el programa min[1]

  if (strcmp(argv[1], INIT_CMD) == 0) { //If init
    config_init_t * preset = new config_init_t;

    for (int opts = argc - 2; opts > 0; opts-=2) {
      // PARA SABER COMO SE IMPRIME
      cout << " [" << argv[opts] << "]: " << argv[opts + 1] << endl;

      if (strcmp(argv[opts], "-i") == 0 && argv[opts + 1] != NULL) {
        int value = atoi(argv[opts + 1]);
        if (value < 0) return -2; // Valor de argumento invalido.

        preset->entries = (unsigned int)value;
      }
      else if (strcmp(argv[opts], "-ie") == 0 && argv[opts + 1] != NULL) {
        int value = atoi(argv[opts + 1]);
        if (value < 0) return -2; // Valor de argumento invalido.

        preset->queue_input_length = (unsigned int)value;
      }
      else if (strcmp(argv[opts], "-oe") == 0 && argv[opts + 1] != NULL) {
        int value = atoi(argv[opts + 1]);
        if (value < 0) return -2; // Valor de argumento invalido.

        preset->queue_output_length = (unsigned int)value;
      }
      else if (strcmp(argv[opts], "-n") == 0 && argv[opts + 1] != NULL) {
        preset->_id = argv[opts + 1];
      }
      else if (strcmp(argv[opts], "-b") == 0 && argv[opts + 1] != NULL) {
        preset->blood_reactive = atoi(argv[opts + 1]);
      }
      else if (strcmp(argv[opts], "-d") == 0 && argv[opts + 1] != NULL) {
        preset->detrit_reactive = atoi(argv[opts + 1]);
      }
      else if (strcmp(argv[opts], "-s") == 0 && argv[opts + 1] != NULL) {
        preset->skin_reactive = atoi(argv[opts + 1]);
      }
      else if (strcmp(argv[opts], "-ee") == 0 && argv[opts + 1] != NULL) {
        preset->queue_sample_length = atoi(argv[opts + 1]);
      }
      else {
        cerr << "Error. Uso: evaluator init [-i <integer>] [-ie <integer>] [-oe <integer>] [-n <string>] [-b <integer>] [-d <integer>] [-s <integer>]" << endl;
               exit(1);
      }
    }
    // Callback a una funcion que se encargue de INIT
    (*init_callback)(preset);

  } else if (strcmp(argv[1], CTRL_CMD) == 0) { //If ctrl
    string shared_memory = MEM_DFID;
    if (argc >= 3) {
      if (strcmp(argv[2], "-s") == 0 && argv[3] != NULL) {
        shared_memory = argv[3];
      } else {
        cerr << "Error. Uso: evaluator ctrl [-s <string>]" << endl;
      }
    }
    (* ctrl_callback)(shared_memory);

  } else if (strcmp(argv[1], REPO_CMD) == 0) { // If rep
      string shared_memory = MEM_DFID;
      int inIteractive;
      int inm;

      for (int opts = argc - 2; opts > 0; opts-=2) {
        if (strcmp(argv[opts], "-s") == 0 && argv[opts + 1] != NULL) {
          shared_memory = argv[opts + 1];
        }
        else {
          if (strcmp(argv[opts], "-i") == 0 && argv[opts + 1] != NULL) {
            inIteractive = atoi(argv[opts+1]);
          }
          else if (strcmp(argv[opts], "-m") == 0 && argv[opts + 1] != NULL) {
            inm = atoi(argv[opts+1]);
          }
          else {
            cerr << "evaluator rep [-s <string>] { -i <integer> | -m <integer> }" << endl;
          }
        }

      }

      cout << "evaluara: " << shared_memory << endl;
      //TODO callback REPO
    //config->mode = config_t::REPO;

  } else if (strcmp(argv[1], REGI_CMD) == 0) { // If reg
    string shared_memory = MEM_DFID;
    vector<string> files;

    bool isIterative = false;

    for (int i = argc - 1; i > 1; --i) {
      if (strcmp(argv[i], "-n") == 0) {
        if (argc < 5) {
          cerr << "Error. Uso: evaluator reg [-n <string>] {{<filename>} ... | - }" << endl;
          exit(1);
        }
        if (strcmp(argv[i+1], "-") == 0) {
          cerr << "Error. Uso: evaluator reg [-n <string>] {{<filename>} ... | - }" << endl;
          exit(1);
        }

        shared_memory = argv[i + 1];
        --i;

        continue;
      } else if (strcmp(argv[i], "-") == 0) {
        if (argc > 5) {
          cerr << "Error. Uso: evaluator reg [-n <string>] {{<filename>} ... | - }" << endl;
          exit(1);
        }

        isIterative = true;
        continue;
      } else {
        files.push_back(argv[i]);
      }

    }

    (* regi_callback)(shared_memory, files, isIterative);
  } else {
    return -1; // Comando invalido
  }

  return 0;
}

void ctrl(string shared_memory) {
  Console cons(CONTROL, shared_memory);
  cons.start();
}

void init(config_init_t * preset) {
  cout << "Entradas: " << preset->entries << endl;
  cout << "Colas de entrada (tamano): " << preset->queue_input_length << endl;
  cout << "Colas de salida (tamano): " << preset->queue_output_length << endl;
  cout << "Name _id: " << preset->_id << endl;
  cout << "Skin react: " << preset->skin_reactive << endl;
  cout << "Blood react: " << preset->blood_reactive << endl;
  cout << "Detrite react: " << preset->detrit_reactive << endl;

  size_t len = shm_size("mem_size", preset);
  int fd = shm_create(preset->_id, len);

  void * mem = shm_context(preset->_id, len);

  config_init_t * shm_config = (config_init_t *)mem;
  *shm_config = *preset;

  aux_entrie_var_t * auxes = (aux_entrie_var_t *)++shm_config;

  for (int i = 0; i < preset->entries; ++i) {
    auxes[i].in = 0;
    auxes[i].out = 0;

    sem_init(&auxes[i].full, 0, 0);
    sem_init(&auxes[i].empty, 0, preset->queue_input_length);
    sem_init(&auxes[i].mutex, 0, 1);
  }

  auxes += (preset->entries + (preset->entries*preset->queue_input_length));

  aux_entrie_var_t * aux_inn = (aux_entrie_var_t *)auxes;

  for (int i = 0; i<3; i++) {
    aux_inn[i].in = 0;
    aux_inn[i].out = 0;

    sem_init(&aux_inn[i].full, 0, 0);
    sem_init(&aux_inn[i].empty, 0, preset->queue_sample_length);
    sem_init(&aux_inn[i].mutex, 0, 1);
  }

  pthread_t * threads = new pthread_t[preset->entries];



  for (int i = 0; i < preset->entries; ++i) {
    args_t arg;
    arg._id = (unsigned short)i;
    arg.memory = preset->_id;

    pthread_create(&threads[i], NULL, kernel, &arg);
    pthread_join(threads[i], NULL);
  }

  string line;
  for (;;) {
    getline(std::cin, line);

    if (!line.empty()) continue;

    close(fd);
    shm_unlink(preset->_id);
    break;
  }

  delete preset;
}

void regi(string shared_memory, vector<string> files, bool isIterative) {
  int inBandeja;
  int inCantidad;
  int inMuestra;

  cout << "Memoria compartida: " << shared_memory << endl;

  if (isIterative) {
    Console cons(REGISTER, shared_memory);
    cons.start();
  } else {
    for (int i = 0; i < files.size(); ++i) {
      cout << files[i] << endl;
    }
  }
}


int main(int argc, char* argv[]) {
  int p = parse_opts(argc, argv, init, ctrl, regi);

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
