#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#define INIT_CMD "init"
#define CTRL_CMD "ctrl"
#define REPO_CMD "rep"
#define REGI_CMD "reg"
#define MEM_DFID "evaluator"

using namespace std;

typedef struct config_init_t {
  config_init_t() :
      entries(5),
      queue_input_length(6),
      queue_output_length(10),
      blood_reactive(100),
      skin_reactive(100),
      detrit_reactive(100),
      _id(MEM_DFID) {}
  unsigned int entries;
  unsigned int queue_input_length;
  unsigned int queue_output_length;
  int blood_reactive;
  int skin_reactive;
  int detrit_reactive;
  string _id;
} config_init_t;

// Para recordar: tipo (*funcion)(arg1, arg2, argn) <- para hacer callbacks
int parse_opts(
    int argc,
    char ** argv,
    void (* init_callback)(config_init_t *),
    void (* ctrl_callback)(string),
    void (* regi_callback)(string, string *, size_t))
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
        string str(argv[opts + 1]);
        preset->_id = str;
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
    //config->mode = config_t::REPO;

  } else if (strcmp(argv[1], REGI_CMD) == 0) { // If reg
    string shared_memory = MEM_DFID;
    string * files = new string[argc - 3];

    int j = 0;
    for (int i = argc - 1; i > 0; i--) {
      if (strcmp(argv[i], "-n") == 0 && argv[i + 1] != NULL) {
        shared_memory = argv[i + 1];
      }

      if (i > 3) {
        files[j++] = argv[i];
      }
    }

    (* regi_callback)(shared_memory, files, j);
  } else {
    return -1; // Comando invalido
  }

  return 0;
}

void split(const string& s, char c, vector<string>& v) {
   string::size_type i = 0;
   string::size_type j = s.find(c); //Posicion donde encuentra char c

   while (j != string::npos) {
      v.push_back(s.substr(i, j-i));
      i = ++j;
      j = s.find(c, j);

      if (j == string::npos)
         v.push_back(s.substr(i, s.length()));
   }
}

bool isInteger(const string& s) {
   char * p ;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0) ;
}

void ctrl(string shared_memory) {
  cout << "Memoria compartida: " << shared_memory << endl;

  string line;

  for (;;) {
    cout << "> ";
    getline(cin, line);
    if (line.empty()) break;

    cout << " Se ingreso: " << line << endl;

  }
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

void validarReg(string line) {
  vector<string> v;

  split(line, ' ', v);

  if (v.size() == 3) {
    int inBandeja;
    int inCantidad;
    char inMuestra;

    if (isInteger(v[0])) {
      inBandeja = atoi(v[0].c_str());
    } else {
      cerr << "Error. Uso: <integer> {B|D|S} <integer>" << endl;
      exit(1);
    }
    if (isInteger(v[2])) {
      inCantidad = atoi(v[2].c_str());
    } else {
      cerr << "Error. Uso: <integer> {B|D|S} <integer>" << endl;
      exit(1);
    }
    if (strcmp(v[1].c_str(), "B") == 0 || strcmp(v[1].c_str(), "D") == 0  || strcmp(v[1].c_str(), "S") == 0) {
      inMuestra = v[1].c_str()[0];
    } else {
      cerr << "Error. Uso: <integer> {B|D|S} <integer>" << endl;
      exit(1);
    }

  } else {
    cerr << "/Error. Uso: <integer> {B|D|S} <integer>" << endl;
    exit(1);
  }
  //TODO guardar en memoria compartida

  cout << "1: " << v[0] << " 2: " << v[1] << " 3: " << v[2] << endl;
}

void regi(string shared_memory, string * files, size_t size) {
  cout << "Memoria compartida: " << shared_memory << endl;

  if (true) //TODO llamar validarReg


  }

  delete[] files;
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
