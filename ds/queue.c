#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct queue
{
    int* first;
    int* last;
    int* data;
    size_t capacity;
    /*  private */
    size_t data_length;
};

struct queue* queue_create(size_t capacity)
{
    struct queue* q = (struct queue*)malloc(sizeof(struct queue));
    q->data         = (int*)malloc(sizeof(int) * capacity);
    q->first = q->last = NULL;
    q->data_length     = 0;
    q->capacity        = capacity;
}

int queue_insert(struct queue* q, int value)
{
    int ret = 1;
    if (q->capacity > q->data_length)
    {
        if (!q->data_length)
        {
            q->last = q->first = q->data;
        }
        *q->last++ = value;
        q->data_length++;
        // make sure you don't go over q length.
        if (q->last == q->data + q->capacity + 1)
            q->last = q->data;
    }
    else
    {
        fprintf(stderr, "Queue Full! Cannot insert %d\n", value);
        for (int i = 0; i < q->data_length; i++)
            printf("%d ", q->data[i]);
        printf("first:%d last:%d\n", *q->first, *q->last);
        ret = -1;
    }
}

/*  remove the top element from the queue */
int queue_remove(struct queue* q, int* value)
{
    int ret = 1;
    if (q->data_length > 0)
    {
        *value = *q->first;
        q->data_length--;
        q->first++;
        // reset if you go over
        if (q->first == q->data + q->capacity + 1)
            q->first = q->data;
    }
    else
    {
        fprintf(stderr, "Empty Queue!\n");
        *value = INT_MIN;
        ret    = -1;
    }
    return ret;
}

int queue_free(struct queue* q)
{
    free(q->data);
    free(q);
}

/*
int main()
{

    struct queue* q = queue_create(10);
    int top = 0;

    for(int i=0; i<10; i++)
        queue_insert(q, i);

    for(int i=0; i<10; i++)
    {
        if(queue_remove(q, &top) != -1) printf("top:%d first:%p last%p\n", top,
q->first, q->last);
    }

    for(int i=0; i<20 ; i++)
    {
        for (int j=0; j<2; j++)
        {
            queue_insert(q, 2*i+j); printf("val:%d first:%d last:%d length:%d
pfirst:%p plast:%p\n", 2*i+j, *q->first, *(q->last-1), q->data_length, q->first,
q->last);
        }
        queue_remove(q, &top); printf("top:%d first:%p last%p\n", top, q->first,
q->last);
    }

    // apparently pointless. All memory will be returned to the OS when process
quits. queue_free(q);
}
*/
