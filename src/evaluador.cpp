#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

using namespace std;

void console();

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cerr << "Error en el formato, ver Readme.txt" << endl;
    return 1;
  }

  // TODO leer los argumentos y guardarlos en variables globales

  if (strcmp(argv[1],"init") == 0) {

  } else if (strcmp(argv[1],"reg") == 0) {
    int opt;

    while((opt= getopt(argc - 1, &argv[1], "n:")) != EOF) {
      switch (opt) {
        case 'n':
        cout << "Reg con argumento: " << optarg << endl;
        break;

        default: break;
      }
    }

  } else if (strcmp(argv[1],"ctrl") == 0) {
    int opt;

    while((opt= getopt(argc - 1, &argv[1], "s:")) != EOF) {
      switch (opt) {
        case 's':
          cout << "Ctrl con argumento: " << optarg << endl;
        break;
        default:
        break;
      }

      console();
    }

  } else if (strcmp(argv[1],"rep") == 0) {

  } else {
    cerr << "Error en el formato, ver Readme.txt" << endl;
  }

  return 0;
}

void console() {
  string input;

  cout << "> ";
  while (cin >> input) {
    // TODO Hacer lo que dice la practica (Parseo)

    cout << "Ingresaste: " << input << endl;

    cout << "> ";
  }
}
