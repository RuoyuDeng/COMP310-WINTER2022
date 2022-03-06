#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcb.h"
#include "run.h"

// typedef struct node{
//     int value;
//     struct node *next;
// }node_t;

void createnode(node_t**ptr_head,int value){
    node_t *new = malloc(sizeof(node_t));
    if(*ptr_head == NULL){
        new->next = NULL;
        new->value = value;
        *ptr_head = new;
        return;
    }
}


void append(node_t *head,int value){
    node_t *cur_node = head;
    node_t **ptr_new = NULL;
    // if(cur_node == NULL){
    //     ptr_new = createnode(ptr_new,value);
    //     head = *ptr_new;
    //     return;

    //     // cur_node = malloc(sizeof(node_t));
    //     // cur_node->value = value;
    //     // cur_node->next = NULL;
    //     // head = cur_node;


    //     // head = malloc(sizeof(node_t));
    //     // head->value = value;
    //     // head->next = NULL;
    //     // return;
    // }
    
    while(cur_node->next != NULL){
        cur_node = cur_node->next;
    }
    cur_node->next = malloc(sizeof(node_t));
    cur_node->next->value = value;
    cur_node->next->next = NULL;
    return;
}


void pophead_pcb(node_t **ptr_head){
    node_t *tmp_head = *ptr_head;
    *ptr_head = tmp_head->next;

}

void changeval(node_t *head, int value){
    head->value = value;
}
int main()
{   
    // node_t **ptr_head;
    // printf("%p",ptr_head);
    // *ptr_head = NULL;
    // node_t *head;
    // createnode(ptr_head,30);
    // head = *ptr_head;
    // printf("%d \n", head->value);
    // // printf("%d \n", tmp_head->value);
    int **x;
    int **y = malloc(8);
    int *a;
    int *b = malloc(8);
    
    int *c;
    printf("x has %p \n",x);
    printf("y has %p \n",y);
    
    puts("");
    // printf("a has %p \n",a);
    // printf("b has %p \n",b);

}



    // // test cross header file includes
    // node_t *head = malloc(sizeof(node_t));
    // head->next = NULL;
    // head->value = 30;
    
    // changeval(head,500);
    // // append(head,40); // append works
    // printf("Current head value is %d \n",head->value);

    // pophead_pcb(&head); // pop head works
    // printf("Current head value is %d \n",head->value);

    // printf("Got a value %d \n",getvalue(head));
    // runnode(head);
    // puts("\n");



    // // concate number to string
    // char filename[20];
    // char file[] = "a.txt";
    // // fgets(filename,30,stdin);
    // int a = 10;
    // snprintf(filename, sizeof filename, "%d\n", a);
    // strcat(file,filename);
    // printf("%s",filename);
    // printf("%c",file[5]);



    // char filename[100];
    // char name_buff[30];
    // fgets(filename,100,stdin);
    // FILE *file = fopen(filename,"rt");
    // memset(name_buff,0,sizeof(name_buff));
    // // add the line of index to the end of file name and set it into shell memory
    // snprintf(name_buff,sizeof name_buff,"%d",1);
    // strcat(filename,name_buff);
    // printf("%s",filename);
