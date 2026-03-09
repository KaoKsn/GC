#pragma once
#include <stddef.h>

typedef struct Stack
{
  size_t top;
  size_t capacity;
  void **data;
} stack_t;

stack_t *get_stack(size_t capacity);
void push(stack_t *stack, void *obj);
void *pop(stack_t *stack);
void free_stack(stack_t *stack);
