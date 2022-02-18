#ifndef pcb_H
#define pcb_H

typedef struct node{
    int value;
    struct node *next;
}node_t;
int getvalue(node_t* head);
#endif