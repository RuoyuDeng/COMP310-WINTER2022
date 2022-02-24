#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "shellmemory.h"
#include "scheduler.h"
#include "helper.h"
#include "interpreter.h"
int global_pid = 2000;

// FOR ALL STRATEGIES: add to tail
void append_pcb(pcb_node **ptr_head,int spot_index,int line_index,int total_lines, int job_score){
    pcb_node *cur_node = *ptr_head;


    if(cur_node == NULL){
        cur_node = malloc(sizeof(pcb_node));
        cur_node->pid = global_pid;
        cur_node->spot_index = spot_index;
        cur_node->line_index = line_index;
        cur_node->total_lines = total_lines;
        cur_node->job_score = job_score;
        cur_node->next = NULL;
        *ptr_head = cur_node;
        return;
    }
    
    // head exists, find the end of the linked list
    while(cur_node->next != NULL){
        cur_node = cur_node->next;
    }
    
    // define the new pcb node & set up fileds
    cur_node->next = malloc(sizeof(pcb_node));
    cur_node->next->pid = (cur_node->pid) + 1;  // current pid based on previous one
    cur_node->next->spot_index = spot_index;
    cur_node->next->line_index = line_index;
    cur_node->next->total_lines = total_lines;
    cur_node->next->job_score = job_score;
    cur_node->next->next = NULL;
    return;
}

void append_pcb_tohead(pcb_node *head, pcb_node *append_node){
    pcb_node *tmp_node = head;
    while(tmp_node->next != NULL){
        tmp_node = tmp_node->next;
    }
    tmp_node->next = append_node;
}

// FCFS ONLY (remove head)
pcb_node* pophead_pcb(pcb_node **ptr_head){
    pcb_node *tmp_head = *ptr_head;
    *ptr_head = tmp_head->next;
    tmp_head->next = NULL;
    return tmp_head;
}


int loadfile(char *filename, pcb_node **ptr_head){
    char line[1000];
    char cpname[20];
    char *line_piece;
    char *ret;
    int lineindex = 0;
    int memory_overload;
    int start_line_index = -2;
    char file_var_buffer[30];
    FILE *file = fopen(filename,"rt");  // the program is in a file

    // file does not exist
    if(file == NULL){
		// printf("%s\n", "Bad command: File not found");
        return 3;
	}
    // get all lines of code
    memset(line,0,sizeof(line));

    // load all lines of code into memory space (set_file)
    while(fgets(line, sizeof(line), file) != NULL){
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
        else {
            // too many lines of codes
            if(mem_set_lines(cpname,line) == -1) return -1;
        }
        lineindex++;
    }

    fclose(file);
    append_pcb(ptr_head,start_line_index,0,lineindex,lineindex);
    return 0;
}

// Task 2: exec with FCFS
int fcfspoly(char* filenames[], int filenum){
    int errCode = 0;
    pcb_node **ptr_head = malloc(8);
    pcb_node *ready_head;
    pcb_node *work_node = NULL;
    *ptr_head = NULL;

    // set up ready queue and load scripts into memory
    for(int i = 1; i <= filenum; i++){
        errCode = loadfile(filenames[i],ptr_head);
        if(errCode == -1) {
            return outofMemoryError();
        }else if (errCode == 3){
            return badcommandFileDoesNotExist();
        }
    }
    ready_head = *ptr_head;
    while(1){  
        work_node = pophead_pcb(&ready_head);
        mem_run_lines(work_node,work_node->total_lines);
        mem_cleanup(work_node);
        free(work_node);
        if(ready_head == NULL) break;
    }
    
    free(ptr_head);
    // mem_print_dirtymem();
    return 0;

}

// Task 3: SJF
int sjfpoly(char* filenames[], int filenum){
    int errCode = 0;
    pcb_node **ptr_head = malloc(8);
    pcb_node *ready_head;
    pcb_node *work_node = NULL;
    *ptr_head = NULL;
    // set up ready queue and load scripts into memory
    for(int i = 1; i <= filenum; i++){
        errCode = loadfile(filenames[i],ptr_head);
        if(errCode == -1) {
            return outofMemoryError();
        }else if (errCode == 3){
            return badcommandFileDoesNotExist();
        }
    }
    ready_head = *ptr_head;
    sortReadyQueue(ready_head,"SJF");
    while(1){  
        work_node = pophead_pcb(&ready_head);
        mem_run_lines(work_node,work_node->total_lines);
        mem_cleanup(work_node);
        free(work_node);
        if(ready_head == NULL) break;
    }
    
    free(ptr_head);
    // mem_print_dirtymem();
    return 0;

}

int rrpoly(char* filenames[], int filenum){
    int rest_lines = 0;
    int errCode = 0;
    pcb_node **ptr_head = malloc(8);
    pcb_node *ready_head;
    pcb_node *work_node = NULL;
    *ptr_head = NULL;
    // set up ready queue and load scripts into memory
    for(int i = 1; i <= filenum; i++){
        errCode = loadfile(filenames[i],ptr_head);
        if(errCode == -1) {
            return outofMemoryError();
        }else if (errCode == 3){
            return badcommandFileDoesNotExist();
        }
    }
    ready_head = *ptr_head;
    while(1){
        if(ready_head == NULL) break;
        work_node = pophead_pcb(&ready_head);
        rest_lines = work_node->total_lines - work_node->line_index;
        if(rest_lines==1) mem_run_lines(work_node,1);
        else mem_run_lines(work_node,2);
        // current work_node is done, do not add to tail, clean it up
        if(work_node->line_index == work_node->total_lines) {
            mem_cleanup(work_node);
            free(work_node);
            continue;
        }
        append_pcb_tohead(ready_head,work_node);
    }
    
    free(ptr_head);
    // mem_print_dirtymem();
    return 0;
}

int agingpoly(char* filenames[], int filenum){
    int errCode = 0;
    int rest_lines = 0;
    int findSwap;
    pcb_node **ptr_head = malloc(8);
    pcb_node *ready_head = NULL;
    pcb_node *poped_head = NULL;
    pcb_node *aging_head = NULL;
    pcb_node *swap_node;
    *ptr_head = NULL;

    // set up ready queue and load scripts into memory
    for(int i = 1; i <= filenum; i++){
        errCode = loadfile(filenames[i],ptr_head);
        if(errCode == -1) {
            return outofMemoryError();
        }else if (errCode == 3){
            return badcommandFileDoesNotExist();
        }
    }
    ready_head = *ptr_head;

    // sort the program based on job score
    sortReadyQueue(ready_head,"AGING");
    while(1){
        if(ready_head == NULL) break;
        // reset findSwap and swap_node
        findSwap = 0;
        swap_node = NULL;
        // run for 1 instruction for head
        mem_run_lines(ready_head,1); 
        
        // start aging and check if needs to swap the head with the smallest job score
        aging_head = ready_head->next;
        while(aging_head != NULL){
            if(aging_head->job_score > 0){
                aging_head->job_score--;
                // where we do prompt and swap
                if(aging_head->job_score < ready_head->job_score && !findSwap){
                    swap_node = aging_head;
                    findSwap = 1;
                }
            }
            aging_head = aging_head->next;
        }

        // check if ready_head is done, if so, pop it out and clean it up
        if(ready_head->line_index == ready_head->total_lines){
            poped_head = pophead_pcb(&ready_head);
            mem_cleanup(poped_head);
            free(poped_head);
            continue;
        }

        // if find anything to swap, do it
        if(swap_node != NULL){
            append_pcb_tohead(ready_head,pophead_pcb(&ready_head)); 
        }
        
    }
    
    free(ptr_head);
    // mem_print_dirtymem();
}


