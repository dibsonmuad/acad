#include "stack.h"

void init_stack(stack* ps) {
  ps->top = 0;
  ps->elements[ps->top] = -1;
}

void push(stack* ps,int element){
  ps->top++;
  if(ps->top >= 100) { //s->size-1){
    //printf("Here is the stack\n");
    // print_stack(ps);
    printf("Reached top of stack..Overflow\n");
    exit(0);
  }
  printf("pushed %d\n",element);
  ps->elements[ps->top] = element;
}

int pop(stack* ps){
  int pop_val;
  if(ps->top == 0){
    printf("Already at top of stack, returning -1...\n");
    return -1;
  }
  pop_val = ps->elements[ps->top];
  ps->top--;
  printf("popped %d\n", pop_val);
  return pop_val;
}

void empty_stack(stack* ps) {
  while(ps->top != 0)
    pop(ps);
}

void print_stack(stack *ps){
  int i;
  for(i=0; i<=ps->top;i++){
    printf("%d ",ps->elements[i]);
  }
  printf("\n");
}


