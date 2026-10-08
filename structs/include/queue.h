#ifndef QUEUE_H
#define QUEUE_H
#include "list.h"
#include <stdio.h>

typedef LinkedList Queue;

Queue* queue_create();

void queue_enqueue(Queue *queue, void *data);

void* queue_dequeue(Queue *queue);

void* queue_peek(Queue *queue);

bool queue_is_empty(Queue *queue);

void queue_destroy(Queue *queue, void (*destroy_data)(void*));

#endif
