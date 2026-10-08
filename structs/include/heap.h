#ifndef HEAP_H
#define HEAP_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Heap {
    void **data;
    size_t size;
    size_t capacity;
    int (*compare)(void*, void*);
} Heap;

Heap* heap_create(size_t initial_capacity, int (*compare)(void*, void*));

void heap_push(Heap *heap, void *data);

void* heap_pop(Heap *heap);

void* heap_peek(Heap *heap);

bool heap_is_empty(Heap *heap);

void heap_destroy(Heap *heap, void (*destroy_data)(void*));

void heap_save_txt(Heap *heap, const char *filename, void (*write_func)(FILE*, void*));

#endif
