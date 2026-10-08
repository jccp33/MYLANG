#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "ast.h"
#include "error.h"

Environment* envCreate(Environment* parent) {
    Environment* env = malloc(sizeof(Environment));
    env->table = ht_create(NULL);
    env->parent = parent;
    return env;
}

void envPut(Environment* env, char* name, Symbol* sym) {
    ht_insert(env->table, name, (void*)sym);
}

Symbol* envGet(Environment* env, char* name) {
    Environment* current = env;
    while (current != NULL) {
        Symbol* sym = (Symbol*)ht_get(current->table, name, NULL);
        if (sym != NULL) return sym;
        current = current->parent;
    }
    return NULL;
}

void envDestroy(Environment* env) {
    if (!env) return;
    for (size_t i = 0; i < env->table->size; i++) {
        if (env->table->buckets[i] != NULL) {
            NODE* curr = env->table->buckets[i]->head;
            while (curr != NULL) {
                HTEntry* entry = (HTEntry*)curr->data;
                Symbol* sym = (Symbol*)entry->value;
                if (sym->type == SYM_STRING && sym->data.string_val) {
                    free(sym->data.string_val);
                }
                if (sym->name) free(sym->name);
                free(sym);
                curr = curr->next;
            }
        }
    }
    ht_destroy(env->table, NULL);
    free(env);
}

Symbol* createSymbolNumber(char* name, double val) {
    Symbol* sym = malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->type = SYM_NUMBER;
    sym->data.number_val = val;
    return sym;
}

Symbol* createSymbolString(char* name, char* val) {
    Symbol* sym = malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->type = SYM_STRING;
    sym->data.string_val = strdup(val);
    return sym;
}

void analyze(TreeNode* node, Environment* env){
    if (!node) return;
    AST* ast = (AST*)node->data;
    switch (ast->type) {
        case AST_UNARY_OP:
            analyze(node->right, env);
            break;
        case AST_COMPOUND:
            analyze(node->left, env);
            analyze(node->right, env);
            break;
        case AST_DEFINITION:
            analyze(node->right, env);
            if (ht_get(env->table, ast->data.var.name, NULL)) {
                char message[256];
                snprintf(message, sizeof(message), "'%s' is already defined in this scope.\n", ast->data.var.name);
                ML_ERROR *error = createError(SEMANTIC_ERROR, "semantics.c", __LINE__, message);
                printErrorOnFile(error);
                exit(EXIT_FAILURE);
            }
            envPut(env, ast->data.var.name, createSymbolNumber(ast->data.var.name, 0));
            break;
        case AST_ASSIGN:
            analyze(node->right, env);
            if (!envGet(env, ast->data.var.name)) {
                char message[256];
                snprintf(message, sizeof(message), "Trying to assign to undeclared variable: '%s'\n", ast->data.var.name);
                ML_ERROR *error = createError(SEMANTIC_ERROR, "semantics.c", __LINE__, message);
                printErrorOnFile(error);
                exit(EXIT_FAILURE);
            }
            break;
        case AST_VARIABLE:
            if (!envGet(env, ast->data.var.name)) {
                char message[256];
                snprintf(message, sizeof(message), "'%s' used before being declared.\n", ast->data.var.name);
                ML_ERROR *error = createError(SEMANTIC_ERROR, "semantics.c", __LINE__, message);
                printErrorOnFile(error);
                exit(EXIT_FAILURE);
            }
            break;
        case AST_BINARY_OP:
        case AST_LOGICAL:
        case AST_COMPARISON:
        case AST_NOOP:
            analyze(node->left, env);
            analyze(node->right, env);
            break;
        case AST_FUNCTION_DECL: {
            // save function in env
            if (ht_get(env->table, ast->data.func.name, NULL)) {
                char message[256];
                snprintf(message, sizeof(message), "Function '%s' is already defined in this scope.\n", ast->data.func.name);
                ML_ERROR *error = createError(SEMANTIC_ERROR, "semantics.c", __LINE__, message);
                printErrorOnFile(error);
                exit(EXIT_FAILURE);
            }
            Symbol* func_sym = malloc(sizeof(Symbol));
            func_sym->name = strdup(ast->data.func.name);
            func_sym->type = SYM_FUNCTION;
            func_sym->data.func.body = node->right;
            func_sym->data.func.params = node->left;
            envPut(env, ast->data.func.name, func_sym);
            // environment for function body
            Environment* local_env = envCreate(env);
            // add parameters to local environment
            if (node->left) {
                analyze_parameters(node->left, local_env);
            }
            analyze(node->right, local_env);
            envDestroy(local_env);
            break;
        }
        case AST_CALL: {
            // find function
            Symbol* new_func = envGet(env, ast->data.func.name);
            // on error
            if (!new_func && strcmp(ast->data.func.name, "print") != 0) {
                char message[256];
                snprintf(message, sizeof(message), "Function '%s' is not defined.\n", ast->data.func.name);
                ML_ERROR *error = createError(SEMANTIC_ERROR, "semantics.c", __LINE__, message);
                printErrorOnFile(error);
                exit(EXIT_FAILURE);
            }
            // arguments
            analyze(node->left, env);
            break;
        }
        default:
            break;
    }
}

void analyze_parameters(TreeNode* node, Environment* env) {
    if (!node) return;
    AST* ast = (AST*)node->left->data;
    if (ast->type == AST_VARIABLE) {
        Symbol* param = createSymbolNumber(ast->data.var.name, 0);
        envPut(env, ast->data.var.name, param);
    }
    if (node->right) {
        analyze_parameters(node->right, env);
    }
}
