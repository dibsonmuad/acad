#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>

#include "list.h"

#define SUCCESS 1
#define FAIL 0


list_element* create_element(int value)
{
   list_element* n = (list_element*) malloc(sizeof(list_element));
   memset(n, 0, sizeof(list_element));
    n->value = value;
   return n;
}

int add(list_element** pHead, int value)
{
    list_element* n = create_element(value);
    if(!*pHead)
    {
       *pHead = n;
    }
    else
    {
        list_element* current = *pHead;
        while(current->next)
        {
            current = current->next;
        }
        current->next = n;
    }
    return SUCCESS;
}

int del(list_element** pHead, int value)
{
    list_element* current = *pHead;
    list_element* prev = current;
    while(current)
    {
        if(current->value == value)
        {
            if(current == *pHead)
                *pHead = current->next;

            prev->next = current->next;
            free(current);
            break;
        }
            prev = current;
            current = current->next;
    }
    return SUCCESS;
}

int free_list(list_element** pHead)
{
    list_element* current = *pHead, *next=NULL;

    while(current)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

int print(list_element** pHead)
{
    list_element* current = *pHead;
    while(current)
    {
        printf("%d ", current->value);
        current = current->next;
    }
    printf("\n");
    return SUCCESS;
}

int reverse(list_element** pHead)
{
    list_element *prev = NULL, *current = *pHead, *next = NULL;

    while(current)
    {
        next = current->next;
        current->next = prev;
        if(!next)
        {
            *pHead = current;
            break;
        }
        prev = current;
        current = next;
    }
}

int push(list_element** pHead, int value)
{
    list_element* n = create_element(value);
    n->next = *pHead;
    *pHead = n;
}

/*int main()
{
    list_element* head = create_element(10);
    list_element** myList = &head;
    add(myList, 10);
    push(myList, 20);
    add(myList, 30);
    print(*myList);
    reverse(myList);
    print(*myList);
    reverse1(myList);
    del(myList, 10);
    add1(myList, 35);
    print(*myList);

}*/
