#ifndef LIST_H
#define LIST_H
#include "node.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct LinkedList {
    NODE *head;
    NODE *tail;
    size_t size;
} LinkedList;

LinkedList* list_create();

void list_add_front(LinkedList *list, void *data);

void list_add_back(LinkedList *list, void *data);

void* list_get_at(LinkedList *list, int index);

bool list_is_empty(LinkedList *list);

void list_destroy(LinkedList *list, void (*destroy_data)(void*));

NODE* list_find(LinkedList *list, void *data, int (*compare)(void*, void*));

void list_remove_node(LinkedList *list, NODE *node, void (*destroy_data)(void*));

void list_sort(LinkedList *list, int (*compare)(void*, void*));

void list_foreach(LinkedList *list, void (*action)(void*));

void list_save_txt(LinkedList *list, const char *filename, void (*write_func)(FILE*, void*));

void list_load_txt(LinkedList *list, const char *filename, void* (*parse_func)(char*));

#endif
