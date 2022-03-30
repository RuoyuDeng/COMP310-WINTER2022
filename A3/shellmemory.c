#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "scheduler.h"
#include "shell.h"

#define MAX_TOKEN_SIZE 6
#define MAX_LINE_SIZE 100
#define FRAME_SIZE 3
#define MAX_FRAMESIZE FRAMESIZE/3
#define MAX_VARMEMSIZE VARMEMSIZE
int frame_full = 0;

typedef struct memory_struct{
    char *var;
    char *value[MAX_TOKEN_SIZE]; // ls[0] = "word", ls[1] = "word2"
} memvar_t;

typedef struct memory_frame{
    char *lines[FRAME_SIZE];
} frame_t;

memvar_t var_store[MAX_VARMEMSIZE];
frame_t *frame_store[MAX_FRAMESIZE];
int LRU_arr[MAX_FRAMESIZE];


// Helper functions
// move most recently used frame_index to front
void init_LRU_arr(){
    for(int i = 0; i < MAX_FRAMESIZE; i++){
        LRU_arr[i] = i;
    }
    return;
}


void clean_frame(){
    for(int i = 0; i < MAX_FRAMESIZE; i++){
        if(frame_store[i] == NULL) continue;
        for(int j = 0; j < 3; j++){
            if(frame_store[i]->lines[j] != NULL)
                memset(frame_store[i]->lines[j],0,100);
        }
        // free(frame_store[i]);
        frame_store[i] = NULL;
    }
    init_LRU_arr();
    return;
}


void move_LRU(int *LRU_arr, int LRU_index){
    int i,j, tmp = 0;
    i = 0;
    while(LRU_arr[i] != LRU_index) i++;
    tmp = LRU_arr[i];
    for(j = i; j >= 0 ;j--){
        if(frame_store[j] != NULL){
            LRU_arr[j] = LRU_arr[j-1];
        }
    }
    LRU_arr[0] = tmp;
    return;
}

int match(char *model, char *var) {
    int i, len=strlen(var), matchCount=0;
    for(i=0;i<len;i++)
        if (*(model+i) == *(var+i)) matchCount++;
    if (matchCount == len)
        return 1;
    else
        return 0;
}

char *extract(char *model) {
    char token='=';    // look for this to find value
    char value[1000];  // stores the extract value
    int i,j, len=strlen(model);
    for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
    // extract the value
    for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
    value[j]='\0';
    return strdup(value);
}


// Shell memory functions

// reset var_store
int var_store_init(){
    int i,j;
    for (i = 0; i < MAX_VARMEMSIZE; i++){
        var_store[i].var = "none";
        for(j = 0; j < MAX_TOKEN_SIZE; j++){
            var_store[i].value[j] = NULL;
        }
    }
    return 0;
}


// Set key value pair
// return the spot index where we set the variable (>=0)
// return -1 if unable to set the variable (memory full)
// reserve the first 100 spots for variables (index 0 -> 99)
int mem_set_value(char *var, char **value) {
    
    int i, j;
    for (i=0; i<MAX_VARMEMSIZE; i++){
        if (strcmp(var_store[i].var, var) == 0){
            for (j = 0; value[j] != NULL; j++) {
                var_store[i].value[j] = strdup(value[j]);
            }
            var_store[i].value[j] = NULL;
            return i;
        }
    }

    //Value does not exist, need to find a free spot.
    for (i=0; i<MAX_VARMEMSIZE; i++){
        if (strcmp(var_store[i].var, "none") == 0){
            var_store[i].var = strdup(var);
            for (j = 0; value[j] != NULL; j++) {
                var_store[i].value[j] = strdup(value[j]);
            }
            return i;
        } 
    }

    return -1;

}

// return the first free spot to insert the frame
// return -1 if the frame store is full
int mem_set_frame(char *store_lines[]) {
    int i, j, insert_index = -1, is_full = 1;
    for (i = 0; i < MAX_FRAMESIZE; i++){
        if(frame_store[i] == NULL){
            if(insert_index == -1) insert_index = i;
            frame_store[insert_index] = malloc(sizeof(frame_t));
            for(j = 0; j < FRAME_SIZE; j++){
                frame_store[insert_index]->lines[j] = malloc(100); // each line has maximum 100 chars
                memset(frame_store[insert_index]->lines[j],0,100);
                frame_store[insert_index]->lines[j] = store_lines[j];
            }
            move_LRU(LRU_arr,insert_index); // change most recently used page
        }
    }
    return insert_index; // all full
}

//get value based on input key
char **mem_get_value(char *var_in) {
    int i;

    for (i=0; i<MAX_VARMEMSIZE; i++){
        if (strcmp(var_store[i].var, var_in) == 0){
            return var_store[i].value;
        } 
    }
    return NULL;
    /* return "Variable does not exist"; */
}

// run all the lines from script stored in shell memory space

// if page fault happens, call 
// page_fault_handler() -> do its job
void mem_run_lines(pcb_node *head, int num_lines){
    int errorCode = 0, free_spots[MAX_FRAMESIZE],free_spot_index=-1;
    int taken_spots[MAX_FRAMESIZE], num_taken_spots = 0;
    char line[100];
    char *line_piece;
    char *ret, all_lines[2];
    int *page_table = head->page_table;
    int line_index, new_line_index, frame_index, i,j,run_index, last_frame_index,next_valid_page;
    int end_flag, total_lines = head->total_lines, evict_index = -1, insert_index;
    frame_t *cur_frame = NULL;
    frame_t *next_frame = NULL;
    int line_len;
    FILE *file = NULL;

    // traverse backwards to find the last_frame_index that is in memory
    for(int k = 33; k > -1; k--){
        if(page_table[k] != -1){
            last_frame_index = page_table[k];
            break;
        }
    }

    for(run_index = 0; run_index < num_lines; run_index++){
        end_flag = 0;
        frame_index = head->frame_index;
        line_index = head->line_index;
        if(frame_index == -1) frame_index = LRU_arr[0];
        if(head->active_pagetable[frame_index] == -2){
            head->is_lastframe = 1;
        }
        // if we are at the last frame that in MEMORY, then we are having page fault
        if(head->is_lastframe){
            // frame is full, evict one page
            //if(evict_index != -1){
            file = fopen(head->filename,"rt");
            if(frame_full){
                // print the evicited page if frame is full
                evict_index = LRU_arr[MAX_FRAMESIZE-1];
                printf("Page fault! Victim page contents: \n");
                for(i = 0; i < 3; i++){
                    line_len = strlen(frame_store[evict_index]->lines[i]);
                    if((frame_store[evict_index]->lines[i])[line_len-1] == '\n')
                        printf("%s",frame_store[evict_index]->lines[i]);
                    else printf("%s\n",frame_store[evict_index]->lines[i]);
                }
                printf("End of victim page contents.\n");

                free(frame_store[evict_index]);
                frame_store[evict_index] = NULL;
                // move_LRU(LRU_arr,evict_index);
                // insert_index = LRU_arr[0];
                insert_index = evict_index;
            }

            else{
                // frame is not full, read the lines from file (continously) to form 1 page
                // ex) line_index = 4, then skip 5 lines
                
                for(i = 0; i < MAX_FRAMESIZE; i++){
                    if(frame_store[i] == NULL){
                        insert_index = i;
                        break;
                    }
                }
            }

            // write to the target insert frame  && frame_store[insert_index] is NULL (precondition)
            frame_store[insert_index] = malloc(sizeof(frame_t));
            move_LRU(LRU_arr,insert_index); // LRU change in page fault

            i = 0;
            while(i < line_index){
                fgets(line, sizeof(line), file);
                i++;
            }

            i = 0;
            while(i<3){
                frame_store[insert_index]->lines[i] = malloc(100);
                memset(frame_store[insert_index]->lines[i],0,100);
                i++;
            }

            memset(line,0,sizeof(line));
            i = 0;
            while(fgets(line, sizeof(line), file) != NULL && i < 3){
                strcpy(frame_store[insert_index]->lines[i],line);
                i++;
            }
            
            i = 0;
            while(page_table[i] != -1) {
                if (page_table[i] == insert_index) break;
                i++;
            }
            // find if cur insert_index in page_table or not, if not add to end
            head->page_table[i] = insert_index;

            for(j = 0; head->page_table[j] != insert_index; j++);
            head->frame_index = j; // relocate which frame of pcb we insert the page
            head->active_pagetable[j] = head->page_table[j]; // reactivate the page
            fclose(file);

            head->is_lastframe = 0; // no matter what stragtegy used to handle page fault, reset back to normal
            j = 1; // j = 1 -> full frame, j = 0 -> not full
            for(i = 0; i < MAX_FRAMESIZE; i++){
                if(frame_store[i] == NULL){
                    j = 0;
                    break;
                }
            }
            if(j) frame_full = 1;
            else frame_full = 0;
            
            break;
        }

        // else, NO page fault
        cur_frame = frame_store[page_table[frame_index]];
        move_LRU(LRU_arr,page_table[frame_index]); // change NOT in page fault

        // Done cond 1: last line of script
        if(line_index == (head->total_lines-1)) end_flag = 1;

        // Done cond 2: read a NULL in page in memory
        // check which line we at -> line_index = 5 % 3 = 2, 6 % 3 = 0, 7 % 3 = 1, 8 % 3 = 2 
        // NOTE: line_index starts at 0
        // line_index can be any index of the line, %3 to get the relative line index for each frame
        new_line_index = line_index % 3;
        if(cur_frame->lines[new_line_index] == NULL){
            end_flag = 1;
            break;
        }

        // run a command
        memset(line,0,sizeof(line));
        strcpy(line,cur_frame->lines[new_line_index]);
        // check if the line has ; or not
        ret = strchr(line,';');
        if(ret != NULL){
            line_piece = strtok(line,";");
            while(line_piece != NULL){
                errorCode = parseInput(line_piece);	// which calls interpreter()
                if (errorCode == -1) exit(99);
                line_piece = strtok(NULL,";");
            }
        }
        else parseInput(line); // if it does not, then execute the line normally
        
            
        memset(line,0,sizeof(line));


        j = 1; // j = 1 -> full frame, j = 0 -> not full
        for(i = 0; i < MAX_FRAMESIZE; i++){
            if(frame_store[i] == NULL){
                j = 0;
                break;
            }
        }
        if(j) frame_full = 1;
        else frame_full = 0;
        head->line_index += 1;
        // move to next page in page table by increment the frame_index by 1
        if(new_line_index == 2) {

            // [-2,3,-1,-1,-1]
            // [-2,-2,-1,-1,-1]
            // [-2,-2,-1,-1,-1] [2 3]
            // frame_index = 0
            // head->page_table[0] = 3


            head->active_pagetable[frame_index] = -2; // the worked frame set to inactivate
            // while(head->page_table[i] != head->page_table[frame_index]) i++;
            // if(i != MAX_FRAMESIZE-1 && (head->page_table[i+1] != -1)){
            //     head->frame_index = i+1;
            // }

            i = 0; // i = 0 if there is no active page, i = 1 if there is one
            j = 0;
            while(j < 34){
                if(head->active_pagetable[j] >= 0) {
                    i = 1;
                    break;
                }
                j++;
            }
            
            if(i == 1) head->frame_index = j; // update if there is activate page
    
        }
        if(end_flag){
            break;
        }
    }
    if(end_flag) head->is_done = 1;
    
    return;
    
}

