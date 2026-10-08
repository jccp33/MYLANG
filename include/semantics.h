#ifndef SYM_H
#define SYM_H
#include <tree.h>
#include <hashtable.h>
#include "ast.h"
#include "error.h"

typedef struct {
    char* name;
    enum {
        SYM_NUMBER,
        SYM_STRING,
        SYM_FUNCTION
    } type;
    union {
        double number_val;
        char* string_val;
        struct {
            TreeNode* params;
            TreeNode* body;
        } func;
    } data;
} Symbol;

typedef struct Environment {
    HashTable* table;
    struct Environment* parent;
} Environment;

Environment* envCreate(Environment* parent);

void envPut(Environment* env, char* name, Symbol* sym);

Symbol* envGet(Environment* env, char* name);

void envDestroy(Environment* env);

Symbol* createSymbolNumber(char* name, double val);

Symbol* createSymbolString(char* name, char* val);

void analyze(TreeNode* node, Environment* env);

void analyze_parameters(TreeNode* node, Environment* env);

#endif
