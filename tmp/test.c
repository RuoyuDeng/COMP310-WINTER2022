#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcb.h"
#include "run.h"

// typedef struct node{
//     int value;
//     struct node *next;
// }node_t;

void append(node_t *head,int value){
    node_t *cur_node = head;
    while(cur_node->next != NULL){
        cur_node = cur_node->next;
    }
    cur_node->next = malloc(sizeof(node_t));
    cur_node->next->value = value;
}


void pophead_pcb(node_t **ptr_head){
    node_t *tmp_head = *ptr_head;
    *ptr_head = tmp_head->next;

}
int main()
{
    // // test cross header file includes
    // node_t *head = NULL;
    // head = malloc(sizeof(node_t));
    // head->value = 30;

    // append(head,40); // append works
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



    char filename[100];
    char name_buff[30];
    fgets(filename,100,stdin);
    FILE *file = fopen(filename,"rt");
    memset(name_buff,0,sizeof(name_buff));
    // add the line of index to the end of file name and set it into shell memory
    snprintf(name_buff,sizeof name_buff,"%d",1);
    strcat(filename,name_buff);
    printf("%s",filename);
}