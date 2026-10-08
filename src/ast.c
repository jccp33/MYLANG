#include "error.h"
#include "ast.h"
#include "tree.h"
#include <stdio.h>

AST* init_AST(int type) {
    AST* ast = (AST*)malloc(sizeof(AST));
    if (!ast) {
        ML_ERROR* error = createError(AST_ERROR, "ast.c", __LINE__, "Ast cannot be initialized (malloc).");
        printErrorOnFile(error);
        exit(EXIT_FAILURE);
    }
    ast->type = type;
    memset(&ast->data, 0, sizeof(ast->data));
    ast->line = 0; 
    return ast;
}

void destroyAstData(void* data) {
    if (!data) return;
    AST* ast = (AST*)data;
    switch (ast->type) {
        case AST_VARIABLE:
        case AST_DEFINITION:
            if (ast->data.var.name) free(ast->data.var.name);
            break;
        case AST_FUNCTION_DECL:
        case AST_FUNCTION_CALL:
            if (ast->data.func.name) free(ast->data.func.name);
            if (ast->data.func.args) free(ast->data.func.args);
            break;
        case AST_STRING:
            if (ast->data.string_val) free(ast->data.string_val);
            break;
        case AST_COMPOUND:
            if (ast->data.compound.children) free(ast->data.compound.children);
            break;
        default:
            break;
    }
    free(ast);
}

TreeNode* createAstNode(int type, int line){
    AST* ast = init_AST(type);
    ast->line = line;
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        ML_ERROR* error = createError(AST_ERROR, "ast.c", __LINE__, "TreeNode cannot be initialized (malloc).");
        printErrorOnFile(error);
        exit(EXIT_FAILURE);
    }
    node->data = (void*)ast;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void free_ast(TreeNode* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    AST* ast = (AST*)node->data;
    if (ast) {
        switch (ast->type) {
            case AST_DEFINITION:
            case AST_VARIABLE:
            case AST_ASSIGN:
            case AST_BINARY_OP:
                if (ast->data.var.name) free(ast->data.var.name);
                break;
            case AST_STRING:
                if (ast->data.string_val) free(ast->data.string_val);
                break;
            case AST_CALL:
            case AST_FUNCTION_DECL:
                if (ast->data.func.name) free(ast->data.func.name);
                break;
            default:
                break;
        }
        free(ast);
    }
    free(node);
}

void print_ast(TreeNode* node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) {
        printf("%c   ", 179);
    }
    printf("%c%c%c ", 195, 196, 196);
    AST* ast = (AST*)node->data;
    if (!ast) {
        printf("ERROR: AST data is NULL\n");
        return;
    }
    switch (ast->type) {
        case AST_DEFINITION:    printf("VAR_DEF: %s\n", ast->data.var.name); break;
        case AST_VARIABLE:      printf("VAR: %s\n", ast->data.var.name); break;
        case AST_NUMBER:        printf("NUMBER: %g\n", ast->data.number_val); break;
        case AST_BINARY_OP:     printf("BIN_OP: %s\n", ast->data.var.name); break;
        case AST_FUNCTION_DECL: printf("FUNC_DECL: %s\n", ast->data.func.name); break;
        case AST_FUNCTION_CALL: printf("FUNC_CALL: %s\n", ast->data.func.name); break;
        case AST_STRING:        printf("STRING: \"%s\"\n", ast->data.string_val); break;
        case AST_COMPOUND:      printf("BLOCK {}\n"); break;
        case AST_ASSIGN:        printf("ASSIGN: %s\n", ast->data.var.name); break;
        case AST_CALL:          printf("CALL: %s\n", ast->data.var.name); break;
        case AST_UNARY_OP:      printf("UNARY_OP: %s\n", ast->data.bool_op.op); break;
        case AST_COMPARISON:    printf("COMPARISON_OP: %s\n", ast->data.bool_op.op); break;
        case AST_LOGICAL:       printf("LOGICAL_OP: %s\n", ast->data.bool_op.op); break;
        case AST_NOOP:          printf("NOOP\n"); break;
        case AST_IF:            printf("IF\n"); break;
        case AST_WHILE:         printf("WHILE\n"); break;
        case AST_FUNCTION_ARG:  printf("FUNC_ARG\n"); break;
        default:                printf("UNDEFINED TYPE: %d\n", ast->type); break;
    }
    if (node->left)  print_ast(node->left, indent + 1);
    if (node->right) print_ast(node->right, indent + 1);
}
