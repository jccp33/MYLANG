#ifndef ML_ERROR_H
#define ML_ERROR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct ML_ERROR {
    enum {
        TOKEN_ERROR,    // 0
        LEXER_ERROR,    // 1
        AST_ERROR,      // 2
        PARSER_ERROR,   // 3
        SEMANTIC_ERROR, // 4
        RUNTIME_ERROR,  // 5
        IO_ERROR        // 6
    } type;
    char* originfile;
    int line;
    char* datetime;
	char* message;
} ML_ERROR;

ML_ERROR* createError(int type, char* originfile, int line, char* message);

void printErrorOnFile(ML_ERROR* error);

#endif
