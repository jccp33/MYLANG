#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "semantics.h"
#include "error.h"

void preDeclareGlobals(TreeNode* node, Environment* env) {
    if (!node) return;
    AST* ast = (AST*)node->data;
    if (ast->type == AST_DEFINITION) {
        if (!envGet(env, ast->data.var.name)) {
            envPut(env, ast->data.var.name, createSymbolNumber(ast->data.var.name, 0));
        }
    } else if (ast->type == AST_COMPOUND) {
        preDeclareGlobals(node->left, env);
        preDeclareGlobals(node->right, env);
    }
}

static RuntimeValue makeVoidValue(void) {
    RuntimeValue value = {VAL_VOID, {0}};
    return value;
}

RuntimeValue evaluateAst(TreeNode* node, Environment* env){
    if (!node) return makeVoidValue();
    AST* ast = (AST*)node->data;
    RuntimeValue result = makeVoidValue();
    switch (ast->type) {
        case AST_NUMBER:
            result.type = VAL_NUMBER;
            result.as.number = ast->data.number_val;
            break;
        case AST_STRING:
            result.type = VAL_STRING;
            result.as.string = strdup(ast->data.string_val);
            break;
        case AST_UNARY_OP:
            RuntimeValue operand = evaluateAst(node->right, env);
            if (strcmp(ast->data.bool_op.op, "-") == 0) {
                if (operand.type != VAL_NUMBER) {
                    ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, "Unary minus requires numeric operand");
                    printErrorOnFile(error);
                    exit(EXIT_FAILURE);
                }
                result.type = VAL_NUMBER;
                result.as.number = -operand.as.number;
            } else {
                ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, "Unknown unary operator");
                printErrorOnFile(error);
                exit(EXIT_FAILURE);
            }
            break;
        case AST_VARIABLE: {
            Symbol* sym = envGet(env, ast->data.var.name);
            if (!sym) {
                sym = createSymbolNumber(ast->data.var.name, 0);
                envPut(env, ast->data.var.name, sym);
            }
            if (sym->type == SYM_NUMBER) {
                result.type = VAL_NUMBER;
                result.as.number = sym->data.number_val;
            } else if (sym->type == SYM_STRING) {
                result.type = VAL_STRING;
                result.as.string = strdup(sym->data.string_val);
            }
            break;
        }
        case AST_BINARY_OP: {
            RuntimeValue left = evaluateAst(node->left, env);
            RuntimeValue right = evaluateAst(node->right, env);
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
                ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, "Binary operations require numeric operands");
                printErrorOnFile(error);
                exit(EXIT_FAILURE);
            }
            result.type = VAL_NUMBER;
            if (strcmp(ast->data.var.name, "+") == 0) {
                result.as.number = left.as.number + right.as.number;
            } else if (strcmp(ast->data.var.name, "-") == 0) {
                result.as.number = left.as.number - right.as.number;
            } else if (strcmp(ast->data.var.name, "*") == 0) {
                result.as.number = left.as.number * right.as.number;
            } else if (strcmp(ast->data.var.name, "/") == 0) {
                if (right.as.number == 0) {
                    ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, "Division by zero");
                    printErrorOnFile(error);
                    exit(EXIT_FAILURE);
                }
                result.as.number = left.as.number / right.as.number;
            }
            break;
        }
        case AST_COMPARISON: {
            RuntimeValue left = evaluateAst(node->left, env);
            RuntimeValue right = evaluateAst(node->right, env);
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
                ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, "Comparison operations require numeric operands");
                printErrorOnFile(error);
                exit(EXIT_FAILURE);
            }
            result.type = VAL_NUMBER; // Use 0/1 for false/true
            if (strcmp(ast->data.bool_op.op, "<") == 0) {
                result.as.number = left.as.number < right.as.number ? 1 : 0;
            } else if (strcmp(ast->data.bool_op.op, "<=") == 0) {
                result.as.number = left.as.number <= right.as.number ? 1 : 0;
            } else if (strcmp(ast->data.bool_op.op, ">") == 0) {
                result.as.number = left.as.number > right.as.number ? 1 : 0;
            } else if (strcmp(ast->data.bool_op.op, ">=") == 0) {
                result.as.number = left.as.number >= right.as.number ? 1 : 0;
            } else if (strcmp(ast->data.bool_op.op, "==") == 0) {
                result.as.number = left.as.number == right.as.number ? 1 : 0;
            } else if (strcmp(ast->data.bool_op.op, "!=") == 0) {
                result.as.number = left.as.number != right.as.number ? 1 : 0;
            }
            break;
        }
        case AST_LOGICAL: {
            RuntimeValue left = evaluateAst(node->left, env);
            RuntimeValue right = evaluateAst(node->right, env);
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
                ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, "Logical operations require numeric operands");
                printErrorOnFile(error);
                exit(EXIT_FAILURE);
            }
            result.type = VAL_NUMBER;
            if (strcmp(ast->data.bool_op.op, "&&") == 0) {
                result.as.number = (left.as.number != 0 && right.as.number != 0) ? 1 : 0;
            } else if (strcmp(ast->data.bool_op.op, "||") == 0) {
                result.as.number = (left.as.number != 0 || right.as.number != 0) ? 1 : 0;
            }
            break;
        }
        case AST_NOOP: {
            return evaluateAst(node->left, env);
        }
        default:
            break;
    }
    return result;
}

static void executeCall(TreeNode* node, Environment* env) {
    AST* ast = (AST*)node->data;
    if (strcmp(ast->data.func.name, "print") == 0) {
        TreeNode* arg_node = node->left;
        while (arg_node) {
            RuntimeValue arg = evaluateAst(arg_node->left, env);
            if (arg.type == VAL_NUMBER) {
                printf("%g", arg.as.number);
            } else if (arg.type == VAL_STRING) {
                printf("%s", arg.as.string);
                free(arg.as.string);
            }
            arg_node = arg_node->right;
            if (arg_node) printf(" ");
        }
        printf("\n");
        return;
    }
    Symbol* func_sym = envGet(env, ast->data.func.name);
    if (!func_sym || func_sym->type != SYM_FUNCTION) {
        char message[256];
        snprintf(message, sizeof(message), "Undefined function: %s", ast->data.func.name);
        ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, message);
        printErrorOnFile(error);
        exit(EXIT_FAILURE);
    }
    Environment* func_env = envCreate(env);
    TreeNode* param_node = func_sym->data.func.params;
    TreeNode* arg_node = node->left;
    while (param_node) {
        if (!arg_node) {
            char message[256];
            snprintf(message, sizeof(message), "Function '%s' expects more arguments.", ast->data.func.name);
            ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, message);
            printErrorOnFile(error);
            exit(EXIT_FAILURE);
        }
        AST* param_ast = (AST*)param_node->left->data;
        RuntimeValue arg_value = evaluateAst(arg_node->left, env);
        if (param_ast->type != AST_VARIABLE) {
            char message[256];
            snprintf(message, sizeof(message), "Invalid function parameter in '%s'.", ast->data.func.name);
            ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, message);
            printErrorOnFile(error);
            exit(EXIT_FAILURE);
        }
        if (arg_value.type == VAL_NUMBER) {
            Symbol* sym = createSymbolNumber(param_ast->data.var.name, arg_value.as.number);
            envPut(func_env, param_ast->data.var.name, sym);
        } else if (arg_value.type == VAL_STRING) {
            Symbol* sym = createSymbolString(param_ast->data.var.name, arg_value.as.string);
            envPut(func_env, param_ast->data.var.name, sym);
            free(arg_value.as.string);
        }
        param_node = param_node->right;
        arg_node = arg_node->right;
    }
    if (arg_node) {
        char message[256];
        snprintf(message, sizeof(message), "Function '%s' received too many arguments.", ast->data.func.name);
        ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, message);
        printErrorOnFile(error);
        exit(EXIT_FAILURE);
    }
    executeAst(func_sym->data.func.body, func_env);
    envDestroy(func_env);
}

void processAst(TreeNode* node, Environment* env){
    if (!node) return;
    AST* ast = (AST*)node->data;
    switch (ast->type) {
        case AST_DEFINITION: {
            RuntimeValue value = evaluateAst(node->right, env);
            Symbol* existing = envGet(env, ast->data.var.name);
            if (existing) {
                if (existing->type == SYM_STRING && existing->data.string_val) {
                    free(existing->data.string_val);
                }
                if (value.type == VAL_NUMBER) {
                    existing->type = SYM_NUMBER;
                    existing->data.number_val = value.as.number;
                } else if (value.type == VAL_STRING) {
                    existing->type = SYM_STRING;
                    existing->data.string_val = strdup(value.as.string);
                    free(value.as.string);
                }
            } else {
                if (value.type == VAL_NUMBER) {
                    Symbol* sym = createSymbolNumber(ast->data.var.name, value.as.number);
                    envPut(env, ast->data.var.name, sym);
                } else if (value.type == VAL_STRING) {
                    Symbol* sym = createSymbolString(ast->data.var.name, value.as.string);
                    envPut(env, ast->data.var.name, sym);
                    free(value.as.string);
                }
            }
            break;
        }
        case AST_ASSIGN: {
            RuntimeValue value = evaluateAst(node->right, env);
            Symbol* sym = envGet(env, ast->data.var.name);
            if (!sym) {
                // Declare the variable
                if (value.type == VAL_NUMBER) {
                    sym = createSymbolNumber(ast->data.var.name, value.as.number);
                    envPut(env, ast->data.var.name, sym);
                } else if (value.type == VAL_STRING) {
                    sym = createSymbolString(ast->data.var.name, value.as.string);
                    envPut(env, ast->data.var.name, sym);
                    free(value.as.string);
                } else {
                    ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, "Invalid value type for new variable");
                    printErrorOnFile(error);
                    exit(EXIT_FAILURE);
                }
            } else {
                // Update existing
                if (sym->type == SYM_NUMBER && value.type == VAL_NUMBER) {
                    sym->data.number_val = value.as.number;
                } else if (sym->type == SYM_STRING && value.type == VAL_STRING) {
                    free(sym->data.string_val);
                    sym->data.string_val = strdup(value.as.string);
                    free(value.as.string);
                } else {
                    ML_ERROR* error = createError(RUNTIME_ERROR, "interpreter.c", __LINE__, "Type mismatch in assignment");
                    printErrorOnFile(error);
                    exit(EXIT_FAILURE);
                }
            }
            break;
        }
        case AST_CALL:
            executeCall(node, env);
            break;
        case AST_FUNCTION_DECL:
            // Function declarations are already handled in semantic analysis
            break;
        case AST_IF:
            RuntimeValue condition = evaluateAst(node->left, env);
            if(condition.as.number != 0){
                executeAst(node->right->left, env);
            } else if (node->right->right) {
                executeAst(node->right->right, env);
            }
            break;
        case AST_WHILE:
            RuntimeValue while_cond = evaluateAst(node->left, env);
            while(while_cond.type==VAL_NUMBER && while_cond.as.number!=0){
                executeAst(node->right, env);
                while_cond = evaluateAst(node->left, env);
            }
        default:
            break;
    }
}

void executeAst(TreeNode* root, Environment* env){
    if (!root) return;
    AST* ast = (AST*)root->data;
    switch (ast->type) {
        case AST_COMPOUND:
            executeAst(root->left, env);
            executeAst(root->right, env);
            break;
        default:
            processAst(root, env);
            break;
    }
}
