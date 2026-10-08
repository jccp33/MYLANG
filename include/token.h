#ifndef TOKEN_H
#define TOKEN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct TOKEN {
	enum {
		ID,          // 0
		EQUALS,      // 1
		STRING,      // 2
		COMMA,       // 3
		SEMI,        // 4
		POINT,       // 5
		LPAREN,      // 6
		RPAREN,      // 7
		LBRACE,      // 8
		RBRACE,      // 9
		LBRACKET,    // 10
		RBRACKET,    // 11
		PLUS,        // 12
		MINUS,       // 13
		MULTIPLY,    // 14
		DIVIDE,      // 15
		NUMBER,      // 16
		VAR,         // 17
		PRINT,       // 18
		LESSTH,      // 19
		GREATERTH,   // 20
		LESSOREQ,    // 21
		GREATEROREQ, // 22
		AND,         // 23
		OR,          // 24
		NOT,         // 25
		EQEQUAL,     // 26
		NOTEQUAL,    // 27
		IF,          // 28
		ELSE,        // 29
		WHILE,       // 30
		FUNCTION,    // 31
		TEOF         // 32
	} type;
	char *value;
} TOKEN;

TOKEN* initToken(int type, char* value);

void free_token(TOKEN* token);

#endif
