#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "shellmemory.h"
#include "scheduler.h"
#include "helper.h"
#include "interpreter.h"
int global_pid = 2000;

// FOR ALL STRATEGIES: add to tail
void append_pcb(pcb_node **ptr_head,int total_lines, int page_table[]){
    pcb_node *cur_node = *ptr_head;
    int page_tablesize = sizeof(page_table) / 4, i;

    if(cur_node == NULL){
        cur_node = malloc(sizeof(pcb_node));
        cur_node->pid = global_pid;
        cur_node->total_lines = total_lines;
        cur_node->frame_index = 0;
        cur_node->line_index = 0;

        cur_node->page_table = malloc(page_tablesize * 4);
        for(i = 0; i < page_tablesize; i++){
            cur_node->page_table[i] = page_table[i];
        }

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
    cur_node->next->total_lines = total_lines;
    cur_node->next->frame_index = 0;
    cur_node->next->line_index = 0;
    cur_node->next->page_table = malloc(page_tablesize * 4);
    for(i = 0; i < page_tablesize; i++){
        cur_node->next->page_table[i] = page_table[i];
    }
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
    int total_lines = 0;
    int frame_index = -2;
    int page_table[34];
    int frame_count = 0;
    int linecount = 0;
    char *lines_tostore[3];
    char back_filename[50];
    char cmd[100];
    char cp[] = "cp ";
    char back_path[] = "backing_store/";

    // form the new file name
    memset(back_filename,0,50);
    strcat(back_filename,back_path);
    strcat(back_filename,filename);

    // form the cp command
    memset(cmd,0,100);
    strcat(cmd,cp);
    strcat(cmd,filename);
    strcat(cmd," ");
    strcat(cmd,back_filename);
    system(cmd);

    FILE *file = fopen(back_filename,"rt");  // the program is in a file

    // file does not exist
    if(file == NULL){
		// printf("%s\n", "Bad command: File not found");
        return 3;
	}
    // get all lines of code
    memset(line,0,sizeof(line));
    memset(lines_tostore,0,sizeof(lines_tostore));
    memset(page_table,0,34);
    // load all lines of code into memory space (set_file)
    while(fgets(line, sizeof(line), file) != NULL){
        lines_tostore[linecount] = strdup(line);
        linecount++;
        total_lines++;

        // only call mem_set_frame when we have linecount == 3
        if(linecount == 3){
            // find the first valid index to insert the first line of the whole script
            
            frame_index = mem_set_frame(lines_tostore);
            if (frame_index == -1){
                printf("No more space to insert frame! \n");
                return -1;
            } 
            page_table[frame_count] = frame_index;
            frame_count ++;
            // reset to store next set of 3 lines
            memset(lines_tostore,0,sizeof(lines_tostore));
            linecount = 0;
        }
        
    }
    // when has less than 3 lines of code left:
    if(linecount == 1 || linecount == 2){
        frame_index = mem_set_frame(lines_tostore);
        if (frame_index == -1){
            printf("No more space to insert frame! \n");
            return -1;
        } 
        page_table[frame_count] = frame_index;
    }

    
    fclose(file);
    append_pcb(ptr_head,total_lines,page_table);
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
        
        append_pcb_tohead(ready_head,work_node);
    }


    // rest_lines = work_node->total_lines - work_node->line_index;
    //     if(rest_lines==1) mem_run_lines(work_node,1);
    //     else mem_run_lines(work_node,2);
    //     // current work_node is done, do not add to tail, clean it up
    //     if(work_node->line_index == work_node->total_lines) {
    //         // mem_cleanup(work_node);
    //         free(work_node);
    //         continue;
    //     }

    //     // work is not done, but work_node is the only node left, so make it the new head
    //     if(ready_head == NULL && work_node != NULL){
    //         ready_head = work_node;
    //         continue;
    //     }
    
    free(ptr_head);
    // mem_print_dirtymem();
    return 0;
}
// // Task 2: exec with FCFS
// int fcfspoly(char* filenames[], int filenum){
//     int errCode = 0;
//     pcb_node **ptr_head = malloc(8);
//     pcb_node *ready_head;
//     pcb_node *work_node = NULL;
//     *ptr_head = NULL;

//     // set up ready queue and load scripts into memory
//     for(int i = 1; i <= filenum; i++){
//         errCode = loadfile(filenames[i],ptr_head);
//         if(errCode == -1) {
//             return outofMemoryError();
//         }else if (errCode == 3){
//             return badcommandFileDoesNotExist();
//         }
//     }
//     ready_head = *ptr_head;
//     while(1){  
//         work_node = pophead_pcb(&ready_head);
//         mem_run_lines(work_node,work_node->total_lines);
//         mem_cleanup(work_node);
//         free(work_node);
//         if(ready_head == NULL) break;
//     }
    
//     free(ptr_head);
//     //mem_print_dirtymem();
//     return 0;

// }

// // Task 3: SJF
// int sjfpoly(char* filenames[], int filenum){
//     int errCode = 0;
//     pcb_node **ptr_head = malloc(8);
//     pcb_node *ready_head;
//     pcb_node *work_node = NULL;
//     *ptr_head = NULL;
//     // set up ready queue and load scripts into memory
//     for(int i = 1; i <= filenum; i++){
//         errCode = loadfile(filenames[i],ptr_head);
//         if(errCode == -1) {
//             return outofMemoryError();
//         }else if (errCode == 3){
//             return badcommandFileDoesNotExist();
//         }
//     }
//     ready_head = *ptr_head;
//     sortReadyQueue(ready_head,"SJF");
//     while(1){  
//         work_node = pophead_pcb(&ready_head);
//         mem_run_lines(work_node,work_node->total_lines);
//         mem_cleanup(work_node);
//         free(work_node);
//         if(ready_head == NULL) break;
//     }
    
//     free(ptr_head);
//     //mem_print_dirtymem();
//     return 0;

// }



// int agingpoly(char* filenames[], int filenum){
//     int errCode = 0;
//     int rest_lines = 0;
//     int findSwap;
//     pcb_node **ptr_head = malloc(8);
//     pcb_node *ready_head = NULL;
//     pcb_node *poped_head = NULL;
//     pcb_node *aging_head = NULL;
//     pcb_node *swap_node;
//     *ptr_head = NULL;

//     // set up ready queue and load scripts into memory
//     for(int i = 1; i <= filenum; i++){
//         errCode = loadfile(filenames[i],ptr_head);
//         if(errCode == -1) {
//             return outofMemoryError();
//         }else if (errCode == 3){
//             return badcommandFileDoesNotExist();
//         }
//     }
//     ready_head = *ptr_head;

//     // sort the program based on job score
//     sortReadyQueue(ready_head,"AGING");
//     while(1){
//         if(ready_head == NULL) break;
//         // reset findSwap and swap_node
//         findSwap = 0;
//         swap_node = NULL;
//         // run for 1 instruction for head
//         mem_run_lines(ready_head,1); 
        
//         // start aging and check if needs to swap the head with the smallest job score
//         aging_head = ready_head->next;
//         while(aging_head != NULL){
//             if(aging_head->job_score > 0){
//                 aging_head->job_score--;
//                 // where we do prompt and swap
//                 if(aging_head->job_score < ready_head->job_score && !findSwap){
//                     swap_node = aging_head;
//                     findSwap = 1;
//                 }
//             }
//             aging_head = aging_head->next;
//         }

//         // check if ready_head is done, if so, pop it out and clean it up
//         if(ready_head->line_index == ready_head->total_lines){
//             poped_head = pophead_pcb(&ready_head);
//             mem_cleanup(poped_head);
//             free(poped_head);
//             continue;
//         }

//         // if find anything to swap, do it
//         if(swap_node != NULL){
//             if(swap_node == ready_head->next){
//                 append_pcb_tohead(ready_head,pophead_pcb(&ready_head));
//                 continue;
//             }
//             swappcb(swap_node,ready_head);
            
//         }
        
//     }
    
//     free(ptr_head);
//     //mem_print_dirtymem();
// }


