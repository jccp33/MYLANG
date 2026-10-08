#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "error.h"

#define FILENAME "errors/errors.txt"

ML_ERROR* createError(int type, char* originfile, int line, char* message){
    ML_ERROR* error = malloc(sizeof(ML_ERROR));
    if (!error) {
        printf("ERROR: Memory allocation failed for ML_ERROR\n");
        exit(EXIT_FAILURE);
    }
    time_t rawtime;
    time(&rawtime);
    char* temp_time = ctime(&rawtime);
    if (temp_time) {
        temp_time[24] = '\0'; 
    }
    error->type = type;
    error->line = line;
    error->originfile = strdup(originfile);
    error->datetime = strdup(temp_time);
    error->message = strdup(message);
    return error;
}

void printErrorOnFile(ML_ERROR* error){
    const char* errorTypeStr;
    switch (error->type) {
        case LEXER_ERROR: errorTypeStr = "LEXER_ERROR"; break;
        case PARSER_ERROR: errorTypeStr = "PARSER_ERROR"; break;
        case SEMANTIC_ERROR: errorTypeStr = "SEMANTIC_ERROR"; break;
        case RUNTIME_ERROR: errorTypeStr = "RUNTIME_ERROR"; break;
        case IO_ERROR: errorTypeStr = "IO_ERROR"; break;
        default: errorTypeStr = "UNKNOWN_ERROR"; break;
    }
    FILE* file = fopen(FILENAME, "a");
    if (!file) {
        printf("ERROR: [%s] %s:%d [%s] - %s\n", errorTypeStr, error->originfile, error->line, error->datetime, error->message);
        printf("ERROR: the file %s could not be opened\n", FILENAME);
    }else{
        fprintf(file, "ERROR: [%s] %s:%d [%s] - %s\n", errorTypeStr, error->originfile, error->line, error->datetime, error->message);
        printf("\nERROR: view error in file '%s'\n", FILENAME);
        fclose(file);
    }
}
