#include <stdio.h>
#include <string.h>

#include "utils.h"

//for easibility
#define PUSH(stack, val) stack.push(&stack, &val, sizeof(val))
#define POP(stack, val)  stack.pop(&stack, &val, sizeof(val))

typedef struct stack_s
{
  //the stack's internal data
  u8int universal_stack[100];

  //the offset in the internal data where items can be pushed back to
  u32int stack_offset;
  void (*push)(struct stack_s*, void*, size_t size);
  void (*pop)(struct stack_s*, void*, size_t size);
} stack_t;

void pop(stack_t *self, void *elem, size_t size)
{
  //the offset must be decreased
  self->stack_offset -= size;
  //copy over what is in the data to the elem
  memcpy(elem, self->universal_stack + self->stack_offset, size);
}

void push(stack_t *self, void *elem, size_t size)
{
  //copy what is in the elem into the data
  memcpy(self->universal_stack + self->stack_offset, elem, size);
  //increase the offset accordingly
  self->stack_offset += size;
}

void init_stack(stack_t *stack)
{
  //initial values
  stack->stack_offset = 0;
  stack->push = push;
  stack->pop = pop;

  return;
}

s32int main()
{
  stack_t stack;
  init_stack(&stack);

  u32int test = 100, out = 20;

  PUSH(stack, test);
  POP(stack, out);

  printf("Out is %d\n", out);
  return 0;
}