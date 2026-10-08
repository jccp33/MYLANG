#include <stdio.h>
#include <stdlib.h>
#include "error.h"

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Could not open file: %s", filename);
        ML_ERROR* error = createError(IO_ERROR, "io.c", __LINE__, error_message);
        printErrorOnFile(error);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)malloc(length + 1);
    if (buffer == NULL) {
        ML_ERROR* error = createError(IO_ERROR, "io.c", __LINE__, "Memory allocation failed");
        printErrorOnFile(error);
        fclose(file);
        return NULL;
    }
    size_t read_bytes = fread(buffer, 1, length, file);
    buffer[read_bytes] = '\0';
    fclose(file);
    return buffer;
}
