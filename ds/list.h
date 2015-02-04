#ifndef __LIST_H__
#define __LIST_H__

struct list_element {
    struct list_element* next;
    int value;
};

typedef struct list_element list_element;

list_element* create_element(int value);

int add(list_element** pHead, int value);
int del(list_element** pHead, int value);
int print(list_element** pHead);
int reverse(list_element** pHead);
int push(list_element** pHead, int value);
int free_list(list_element** pHead);

#endif
