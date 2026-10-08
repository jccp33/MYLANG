#ifndef AST_H
#define AST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"

typedef struct AST {
    enum {
        AST_DEFINITION,    // 0
        AST_VARIABLE,      // 1
        AST_NUMBER,        // 2
        AST_UNARY_OP,      // 3
        AST_BINARY_OP,     // 4
        AST_FUNCTION_DECL, // 5
        AST_FUNCTION_CALL, // 6
        AST_STRING,        // 7
        AST_COMPOUND,      // 8
        AST_CALL,          // 9
        AST_ASSIGN,        // 10
        AST_COMPARISON,    // 11
        AST_LOGICAL,       // 12
        AST_BOOLEAN,       // 13
        AST_NOOP,          // 14
        AST_FUNCTION_ARG,  // 15
        AST_IF,            // 16
        AST_WHILE          // 17
    } type;
    int line; 
    union {
        char* string_val;
        struct {
            char* name;
            struct AST* value;
        } var;
        double number_val;
        struct {
            char* name;
            struct AST** args;
            size_t args_size;
        } func;
        struct {
            struct AST** children;
            size_t size;
        } compound;
        struct {
            char* op;
        } bool_op;
    } data;
} AST;

AST* init_AST(int type);

void print_ast(TreeNode* node, int indent);

void free_ast(TreeNode* node);

#endif
