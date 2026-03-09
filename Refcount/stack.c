#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

// Create a stack of the required capacity.
stack_t *get_stack(size_t capacity)
{
  stack_t *stack = malloc(sizeof(stack_t));
  if (stack == NULL) {
    fprintf(stderr, "Stack initialization failed!\n");
    return NULL;
  }

  stack->data = calloc(capacity, sizeof(void *));
  if (stack->data == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    free(stack);
    return NULL;
  }

  stack->capacity = capacity;
  stack->top = 0;
  return stack;
}

// Push an object into the stack.
void push(stack_t *stack, void *obj)
{
  if (stack == NULL || obj == NULL) {
    fprintf(stderr, "Invalid push operation!\n");
    return;
  } else if (stack->top == stack->capacity) {
    void **tmp = realloc(stack->data, sizeof(void *) * (stack->capacity * 2));
    if (tmp == NULL) {
      fprintf(stderr, "Stack FULL. Failed to resize it!\n");
      return;
    }
    stack->data = tmp;
    stack->capacity *= 2;
  }
  stack->data[stack->top++] = obj;
  return;
}

// Pop top element from the stack.
void *pop(stack_t *stack)
{
  if (stack == NULL || stack->top == 0) {
    fprintf(stderr, "Stack EMPTY!\n");
    return NULL;
  }
  return stack->data[--(stack->top)];
}

// Freeing the stack.
// Prerequisite: Memory allocated for data must be freed before hand.
void free_stack(stack_t *stack)
{
  if (stack == NULL) {
    return;
  }

  free(stack->data);
  free(stack);
  return;
}
