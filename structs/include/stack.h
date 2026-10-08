#ifndef STACK_H
#define STACK_H
#include "list.h"
#include <stdio.h>

typedef LinkedList Stack;

Stack* stack_create();

void stack_push(Stack *stack, void *data);

void* stack_pop(Stack *stack);

void* stack_peek(Stack *stack);

bool stack_is_empty(Stack *stack);

void stack_destroy(Stack *stack, void (*destroy_data)(void*));

#endif
