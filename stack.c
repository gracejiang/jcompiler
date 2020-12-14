/*
*
*   STACK
*
*   This is the stack class. It acts as a doubly-linked list.
*
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ACTUAL STACK OF INTS
typedef struct stack_list {
    struct node * head;
    struct node * tail;
    int size; 
} stack_list;

// stack_node for nodes in stack
typedef struct node {
    int val;
    struct node * next;
    struct node * prev;
} node;

// push value to top of stack
void push_stack(stack_list * slist, int val) {

    node * current = NULL;
    node * new_node = NULL;

    // more than 1 element left
    if (slist->size >= 1) {
        current = slist->tail;

        current->next = (node *) malloc(sizeof(node));
        current->next->val = val;
        current->next->next = NULL;
        current->next->prev = current;

        slist->tail = current->next;
        slist->size = slist->size + 1;
    } else if (slist->size == 0) {
        new_node = (node *) malloc(sizeof(node));
        new_node->val = val;
        new_node->next = NULL;
        new_node->prev = NULL;

        slist->tail = new_node;
        slist->head = new_node;
        slist->size = 1;
    }
}

// drop value from top of stack
void drop_stack(stack_list * slist) {
    if (slist->size > 1) {
        node * current_node = slist->tail;
        slist->tail = current_node->prev;
        free(current_node);
        slist->size = slist->size - 1;
        slist->tail->next = NULL;
    } else if (slist->size == 1) {
        slist->tail = NULL;
        slist->head = NULL;
        slist->size = slist->size - 1;
    }
}

// get value at top of stack
int get_stack(stack_list * slist) {
    return slist->tail->val;
}

// pop value from top of stack
int pop_stack(stack_list * slist) {
    int num = get_stack(slist);
    drop_stack(slist);
    return num;
}


// checks if elem is in stackl
int in_stack(stack_list * slist, int search_val) {
    if (slist->size == 0) {
        return 0;
    }

    node * current_node = slist->head;
    while (current_node != NULL) {
        if (current_node->val == search_val) {
            return 1;
        }
        current_node = current_node->next;
    }
    
    return 0;
}

// if stack still has more values
int has_next(stack_list * slist) {
    return (slist->size != 0);
}

// print stack
void print_stack(stack_list * slist) {
    if (slist->size == 0) {
        printf("empty stack\n");
        return;
    }
    
    node * current_node = slist->head;
    while (current_node != NULL) {
        printf("%d, ", current_node->val);
        current_node = current_node->next;
    }

}

