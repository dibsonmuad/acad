#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "helper.h"

struct heap
{
   int* data;
   int capacity;
   int data_length;
};

struct heap* heap_create(int capacity)
{
    struct heap* h = (struct heap*) malloc(sizeof (struct heap));

    h->data = (int*) malloc(sizeof(int) * capacity);
    for (int i=0; i<capacity; i++)
        h->data[i] = INT_MIN;

    h->capacity = capacity; h->data_length = 0;
}

void heap_free(struct heap* h)
{
    free(h->data);
    free(h);
}

int heap_up(struct heap* h)
{
    for(int i = h->data_length; i > 1 ; i /= 2 )
    {
        if(h->data[i-1] > h->data[i/2-1])
            swap(h->data, i-1, i/2-1);
        else break;
    }
}

int heap_down(struct heap* h, int start)
{
    int *d = h->data;
    for(int i=start; i < h->data_length; )
    {
        int c1_index = i*2 + 1, c2_index = i*2+2;
        int c1_valid = (c1_index < h->data_length);
        int c2_valid = (c2_index < h->data_length);

        if(!c1_valid) break;

        if(c1_valid && !c2_valid)
        {
            if(d[i] > d[c1_index])
                break;
            else
            {
                swap(d, i, c1_index); break;
            }
        }
        if(c1_valid && c2_valid)
        {
            if((d[i] > d[c1_index]) && (d[i] > d[c2_index]) )
                break;
            if(d[c1_index] > d[c2_index])
            {
                swap(d, i, c1_index);
                i = c1_index;
            }
            else
            {
                swap(d, i, c2_index);
                i = c2_index;
            }
        }
    }
}

int heap_insert(struct heap* h, int val)
{
    if(h->data_length < h->capacity)
    {
        h->data[h->data_length++] = val;
        heap_up(h);
    }
}

int heap_modify(struct heap* h, int index, int val)
{
    if(index < h->data_length)
    {
        h->data[index] = val;
        heap_down(h, index);
    }
}

int heap_remove(struct heap* h, int index)
{
    if(index < h->data_length)
    {
        h->data[index] = h->data[h->data_length-- - 1];
        heap_down(h, index);
    }
}

int heap_max(struct heap* h)
{
    return h->data[0];
}
/*
int main()
{
    struct heap* h = heap_create(10);
    for(int i=0; i< h->capacity; i++)
    {
        heap_insert(h, (i+1)*2);
    }

    heap_modify(h, 0, 1);
    print_array(h->data, h->capacity);
}
*/
