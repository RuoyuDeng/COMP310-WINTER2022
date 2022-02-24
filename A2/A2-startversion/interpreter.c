#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "shellmemory.h"
#include "scheduler.h"
#include "shell.h"

int MAX_ARGS_SIZE = 100;

int help();
int quit();
int badcommand();
int set(char* var, char** value);
int print(char* var);
int run(char* script);
int echo(char* var);
int badcommandFileDoesNotExist();
int outofMemoryError();
int fcfspoly(char* filenames[], int filenum);

int sjfpoly(char* filenames[], int filenum);
void swappcb(pcb_node *largeNode, pcb_node *smallNode);
void sortReadyQueue(pcb_node *head, char* mode);

int rrpoly(char* filenames[], int filenum);
int agingpoly(char* filenames[], int filenum);

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
    int i;

    if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
        return badcommand();
    }


    for ( i=0; i<args_size; i++){ //strip spaces new line etc
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }

    if (strcmp(command_args[0], "help")==0){
        //help
        if (args_size != 1) return badcommand();
        return help();

    } else if (strcmp(command_args[0], "quit")==0) {
        //quit
        if (args_size != 1) return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "set")==0) {
        //set
        if (args_size > 7) {
            printf("Bad command: Too many tokens\n");
            return 1;
        }
        return set(command_args[1], command_args + 2);

    } else if (strcmp(command_args[0], "print")==0) {
        if (args_size != 2) return badcommand();
        return print(command_args[1]);

    } else if (strcmp(command_args[0], "echo") == 0) {
        if (args_size != 2) return badcommand();
        return echo(command_args[1]);
    } else if (strcmp(command_args[0], "run")==0) {
        if (args_size != 2) return badcommand();
        return run(command_args[1]);
    } else if (strcmp(command_args[0], "my_ls")==0) {
        if (args_size != 1) return badcommand();
        return system("ls -1");
    } else if (strcmp(command_args[0], "exec")==0) {

        if (args_size >= 3 && args_size <= 5) {
            // check for identical files
            if(args_size > 3){
                for(int i = 1; i<args_size-1; i++){
                    for(int j = i+1; j<args_size-1; j++){
                        if(strcmp(command_args[i],command_args[j]) == 0){
                            printf("Identical files found, Error\n");
                            return badcommand();
                        }       
                    }
                }
            }
            // exec prog1 prog2 prog3 FCFS (command_args)
            // all polies (1. the address of first file, 2. total number of files to read)
            if(strcmp(command_args[args_size-1],"FCFS") == 0){
                
                return fcfspoly(command_args,args_size-2);
            }
            else if (strcmp(command_args[args_size-1],"SJF") == 0){
                return sjfpoly(command_args,args_size-2);
            }
            else if(strcmp(command_args[args_size-1],"RR") == 0){
                return rrpoly(command_args,args_size-2);
            }
            else if(strcmp(command_args[args_size-1],"AGING") == 0){
                return agingpoly(command_args,args_size-2);
            }
        }
        return badcommand();
    } else return badcommand();
}

int help(){

    char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n \
exec prog1 prog2 prog3 POLICY     Executes up to 3 concurrent programs, according to a given scheduling policy ";
    printf("%s\n", help_string);
    return 0;
}

int quit(){
    printf("%s\n", "Bye!");
    exit(0);
}

int badcommand(){
    printf("%s\n", "Unknown Command");
    return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
    printf("%s\n", "Bad command: File not found");
    return 3;
}

int outofMemoryError(){
    printf("Out of memory, can not set new variable\n");
    return -1;
}

int set(char* var, char** value){
    if(mem_set_value(var, value) == -1){
        return outofMemoryError();
    }
    return 0;
}

int print(char* var){
    char **tokens = mem_get_value(var);
    if (tokens == NULL) {
        printf("Variable does not exist\n");
        return 0;
    } else {
        int i;
        for (i = 0; tokens[i]; ++i) {
            if (i != 0) {
                printf(" ");
            }
            printf("%s", tokens[i]);
        }
        printf("\n");
    }
    return 0;
}

int echo(char* var) {
    if (var[0] == '$') {
        char **tokens = mem_get_value(var+1);
        if (tokens != NULL) {
            int i;
            for (i = 0; tokens[i]; ++i) {
                if (i != 0) {
                    printf(" ");
                }
                printf("%s", tokens[i]);
            }
        }
    } else {
        printf("%s", var);
    }
    printf("\n");
}

// Task 1: modify run
int run(char* filename){
    int errCode = 0;
    pcb_node *ready_head = malloc(sizeof(pcb_node)); // ready queue head
    // if the spot_index is -3, means it is not initiallized yet
    ready_head->spot_index = -3; 

    //loadfile()
    errCode = loadfile(filename,ready_head);
    if(errCode == -1) {
        return outofMemoryError();
    }else if (errCode == 3){
        return badcommandFileDoesNotExist();
    }
    // ALL info stored in ready_head
    // 1. start index of shell memory
    // 2. which line we are working on (incremented in mem_run_lines)
    // 3. max number of iteration 
    mem_run_lines(ready_head,ready_head->total_lines);

    // clean up
    // 1. remove all lines of code from shell memory space
    mem_cleanup(ready_head);
    // 2. remove ready queue (made of pcb node), since there is only one node so we just free it
    free(ready_head);

    // print to check if there is any dirty memory
    // mem_print_dirtymem();
    return errCode;
}


// Task 2: exec with FCFS

int fcfspoly(char* filenames[], int filenum){
    int errCode = 0;
    pcb_node *ready_head = malloc(sizeof(pcb_node));
    pcb_node *work_node = NULL;
    ready_head->spot_index = -3;

    // set up ready queue and load scripts into memory
    for(int i = 1; i <= filenum; i++){
        errCode = loadfile(filenames[i],ready_head);
        if(errCode == -1) {
            return outofMemoryError();
        }else if (errCode == 3){
            return badcommandFileDoesNotExist();
        }
    }
    while(1){  
        work_node = pophead_pcb(&ready_head);
        mem_run_lines(work_node,work_node->total_lines);
        mem_cleanup(work_node);
        free(work_node);
        if(ready_head == NULL) break;
    }
    // mem_print_dirtymem();
    return 0;

}

// swap 2 nodes
void swappcb(pcb_node *largeNode, pcb_node *smallNode){
    int tmp_pid = smallNode->pid;
    int tmp_spotindex = smallNode->spot_index;
    int tmp_lineindex = smallNode->line_index;
    int tmp_totallines = smallNode->total_lines;
    int tmp_jobscore = smallNode->job_score;

    smallNode->pid = largeNode->pid;
    smallNode->spot_index = largeNode->spot_index;
    smallNode->line_index = largeNode->line_index;
    smallNode->total_lines = largeNode->total_lines;
    smallNode->job_score = largeNode->job_score;

    largeNode->pid = tmp_pid;
    largeNode->spot_index = tmp_spotindex;
    largeNode->line_index = tmp_lineindex;
    largeNode->total_lines = tmp_totallines;
    largeNode->job_score = tmp_jobscore;
}

void sortReadyQueue(pcb_node *head, char *mode){
    int done = 1;

    pcb_node *queue_end = NULL;
    pcb_node *cur_node;

    // NULL head check
    if(head == NULL) return;
    
    while(done){
        // assume we can finish sorting this iteration -> no swapping happened means it is sorted (small to large)
        done = 0;
        cur_node = head;
        
        // keep swapping until the max total lines of pcb goes to the end of queue
        while(cur_node->next != queue_end){
            // sort based on total lines
            if(strcmp(mode,"SJF") == 0){
                if(cur_node->total_lines > cur_node->next->total_lines){
                    swappcb(cur_node,cur_node->next);
                    // some swap happened, keep iterating
                    done = 1;
                }
            }
            // sort based on job age
            else if (strcmp(mode,"AGING") == 0){
                if(cur_node->job_score > cur_node->next->job_score){
                    swappcb(cur_node,cur_node->next);
                    // some swap happened, keep iterating
                    done = 1;
                }
            }
            
            cur_node = cur_node->next;
        }
        // inner loop terminates when cur_node->next at the end, treat cur_node as the new end
        queue_end = cur_node;
    }
}

// Task 3: SJF
int sjfpoly(char* filenames[], int filenum){
    int errCode = 0;
    pcb_node *ready_head = malloc(sizeof(pcb_node));
    pcb_node *work_node = NULL;
    ready_head->spot_index = -3;

    // set up ready queue and load scripts into memory
    for(int i = 1; i <= filenum; i++){
        errCode = loadfile(filenames[i],ready_head);
        if(errCode == -1) {
            return outofMemoryError();
        }else if (errCode == 3){
            return badcommandFileDoesNotExist();
        }
    }
    sortReadyQueue(ready_head,"SJF");
    while(1){  
        work_node = pophead_pcb(&ready_head);
        mem_run_lines(work_node,work_node->total_lines);
        mem_cleanup(work_node);
        free(work_node);
        if(ready_head == NULL) break;
    }

    // mem_print_dirtymem();
    return 0;

}

int rrpoly(char* filenames[], int filenum){
    int errCode = 0;
    int rest_lines = 0;
    pcb_node *ready_head = malloc(sizeof(pcb_node));
    pcb_node *work_node = NULL;
    ready_head->spot_index = -3;

    // set up ready queue and load scripts into memory
    for(int i = 1; i <= filenum; i++){
        errCode = loadfile(filenames[i],ready_head);
        if(errCode == -1) {
            return outofMemoryError();
        }else if (errCode == 3){
            return badcommandFileDoesNotExist();
        }
    }
    
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

    // mem_print_dirtymem();
    return 0;
}

int agingpoly(char* filenames[], int filenum){
    int errCode = 0;
    int rest_lines = 0;
    int findSwap;
    pcb_node *ready_head = malloc(sizeof(pcb_node));
    pcb_node *poped_head = NULL;
    pcb_node *aging_head = NULL;
    pcb_node *swap_node;
    ready_head->spot_index = -3;

    // set up ready queue and load scripts into memory
    for(int i = 1; i <= filenum; i++){
        errCode = loadfile(filenames[i],ready_head);
        if(errCode == -1) {
            return outofMemoryError();
        }else if (errCode == 3){
            return badcommandFileDoesNotExist();
        }
    }

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
    // mem_print_dirtymem();
}