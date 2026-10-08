#include "lexer.h"

LEXER* initLexer(char* contents) {
    LEXER* lexer = calloc(1, sizeof(LEXER));
    if(!lexer) {
        ML_ERROR* error = createError(LEXER_ERROR, "lexer.c", __LINE__, "Lexer cannot be initialized (calloc).");
        printErrorOnFile(error);
        exit(EXIT_FAILURE);
    }
    lexer->contents = contents;
    lexer->i = 0;
    lexer->c = contents[0];
    return lexer;
}

void advance(LEXER* lexer) {
    if(lexer->c != '\0' && (size_t)lexer->i < strlen(lexer->contents)){
        lexer->i += 1;
        lexer->c = lexer->contents[lexer->i];
    }
}

void skipWhitespace(LEXER* lexer) {
    while(lexer->c == ' ' || lexer->c == '\t' || lexer->c == 10 || lexer->c == 13 || lexer->c == '\r' || lexer->c == '\n') {
        advance(lexer);
    }
}

char* getCharAsString(LEXER* lexer) {
    char* str = calloc(2, sizeof(char));
    if(!str) {
        ML_ERROR* error = createError(LEXER_ERROR, "lexer.c", __LINE__, "str cannot be initialized (calloc).");
        printErrorOnFile(error);
        exit(EXIT_FAILURE);
    }
    str[0] = lexer->c;
    str[1] = '\0';
    return str;
}

TOKEN* collectId(LEXER* lexer, HashTable* keywords) {
    char buffer[1024];
    int i = 0;
    while(isalnum(lexer->c) || lexer->c == '_'){
        if(i < 1023) buffer[i++] = lexer->c;
        advance(lexer);
    }
    buffer[i] = '\0';
    void* type_ptr = ht_get(keywords, buffer, NULL);
    int type = (type_ptr != NULL) ? (int)(size_t)type_ptr : ID;
    return initToken(type, strdup(buffer));
}

TOKEN* collectNumber(LEXER* lexer) {
    char buffer[1024];
    int i = 0;
    while(isdigit(lexer->c) || lexer->c == '.'){
        if(i < 1023) buffer[i++] = lexer->c;
        advance(lexer);
    }
    buffer[i] = '\0';
    return initToken(NUMBER, strdup(buffer));
}

TOKEN* collectString(LEXER* lexer) {
    advance(lexer);
    char buffer[1024];
    int i = 0;
    while(lexer->c != '"' && lexer->c != '\0'){
        if(i < 1023) buffer[i++] = lexer->c;
        advance(lexer);
    }
    advance(lexer);
    buffer[i] = '\0';
    return initToken(STRING, strdup(buffer));
}

TOKEN* advanceWithToken(LEXER* lexer, TOKEN* token) {
    advance(lexer);
    return token;
}

TOKEN* getNextToken(LEXER* lexer, HashTable* keywords) {
    while(lexer->c != '\0' && (size_t)lexer->i < strlen(lexer->contents)){
        if (lexer->c == ' ' || lexer->c == '\t' || lexer->c == 10 || lexer->c == 13) {
            skipWhitespace(lexer);
            continue;
        }
        if (isalpha(lexer->c) || lexer->c == '_') return collectId(lexer, keywords);
        if (isdigit(lexer->c)) return collectNumber(lexer);
        if (lexer->c == '"') return collectString(lexer);
        if(lexer->c == '<'){
            advance(lexer);
            if(lexer->c == '=') return advanceWithToken(lexer, initToken(LESSOREQ, strdup("<=")));
            return initToken(LESSTH, strdup("<"));
        }
        if(lexer->c == '>'){
            advance(lexer);
            if(lexer->c == '=') return advanceWithToken(lexer, initToken(GREATEROREQ, strdup(">=")));
            return initToken(GREATERTH, strdup(">"));
        }
        if(lexer->c == '&'){
            advance(lexer);
            if(lexer->c == '&') return advanceWithToken(lexer, initToken(AND, strdup("&&")));
            char message[256];
            snprintf(message, sizeof(message), "Unrecognized character [%c, %d] after '&'.", lexer->c, lexer->c);
            ML_ERROR* error = createError(LEXER_ERROR, "lexer.c", __LINE__, message);
            printErrorOnFile(error);
            continue;
        }
        if(lexer->c == '|'){
            advance(lexer);
            if(lexer->c == '|') return advanceWithToken(lexer, initToken(OR, strdup("||")));
            char message[256];
            snprintf(message, sizeof(message), "Unrecognized character [%c, %d] after '|'.", lexer->c, lexer->c);
            ML_ERROR* error = createError(LEXER_ERROR, "lexer.c", __LINE__, message);
            printErrorOnFile(error);
            continue;
        }
        switch(lexer->c){
            case '=':
                advance(lexer);
                if(lexer->c == '=') return advanceWithToken(lexer, initToken(EQEQUAL, strdup("==")));
                return initToken(EQUALS, strdup("="));
            case '.': return advanceWithToken(lexer, initToken(POINT, getCharAsString(lexer)));
            case ',': return advanceWithToken(lexer, initToken(COMMA, getCharAsString(lexer)));
            case ';': return advanceWithToken(lexer, initToken(SEMI, getCharAsString(lexer)));
            case '(': return advanceWithToken(lexer, initToken(LPAREN, getCharAsString(lexer)));
            case ')': return advanceWithToken(lexer, initToken(RPAREN, getCharAsString(lexer)));
            case '{': return advanceWithToken(lexer, initToken(LBRACE, getCharAsString(lexer)));
            case '}': return advanceWithToken(lexer, initToken(RBRACE, getCharAsString(lexer)));
            case '[': return advanceWithToken(lexer, initToken(LBRACKET, getCharAsString(lexer)));
            case ']': return advanceWithToken(lexer, initToken(RBRACKET, getCharAsString(lexer)));
            case '+': return advanceWithToken(lexer, initToken(PLUS, getCharAsString(lexer)));
            case '-': return advanceWithToken(lexer, initToken(MINUS, getCharAsString(lexer)));
            case '*': return advanceWithToken(lexer, initToken(MULTIPLY, getCharAsString(lexer)));
            case '/': return advanceWithToken(lexer, initToken(DIVIDE, getCharAsString(lexer)));
            case '!': 
                advance(lexer);
                if(lexer->c == '=') return advanceWithToken(lexer, initToken(NOTEQUAL, strdup("!=")));
                return initToken(NOT, strdup("!"));
            case '\\': advance(lexer); return getNextToken(lexer, keywords);
            case '\r': advance(lexer); return getNextToken(lexer, keywords);
            default: {
                char message[256];
                snprintf(message, sizeof(message), "Unrecognized character [%c, %d].", lexer->c, lexer->c);
                ML_ERROR* error = createError(LEXER_ERROR, "lexer.c", __LINE__, message);
                printErrorOnFile(error);
                advance(lexer);
                continue;
            }
        }
    }
    return initToken(TEOF, strdup("\0"));
}
