#ifndef TREE_H
#define TREE_H
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>

typedef struct TreeNode {
    void *data;
    char* op_name;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

typedef struct BinaryTree {
    TreeNode *root;
    int (*compare)(void*, void*);
} BinaryTree;

BinaryTree* tree_create(int (*compare)(void*, void*));

void tree_insert(BinaryTree *tree, void *data);

void* tree_search(BinaryTree *tree, void *data);

void tree_inorder(BinaryTree *tree, void (*action)(void*));

void tree_destroy(BinaryTree *tree, void (*destroy_data)(void*));

void tree_save_txt(BinaryTree *tree, const char *filename, void (*write_func)(FILE*, void*));

#endif
