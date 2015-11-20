#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void split(const std::string&, char, std::vector<std::string>&);
bool isInteger(const std::string&);
bool validarReg(std::string);

#endif
