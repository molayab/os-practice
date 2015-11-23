#include "helper.h"

void split(const std::string& s, char c, std::vector<std::string>& v) {
   std::string::size_type i = 0;
   std::string::size_type j = s.find(c); //Posicion donde encuentra char c

   while (j != std::string::npos) {
      v.push_back(s.substr(i, j-i));
      i = ++j;
      j = s.find(c, j);

      if (j == std::string::npos)
         v.push_back(s.substr(i, s.length()));
   }
}

bool isInteger(const std::string& s) {
   char * p ;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0) ;
}

bool validarReg(std::string line) {
  std::vector<std::string> v;

  split(line, ' ', v);

  if (v.size() == 3) {
    int inBandeja;
    int inCantidad;
    char inMuestra;
    if (isInteger(v[0]) && (atoi(v[0].c_str()) >= 0)) {
      if (isInteger(v[2]) && (atoi(v[2].c_str()) > 0)) {
        if (strcmp(v[1].c_str(), "B") == 0 || strcmp(v[1].c_str(), "D") == 0  || strcmp(v[1].c_str(), "S") == 0) {
          return true;
        }
      }
    }
  }
  return false;
}
