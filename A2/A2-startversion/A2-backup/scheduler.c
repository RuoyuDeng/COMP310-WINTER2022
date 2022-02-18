#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "shellmemory.h"
#include "scheduler.h"
int global_pid = 2000;

// FOR ALL STRATEGIES: add to tail
void append_pcb(pcb_node *head,int spot_index,int line_index,int total_lines){
    pcb_node *cur_node = head;
    // head does not exist, create a new head
    // BUG!! CREATION OF NEW HEAD does not work somehow
    // if(head == NULL){
    //     head = malloc(sizeof(pcb_node));
    //     head->pid = global_pid;
    //     head->spot_index = spot_index;
    //     head->line_index = line_index;
    //     head->total_lines = total_lines;
    //     head->next = NULL;
    //     return;
    // }
    if(cur_node->spot_index == -3){
        head->pid = global_pid;
        global_pid ++;
        head->spot_index = spot_index;
        head->line_index = line_index;
        head->total_lines = total_lines;
        head->next = NULL;
        return;
    }
    
    // head exists, find the end of the linked list
    // spot_index != -3, it is a valid head
    while(cur_node->next != NULL){
        cur_node = cur_node->next;
    }
    
    // define the new pcb node & set up fileds
    cur_node->next = malloc(sizeof(pcb_node));
    cur_node->next->pid = global_pid;
    global_pid++;
    cur_node->next->spot_index = spot_index;
    cur_node->next->line_index = line_index;
    cur_node->next->total_lines = total_lines;
    cur_node->next->next = NULL;
    return;
}

// FCFS ONLY (remove head)
void pophead_pcb(pcb_node **ptr_head){
    pcb_node *tmp_head = *ptr_head;
    *ptr_head = tmp_head->next;
}



int loadfile(char *filename, pcb_node *ready_head){
    char line[1000];
    char cpname[20];
    char *line_piece;
    char *ret;
    int lineindex = 0;
    int start_line_index = -2;
    char file_var_buffer[30];
    FILE *file = fopen(filename,"rt");  // the program is in a file

    // file does not exist
    if(file == NULL){
		printf("%s\n", "Bad command: File not found");
        return 3;
	}
    // get all lines of code
    fgets(line,999,file);

    // load all lines of code into memory space (set_file)
    while(1){
        if(feof(file)) break;
        // make a copy of existing filename
        strcpy(cpname,filename);
        // make sure the variable name is correct
        memset(file_var_buffer,0,sizeof(file_var_buffer));
        // add the line of index to the end of file name and set it into shell memory
        snprintf(file_var_buffer,sizeof file_var_buffer,"%d",lineindex);
        strcat(cpname,file_var_buffer);

        // find the first valid index to insert the first line of the whole script
        if(start_line_index == -2)
            start_line_index = mem_set_lines(cpname,line);
        else if (start_line_index == -1){
            printf("No more space to insert variable! \n");
            return -1;
        } 
        else mem_set_lines(cpname,line);

        
        fgets(line,999,file);
        lineindex++;
    }

    fclose(file);
    append_pcb(ready_head,start_line_index,0,lineindex);
    return 0;
}


