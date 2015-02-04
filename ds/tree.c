#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//binary tree

typedef enum
{
    NODE_LEFT=0,
    NODE_RIGHT
} NODE_TYPE;

typedef struct tree_element
{
    struct tree_element* parent;
    struct tree_element* left;
    struct tree_element* right;
    int value;
} tree_element;

tree_element** my_tree = NULL;

void insert_node(int value, tree_element* parent, NODE_TYPE node_type)
{
    tree_element* child = (tree_element*) malloc(sizeof(tree_element));
    memset(child, 0, sizeof(child)); child->value = value;

    switch(node_type)
    {
        case NODE_LEFT:
            printf("appending %d to left of %d\n", value, parent->value);
            parent->left = child;
            break;
        case NODE_RIGHT:
            printf("appending %d to right of %d\n", value, parent->value);
            parent->right = child;
            break;
        default:
            fprintf(stderr, "Incorrect Node type:%d", node_type);
            exit(-1);
    }
    child->parent = parent;
}

void print_tree(tree_element* node)
{
    if(node)
    {
        print_tree(node->left); print_tree(node->right);
        printf("%d\t", node->value);
    }
}

void free_tree(tree_element* node)
{
    if(node)
    {
        free_tree(node->left);
        free_tree(node->right);
        free(node);
    }
}
typedef enum
{
    SEARCH_DFS,
    SEARCH_BFS
}SEARCH_TYPE;

tree_element* find(int value, tree_element* root, SEARCH_TYPE search_type)
{
    if(root != NULL){
        printf("Current value:%d\n", root->value);
        if(root->value == value)
            return root;
    }
    else
    {
        printf("for search value:%d root is:%p\n", value, root);
        return root;
    }

    {
        switch(search_type)
        {
            case SEARCH_DFS:
            {
                if(root->left) find(value, root->left, SEARCH_DFS); if(root->right) find(value, root->right, SEARCH_DFS);
            }
            break;
            case SEARCH_BFS:
            {
                if(root->left)
                    if(root->value == value)
                        return root;
                if(root->right)
                    if(root->value == value)
                        return root;
                find(value, root->left, SEARCH_BFS);
            }
            break;
        }
    }
    return root;
}

int add(int value)
{
    if(!my_tree)
    {

        tree_element* child = (tree_element*) malloc(sizeof(tree_element));
        memset(child, 0, sizeof(child)); child->value = value;
        my_tree = &child;
        printf("added root node: %d\n", value);
    }
    else
    {
        tree_element* root = *my_tree;
        while(1)
        {
            if(value < root->value)
            {
                printf("%d < %d\n", value, root->value);
                if(root->left)
                {
                    root = root->left;
                    continue;
                }
                else //left child is null
                {
                    insert_node(value, root, NODE_LEFT);
                    break;
                }
            }
            else //value > root->value
            {
                printf("%d > %d\n", value, root->value);
                if(root->right)
                {
                    root = root->right;
                    continue;
                }
                else
                {
                    insert_node(value, root, NODE_RIGHT);
                    break;
                }
            }

        }
    }
    return 1;
}

int main()
{
    add(10); add(20);
    add(5);
    add(30); add(15); add(3);
    add(40);
    add(1); add(2);
    find(30, *my_tree, SEARCH_BFS);
    print_tree(*my_tree);
    free_tree(*my_tree);
    return 0;
}

