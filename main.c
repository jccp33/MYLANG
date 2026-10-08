#include <stdio.h>
#include <stdlib.h>
#include <hashtable.h>
#include "ast.h"
#include "error.h"
#include "interpreter.h"
#include "io.h"
#include "lexer.h"
#include "parser.h"
#include "semantics.h"
#include "token.h"
#include "tree.h"

HashTable *init_keywords()
{
	HashTable *table = ht_create(NULL);
	ht_insert(table, "var", (void *)VAR);
	ht_insert(table, "print", (void *)PRINT);
	ht_insert(table, "if", (void *)IF);
	ht_insert(table, "else", (void *)ELSE);
	ht_insert(table, "while", (void *)WHILE);
	ht_insert(table, "function", (void *)FUNCTION);
	return table;
}

int main(int argc, char *argv[])
{
	while (argc < 2)
	{
		printf("*** USAGE: ./mylang.exe <<source_file>> ***\n");
		printf("***  Provide a source file to execute.  ***\n");
		return 1;
	}

	char *path = argv[1];
	char *source = read_file(path);
	if (source == NULL)
	{
		char error_message[256];
		snprintf(error_message, sizeof(error_message),
				 "Failed to read source file: %s", argv[1]);
		ML_ERROR *error = createError(IO_ERROR, "main.c", __LINE__, error_message);
		printErrorOnFile(error);
		return 1;
	}

	HashTable *keywords = init_keywords();
	LEXER *lexer = initLexer(source);
	PARSER *parser = init_parser(lexer, keywords);
	TreeNode *root = parse(parser);
	TOKEN *token = getNextToken(lexer, keywords);

	// printf("\n---------- KEYWORDS ----------- \n");
	// printf("Bucket [##] : [key , value]\n");
	// printf("------------------------------- \n");
	// for (size_t i = 0; i < keywords->size; i++) {
	//     if (keywords->buckets[i] != NULL) {
	//         NODE* curr = keywords->buckets[i]->head;
	//         while (curr != NULL) {
	//             HTEntry* entry = (HTEntry*)curr->data;
	//             printf("Bucket [%zu] : [%s , %d]\n", i, entry->key,
	//             (int)(size_t)entry->value); curr = curr->next;
	//         }
	//     }
	// }


	if (root != NULL)
	{
		Environment *global_env = envCreate(NULL);
		analyze(root, global_env);
		preDeclareGlobals(root, global_env);
		// print ast
		//printf("\nSemantic analysis completed successfully.\n");
		//printf("--- INIT AST ---\n");
		//print_ast(root, 0);
		//printf("--- AST  END ---\n\n");
		// printf("\nExecuting AST...\n");
		executeAst(root, global_env);
		envDestroy(global_env);
	}
	else
	{
		printf("main.c:%d: Parsing failed. No AST generated.\n", __LINE__);
	}

	// tokens
	while (token->type != TEOF)
	{
		printf("Token [Type = %d | Value = %s]\n", token->type, token->value);
		free(token->value);
		free(token);
		token = getNextToken(lexer, keywords);
	}

	// free tree
	if (root)
		free_ast(root);
	// free last token
	if (parser && parser->current_token)
	{
		free_token(parser->current_token);
	}
	// free parser y lexer
	free(parser);
	free(lexer);
	// free keywords
	ht_destroy(keywords, NULL);
	// free token
	if (token)
		free_token(token);

	// printf("\n[SYSTEM] Memory freed successfully.\n");
	// printf("\n----------------------------------------------\n\n");

	return 0;
}
