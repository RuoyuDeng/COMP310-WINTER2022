#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
    int data;
    struct node *next;
} node_t;

/*
Interates through the list and prints the values of each node
*/
void print_list(node_t *head)
{
    node_t *current = head;

    while (current != NULL)
    {
        printf("%d\n", current->data);
        current = current->next;
    }
}

/*
Append an item at the end of the Linked List
*/
void append(node_t *head, int val)
{
    node_t *current = head;
    //  Finds the end of the list
    while (current->next != NULL)
    {
        current = current->next;
    }

    // Now we can add a new variable
    current->next = (node_t *)malloc(sizeof(node_t));
    current->next->data = val;
    current->next->next = NULL;
}

/*
Append an item at the beginning of the Linked List
We need to change where the head points to, so we need a pointer to a pointer!!!
*/
void prepend(node_t **head_ptr, int val)
{
    node_t *new_node;
    new_node = malloc(sizeof(node_t));

    new_node->data = val;
    new_node->next = *head_ptr; //  Dereference head_ptr so RHS is just the head
    *head_ptr = new_node;   // Now the head needs to point to the new node
}

/*
Removes the Head node (remember to keep track of head)
*/
int pop(node_t **head_ptr)
{
    int retval = -1;
    node_t *next_node = NULL;

    //  Checks if the Linked List is empty
    if (*head_ptr == NULL)
    {
        return -1;
    }

    next_node = (*head_ptr)->next;
    retval = (*head_ptr)->data; //  Retrives the data in the node being removed
    free(*head_ptr);    //  Remember to free the memory pointed by the initial head
    *head_ptr = next_node;  //  Assign the new node as the head

    return retval;
}

int main()
{
    //  Creating a node named head
    node_t *head = NULL;
    head = malloc(sizeof(node_t));  //   Allocates memory for the node from Heap
    if (head == NULL)   //  Validates memory allocation
    {
        return 1;
    }

    //  Assign values to the "node_t" members
    //  Use "->" instead of "." for pointers (head is a pointer)
    head->data = 1;
    head->next = NULL;
    return 0;
}