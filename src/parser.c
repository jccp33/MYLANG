#include <tree.h>
#include "parser.h"
#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "token.h"

PARSER *init_parser(LEXER *lexer, HashTable *external_st)
{
	PARSER *parser = malloc(sizeof(PARSER));
	if (!parser)
	{
		ML_ERROR *error = createError(3, "parser.c", __LINE__, "Parser cannot be initialized (malloc).");
		printErrorOnFile(error);
		exit(EXIT_FAILURE);
	}
	parser->lexer = lexer;
	parser->current_token = getNextToken(lexer, external_st);
	parser->previous_token = NULL;
	parser->symbol_table = external_st;
	parser->ast = tree_create(NULL);
	if (!parser->ast)
	{
		ML_ERROR *error = createError(3, "parser.c", __LINE__, "Ast cannot be initialized (tree_create).");
		printErrorOnFile(error);
		free(parser);
		exit(EXIT_FAILURE);
	}
	return parser;
}

void eat(PARSER *parser, int token_type)
{
	if ((int)parser->current_token->type != token_type)
	{
		char message[256];
		snprintf(message, sizeof(message), "Unexpected token (%d != %d).", parser->current_token->type, token_type);
		ML_ERROR *error = createError(3, "parser.c", __LINE__, message);
		printErrorOnFile(error);
		exit(EXIT_FAILURE);
	}
	if (parser->previous_token != NULL)
	{
		if (parser->previous_token->value) free(parser->previous_token->value);
		free(parser->previous_token);
	}
	parser->previous_token = parser->current_token;
	parser->current_token = getNextToken(parser->lexer, parser->symbol_table);
}

TreeNode *parseVariableDef(PARSER *parser)
{
	eat(parser, VAR);
	char *var_name = strdup(parser->current_token->value);
	eat(parser, ID);
	eat(parser, EQUALS);
	TreeNode *node = create_ast_node_wrapper(AST_DEFINITION);
	AST *ast = (AST *)node->data;
	ast->data.var.name = var_name;
	node->right = parseLogicalExpr(parser);
	eat(parser, SEMI);
	return node;
}

TreeNode *parseVariable(PARSER *parser)
{
	TreeNode *node = create_ast_node_wrapper(AST_VARIABLE);
	AST *ast = (AST *)node->data;
	ast->data.var.name = strdup(parser->current_token->value);
	eat(parser, ID);
	return node;
}

TreeNode *parseId(PARSER *parser)
{
	char *id_name = strdup(parser->current_token->value);
	eat(parser, ID);
	if (parser->current_token->type == LPAREN)
	{
		eat(parser, LPAREN);
		TreeNode *main_node = create_ast_node_wrapper(AST_CALL);
		AST *ast = (AST *)main_node->data;
		ast->data.func.name = id_name;
		if (parser->current_token->type != RPAREN)
		{
			main_node->left = parseArguments(parser);
		}
		eat(parser, RPAREN);
		return main_node;
	}
	TreeNode *node = create_ast_node_wrapper(AST_VARIABLE);
	AST *ast = (AST *)node->data;
	ast->data.var.name = id_name;
	return node;
}

TreeNode *parseTerm(PARSER *parser)
{
	TreeNode *node = parseFactor(parser);
	while (parser->current_token->type == MULTIPLY || parser->current_token->type == DIVIDE)
	{
		int op_type = parser->current_token->type;
		eat(parser, op_type);
		TreeNode *new_node = create_ast_node_wrapper(AST_BINARY_OP);
		AST *ast = (AST *)new_node->data;
		ast->data.var.name = strdup(op_type == MULTIPLY ? "*" : "/");
		new_node->left = node;
		new_node->right = parseFactor(parser);
		node = new_node;
	}
	return node;
}

TreeNode *parseFactor(PARSER *parser)
{
	if (parser->current_token->type == PLUS)
	{
		eat(parser, PLUS);
		TreeNode *node = create_ast_node_wrapper(AST_UNARY_OP);
		AST *ast = (AST *)node->data;
		ast->data.bool_op.op = "+";
		node->right = parseFactor(parser);
		return node;
	}
	if (parser->current_token->type == MINUS)
	{
		eat(parser, MINUS);
		TreeNode *node = create_ast_node_wrapper(AST_UNARY_OP);
		AST *ast = (AST *)node->data;
		ast->data.bool_op.op = "-";
		node->right = parseFactor(parser);
		return node;
	}
	if (parser->current_token->type == NUMBER)
	{
		TreeNode *node = create_ast_node_wrapper(AST_NUMBER);
		AST *ast = (AST *)node->data;
		ast->data.number_val = strtod(parser->current_token->value, NULL);
		eat(parser, NUMBER);
		return node;
	}
	if (parser->current_token->type == STRING)
	{
		return parseString(parser);
	}
	if (parser->current_token->type == ID)
	{
		return parseId(parser);
	}
	if (parser->current_token->type == LPAREN)
	{
		eat(parser, LPAREN);
		TreeNode *node = parseExpr(parser);
		eat(parser, RPAREN);
		return node;
	}
	char message[256];
	snprintf(message, sizeof(message), "ERROR: Unexpected token %s (%d).",
			 parser->current_token->value, parser->current_token->type);
	ML_ERROR *error = createError(3, "parser.c", __LINE__, message);
	printErrorOnFile(error);
	exit(EXIT_FAILURE);
}

TreeNode *parseString(PARSER *parser)
{
	TreeNode *node = create_ast_node_wrapper(AST_STRING);
	AST *ast = (AST *)node->data;
	ast->data.string_val = strdup(parser->current_token->value);
	eat(parser, STRING);
	return node;
}

TreeNode *parsePrint(PARSER *parser)
{
	eat(parser, PRINT);
	eat(parser, LPAREN);
	TreeNode *node = create_ast_node_wrapper(AST_CALL);
	AST *ast = (AST *)node->data;
	ast->data.func.name = strdup("print");
	if (parser->current_token->type != RPAREN)
	{
		node->left = parseArguments(parser);
	}
	eat(parser, RPAREN);
	eat(parser, SEMI);
	return node;
}

TreeNode *parseExpr(PARSER *parser)
{
	TreeNode *node = parseTerm(parser);
	while (parser->current_token->type == PLUS || parser->current_token->type == MINUS){
		int op_type = parser->current_token->type;
		eat(parser, op_type);
		TreeNode *new_node = create_ast_node_wrapper(AST_BINARY_OP);
		AST *ast = (AST *)new_node->data;
		ast->data.var.name = strdup(op_type == PLUS ? "+" : "-");
		new_node->left = node;
		new_node->right = parseTerm(parser);
		node = new_node;
	}
	return node;
}

static TOKEN *peekNextToken(PARSER *parser)
{
	int saved_i = parser->lexer->i;
	char saved_c = parser->lexer->c;
	TOKEN *peeked = getNextToken(parser->lexer, parser->symbol_table);
	parser->lexer->i = saved_i;
	parser->lexer->c = saved_c;
	return peeked;
}

TreeNode *parseLogicalExpr(PARSER *parser) { 
	return parseLogicalOr(parser); 
}

TreeNode *parseExpressionStatement(PARSER *parser)
{
	TreeNode *node = parseLogicalExpr(parser);
	eat(parser, SEMI);
	return node;
}

TreeNode *parseLogicalOr(PARSER *parser)
{
	TreeNode *node = parseLogicalAnd(parser);
	while (parser->current_token->type == OR)
	{
		eat(parser, OR);
		TreeNode *new_node = create_ast_node_wrapper(AST_LOGICAL);
		AST *ast = (AST *)new_node->data;
		ast->data.bool_op.op = strdup("||");
		new_node->left = node;
		new_node->right = parseLogicalAnd(parser);
		node = new_node;
	}
	return node;
}

TreeNode *parseLogicalAnd(PARSER *parser)
{
	TreeNode *node = parseComparison(parser);
	while (parser->current_token->type == AND)
	{
		eat(parser, AND);
		TreeNode *new_node = create_ast_node_wrapper(AST_LOGICAL);
		AST *ast = (AST *)new_node->data;
		ast->data.bool_op.op = strdup("&&");
		new_node->left = node;
		new_node->right = parseComparison(parser);
		node = new_node;
	}
	return node;
}

TreeNode *parseComparison(PARSER *parser)
{
	TreeNode *node = parseExpr(parser);
	while (parser->current_token->type == LESSTH ||
		   parser->current_token->type == LESSOREQ ||
		   parser->current_token->type == GREATERTH ||
		   parser->current_token->type == GREATEROREQ ||
		   parser->current_token->type == EQEQUAL ||
		   parser->current_token->type == NOTEQUAL
	) {
		int op_type = parser->current_token->type;
		eat(parser, op_type);
		TreeNode *new_node = create_ast_node_wrapper(AST_COMPARISON);
		AST *ast = (AST *)new_node->data;
		switch (op_type)
		{
		case LESSTH:
			ast->data.bool_op.op = strdup("<");
			break;
		case LESSOREQ:
			ast->data.bool_op.op = strdup("<=");
			break;
		case GREATERTH:
			ast->data.bool_op.op = strdup(">");
			break;
		case GREATEROREQ:
			ast->data.bool_op.op = strdup(">=");
			break;
		case EQEQUAL:
			ast->data.bool_op.op = strdup("==");
			break;
		case NOTEQUAL:
			ast->data.bool_op.op = strdup("!=");
			break;
		}
		new_node->left = node;
		new_node->right = parseExpr(parser);
		node = new_node;
	}
	return node;
}

TreeNode *parseFunction(PARSER *parser)
{
	eat(parser, FUNCTION);
	char *func_name = strdup(parser->current_token->value);
	eat(parser, ID);
	eat(parser, LPAREN);
	TreeNode *main_node = create_ast_node_wrapper(AST_FUNCTION_DECL);
	AST *ast = (AST *)main_node->data;
	ast->data.func.name = func_name;
	if (parser->current_token->type != RPAREN)
	{
		main_node->left = parseArguments(parser);
	}
	eat(parser, RPAREN);
	main_node->right = parseCompound(parser);
	return main_node;
}

TreeNode *parseCall(PARSER *parser)
{
	char *func_name = strdup(parser->current_token->value);
	eat(parser, ID);
	eat(parser, LPAREN);
	TreeNode *node = create_ast_node_wrapper(AST_CALL);
	AST *ast = (AST *)node->data;
	ast->data.func.name = func_name;
	if (parser->current_token->type != RPAREN)
	{
		node->left = parseArguments(parser);
	}
	eat(parser, RPAREN);
	return node;
}

TreeNode *parseArguments(PARSER *parser)
{
	TreeNode *wrapper = create_ast_node_wrapper(AST_FUNCTION_ARG);
	wrapper->left = parseLogicalExpr(parser);
	if (parser->current_token->type == COMMA)
	{
		eat(parser, COMMA);
		wrapper->right = parseArguments(parser);
	}
	return wrapper;
}

TreeNode *parseCompound(PARSER *parser)
{
	eat(parser, LBRACE);
	TreeNode *compound_node = create_ast_node_wrapper(AST_COMPOUND);
	TreeNode *current_node = compound_node;
	while (parser->current_token->type != RBRACE && parser->current_token->type != TEOF)
	{
		TreeNode *statement = parseStatement(parser);
		if (statement != NULL)
		{
			current_node->left = statement;
			if (parser->current_token->type != RBRACE && parser->current_token->type != TEOF)
			{
				current_node->right = create_ast_node_wrapper(AST_COMPOUND);
				current_node = current_node->right;
			}
		}
	}
	eat(parser, RBRACE);
	return compound_node;
}

TreeNode *parseStatement(PARSER *parser)
{
	int type = parser->current_token->type;
	switch (type) {
		case VAR:
			return parseVariableDef(parser);
		case FUNCTION:
			return parseFunction(parser);
		case PRINT:
			return parsePrint(parser);
		case LBRACE:
			return parseCompound(parser);
		case ID: {
			TOKEN *next = peekNextToken(parser);
			int nextType = next->type;
			if (next->value)
				free(next->value);
			free(next);
			if (nextType == EQUALS || nextType == LPAREN)
			{
				return parseAssignmentOrCall(parser);
			}
			return parseExpressionStatement(parser);
		}
		case IF:
			return parseIf(parser); 
		case WHILE:
			return parseWhile(parser);
		default:
			if (type == SEMI)
			{
				eat(parser, SEMI);
				return NULL;
			}else if (type == NUMBER || type == STRING || type == LPAREN || type == PLUS || type == MINUS)
			{
				return parseExpressionStatement(parser);
			}else if (type == TEOF)
			{
				return NULL;
			} else {
				char message[256];
				snprintf(message, sizeof(message), "Invalid statement start: '%s' (%d).",
						parser->current_token->value, parser->current_token->type);
				ML_ERROR *error =
					createError(PARSER_ERROR, "parser.c", __LINE__, message);
				printErrorOnFile(error);
				exit(EXIT_FAILURE);
			}
	}
}

TreeNode *parseAssignmentOrCall(PARSER *parser)
{
	char *name = strdup(parser->current_token->value);
	eat(parser, ID);
	if (parser->current_token->type == EQUALS)
	{
		eat(parser, EQUALS);
		TreeNode *node = create_ast_node_wrapper(AST_ASSIGN);
		AST *ast = (AST *)node->data;
		ast->data.var.name = name;
		node->right = parseLogicalExpr(parser);
		eat(parser, SEMI);
		return node;
	}
	if (parser->current_token->type == LPAREN) {
		eat(parser, LPAREN);
		TreeNode *node = create_ast_node_wrapper(AST_CALL);
		AST *ast = (AST *)node->data;
		ast->data.func.name = name;
		if (parser->current_token->type != RPAREN)
		{
			node->left = parseArguments(parser);
		}
		eat(parser, RPAREN);
		eat(parser, SEMI);
		return node;
	}
	free(name);
	return NULL;
}

TreeNode *parseStatements(PARSER *parser)
{
	TreeNode *statement = parseStatement(parser);
	if (statement == NULL && parser->current_token->type != TEOF && parser->current_token->type != RBRACE)
	{
		return NULL;
	}
	if (statement == NULL) return NULL;
	TreeNode *node = create_ast_node_wrapper(AST_COMPOUND);
	node->left = statement;
	node->right = parseStatements(parser);
	return node;
}

TreeNode* parseIf(PARSER* parser){
	eat(parser, IF);
	eat(parser, LPAREN);
	TreeNode* if_node = create_ast_node_wrapper(AST_IF);
	if_node->left = parseLogicalExpr(parser);
	eat(parser, RPAREN);
	TreeNode* branch_node = create_ast_node_wrapper(AST_NOOP);
    if_node->right = branch_node;
    branch_node->left = parseCompound(parser);
	if (parser->current_token->type == ELSE)
	{
		eat(parser, ELSE);
		branch_node->right = parseCompound(parser);
	}
	return if_node;
}

TreeNode* parseWhile(PARSER* parser){
	eat(parser, WHILE);
	eat(parser, LPAREN);
	TreeNode* while_node = create_ast_node_wrapper(AST_WHILE);
	while_node->left = parseLogicalExpr(parser);
	eat(parser, RPAREN);
	while_node->right = parseCompound(parser);
	return while_node;
}

TreeNode *parse(PARSER *parser)
{
	TreeNode *root = parseStatements(parser);
	parser->ast->root = root;
	return root;
}

TreeNode *create_ast_node_wrapper(int type)
{
	AST *ast_data = calloc(1, sizeof(AST));
	ast_data->type = type;
	TreeNode *node = malloc(sizeof(TreeNode));
	node->data = ast_data;
	node->left = NULL;
	node->right = NULL;
	return node;
}
