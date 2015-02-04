#ifndef __HEAP_H__
#define __HEAP_H__

struct heap
{
   int* data;
   int capacity;
   int data_length;
};

struct heap* heap_create(int capacity);
void heap_free(struct heap* h);
int heap_up(struct heap* h);
int heap_down(struct heap* h, int start);
int heap_insert(struct heap* h, int val);
int heap_modify(struct heap* h, int index, int val);
int heap_remove(struct heap* h, int index);
int heap_max(struct heap* h);

#endif
