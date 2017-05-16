
#ifndef __stack__h__
#define __stack__h__
#include <stdio.h>
#include <stdlib.h>
typedef struct
{
    int top;
    int elements[101];
} stack;

void init_stack(stack* s);
void push(stack* s, int element);
int pop(stack* s);
void empty_stack(stack* s);
void print_stack(stack* s);
#endif /* _Stack_h */
