#ifndef ML_ERROR_H
#define ML_ERROR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void printErrorOnFile(const char* originfile, int line, const char* message);

#endif
