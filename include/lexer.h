#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "error.h"
#include "token.h"
#include "hashtable.h"

typedef struct LEXER {
	char* contents;
	int i;
	char c;
} LEXER;

LEXER* initLexer(char* contents);

void advance(LEXER* lexer);

void skipWhitespace(LEXER* lexer);

char* getCharAsString(LEXER* lexer);

TOKEN* collectId(LEXER* lexer, HashTable* keywords);

TOKEN* collectNumber(LEXER* lexer);

TOKEN* collectString(LEXER* lexer);

TOKEN* advanceWithToken(LEXER* lexer, TOKEN* token);

TOKEN* getNextToken(LEXER* lexer, HashTable* keywords);

#endif
