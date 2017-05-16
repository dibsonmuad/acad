#ifndef __LIST_HPP__
#define __LIST_HPP__

#include <iostream>
#define SUCCESS 0

template <typename T> struct List
{
private:
    struct list_element
    {
        struct list_element* next;
        T value;
    };

public:
    List()
        : mpHead(nullptr)
    {
    }

    ~List() { free_list(); }

    int add(int value)
    {
        list_element* n = create_element(value);
        if (!mpHead)
        {
            mpHead = n;
        }
        else
        {
            n->next = mpHead;
            mpHead  = n;
        }
        return SUCCESS;
    }

    int del(int value)
    {
        list_element* cur  = mpHead;
        list_element* prev = cur;
        while (cur)
        {
            if (cur->value == value)
            {
                if (cur == mpHead)
                    mpHead = cur->next;

                prev->next = cur->next;
                free(cur);
                break;
            }
            prev = cur;
            cur  = cur->next;
        }
        return SUCCESS;
    }

    int print()
    {
        list_element* cur = mpHead;
        while (cur)
        {
            std::cout << cur->value << " ";
            cur = cur->next;
        }
        std::cout << std::endl;
        return SUCCESS;
    }

    int reverse()
    {
        list_element *prev = NULL, *cur = mpHead, *next = NULL;

        while (cur)
        {
            next      = cur->next;
            cur->next = prev;
            if (!next)
            {
                mpHead = cur;
                break;
            }
            prev = cur;
            cur  = next;
        }
        return SUCCESS;
    }

    int free_list()
    {
        list_element *cur = mpHead, *next = NULL;

        while (cur)
        {
            next = cur->next;
            delete cur;
            cur = next;
        }
        return SUCCESS;
    }

private:
    // methods
    list_element* create_element(int value)
    {
        list_element* n = new list_element;
        n->value        = value;
        n->next         = nullptr;
        return n;
    }

    // Data
    list_element* mpHead;
};

#endif
