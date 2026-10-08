#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "ast.h"
#include "semantics.h"

typedef struct {
    enum { 
        VAL_NUMBER, 
        VAL_STRING, 
        VAL_VOID 
    } type;
    union {
        double number;
        char* string;
    } as;
} RuntimeValue;

RuntimeValue evaluateAst(TreeNode* node, Environment* env);

void processAst(TreeNode* node, Environment* env);

void preDeclareGlobals(TreeNode* node, Environment* env);

void executeAst(TreeNode* root, Environment* env);

#endif
