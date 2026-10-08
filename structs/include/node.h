#ifndef NODE_H
#define NODE_H
#include <stdlib.h>
#include <stdio.h>

typedef struct NODE {
    void *data;
    struct NODE *next;
    struct NODE *prev;
} NODE;

NODE* create_node(void *data);

void destroy_node(NODE *NODE);

#endif
