#include "helper.h"
#include "console.h"
#include "config.h"
#include "default.h"
#include "shared_data.h"
#include "core.h"

#include <fstream>

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


      pthread_t threads_out;
      pthread_mutex_t mutex_out = PTHREAD_MUTEX_INITIALIZER;

        args_t arg;
        arg._id = 0;
        arg.memory = (char *)shared_memory.c_str();
        arg.mutex = &mutex_out;

        pthread_create(&threads_out, NULL, regi_out, &arg);
        pthread_join(threads_out, NULL);

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

  config_init_t * mem = (config_init_t *)shm_context(preset->_id, len);
  config_init_t * shm_config = mem;

  *shm_config = *preset;

  aux_entrie_var_t * auxes = (aux_entrie_var_t *)mem + 1;

  for (int i = 0; i < shm_config->entries; ++i) {
    auxes[i].in = 0;
    auxes[i].out = 0;

    sem_init(&auxes[i].full, 1, 0);
    sem_init(&auxes[i].empty, 1, shm_config->queue_input_length);
    sem_init(&auxes[i].mutex, 1, 1);
  }

  sample_t * samples = (sample_t *)((shm_config + 1) + shm_config->entries);

  aux_entrie_var_t * aux_inn = (aux_entrie_var_t *)((shm_config + 1) + (shm_config->entries) + (shm_config->entries * shm_config->queue_input_length));

  for (int i = 0; i<3; ++i) {
    aux_inn[i].in = 0;
    aux_inn[i].out = 0;

    sem_init(&aux_inn[i].full, 1, 0);
    sem_init(&aux_inn[i].empty, 1, shm_config->queue_sample_length);
    sem_init(&aux_inn[i].mutex, 1, 1);
  }

  aux_entrie_var_t * aux_out = (aux_entrie_var_t *)((shm_config + 1) + (shm_config->entries) + (shm_config->entries * shm_config->queue_input_length) + 3 + (3 * shm_config->queue_sample_length));

  aux_out[0].in = 0;
  aux_out[0].out = 0;

  sem_init(&aux_out[0].full, 1, 0);
  sem_init(&aux_out[0].empty, 1, shm_config->queue_output_length);
  sem_init(&aux_out[0].mutex, 1, 1);

  sample_t * inner_samples = (sample_t *) aux_inn + 3;


  pthread_t * threads = new pthread_t[shm_config->entries];
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  for (int i = 0; i < shm_config->entries; ++i) {
    args_t arg;
    arg._id = i;
    arg.memory = shm_config->_id;
    arg.mutex = &mutex;

    pthread_create(&threads[i], NULL, kernel, &arg);
    sleep(1);
  }

  pthread_t * threads_inn = new pthread_t[3];
  pthread_mutex_t mutex_inn = PTHREAD_MUTEX_INITIALIZER;

  for (int i = 0; i < 3; ++i) {
    args_t arg;
    arg._id = i;
    arg.memory = shm_config->_id;
    arg.mutex = &mutex_inn;

    pthread_create(&threads_inn[i], NULL, kernel_inn, &arg);
    sleep(1);
  }

  close(fd);

  delete preset;

  for(;;);
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

      ifstream file(files[i].c_str());

      while(!file.eof()) {
        std::string s;
        getline(file, s);

        if (validarReg(s)) {
          std::vector<std::string> v;

          split(s, ' ', v);

          sample_t sample;
          sample.queue = atoi(v[0].c_str());
          sample.kind = v[1].c_str()[0];
          sample.quantity = atoi(v[2].c_str());

          size_t len = shm_size("mem_size", NULL);
          config_init_t * mem = (config_init_t *)shm_context(shared_memory.c_str(), len);
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
