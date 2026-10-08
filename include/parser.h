#ifndef PARSER_H
#define PARSER_H
#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "hashtable.h"
#include "tree.h"

typedef struct FUNCTION_ARG {
    enum {
        AST_ARG_NUMBER,
        AST_ARG_STRING,
    } type;
    union {
        double number_val;
        char* string_val;
    } data;
} FUNCTION_ARG;

typedef struct PARSER {
    LEXER* lexer;
    TOKEN* current_token;
    TOKEN* previous_token;
    HashTable* symbol_table;
    BinaryTree* ast;
} PARSER;

PARSER* init_parser(LEXER* lexer, HashTable* external_st);

void eat(PARSER* parser, int token_type);

TreeNode* parseVariableDef(PARSER* parser);

TreeNode* parseVariable(PARSER* parser);

TreeNode* parseId(PARSER* parser);

TreeNode* parseTerm(PARSER* parser);

TreeNode* parseFactor(PARSER* parser);

TreeNode* parseString(PARSER* parser);

TreeNode* parseExpr(PARSER* parser);

TreeNode* parseLogicalExpr(PARSER* parser);

TreeNode* parseLogicalOr(PARSER* parser);

TreeNode* parseLogicalAnd(PARSER* parser);

TreeNode* parseComparison(PARSER* parser);

TreeNode* parseFunction(PARSER* parser);

TreeNode* parseCall(PARSER* parser);

TreeNode* parseArguments(PARSER* parser);

TreeNode* parseCompound(PARSER* parser);

TreeNode* parseStatement(PARSER* parser);

TreeNode* parseAssignmentOrCall(PARSER* parser);

TreeNode* parseExpressionStatement(PARSER* parser);

TreeNode* parseStatements(PARSER* parser);

TreeNode* parseIf(PARSER* parser);

TreeNode* parseWhile(PARSER* parser);

TreeNode* parse(PARSER* parser);

TreeNode* create_ast_node_wrapper(int type);

#endif
