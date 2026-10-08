#include "token.h"
#include "error.h"

TOKEN* initToken(int type, char* value){
    TOKEN* token = calloc(1, sizeof(TOKEN));
    if(!token) {
        ML_ERROR* error = createError(TOKEN_ERROR, "token.c", __LINE__, "Token cannot be initialized (calloc).");
        printErrorOnFile(error);
        exit(EXIT_FAILURE);
    }
    token->type = type;
    token->value = value;
    return token;
}

void free_token(TOKEN* token) {
    if (token) {
        if (token->value) free(token->value);
        free(token);
    }
}
