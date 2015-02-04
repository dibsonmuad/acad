#include "helper.h"
#include <stdio.h>

void swap(int* d, int i1, int i2)
{
    int temp = d[i1];
   d[i1] = d[i2];
   d[i2] = temp;
}

void print_array(int* a, int size)
{
    printf("%d:", size);
    for( int i=0; i<size; i++)
        printf("%i ", a[i]);
    printf("\n");
}



