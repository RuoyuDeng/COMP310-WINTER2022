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
    // char line[MAX_LINE_SIZE]; // used to store a line for the file, "set x 10"
} memvar_t;

typedef struct memory_frame{
    char *lines[FRAME_SIZE];
} frame_t;

memvar_t var_store[MAX_VARMEMSIZE];
frame_t *frame_store[MAX_FRAMESIZE];
int LRU_arr[MAX_FRAMESIZE];


// Helper functions
// move most recently used frame_index to front

void clean_frame(){
    for(int i = 0; i < MAX_FRAMESIZE; i++){
        free(frame_store[i]);
        frame_store[i] = NULL;
    }
}

void init_LRU_arr(){
    for(int i = 0; i < MAX_FRAMESIZE; i++){
        LRU_arr[i] = i;
    }
}


void move_LRU(int *LRU_arr, int LRU_index){
    int i,j, tmp = 0;
    i = 0;
    while(LRU_arr[i] != LRU_index) i++;
    tmp = LRU_arr[i];
    for(j = i; j >= 0 ;j--){
        LRU_arr[j] = LRU_arr[j-1];
    }
    LRU_arr[0] = tmp;
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

// // reset frame store
// void frame_store_init(){
//     int i;
//     for (i = 0; i < MAX_FRAMESIZE; i++){
//         frame_store[i] = NULL;
//     }
// }


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

// 0 1 2 3
// e e e e 
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
    int line_index, new_line_index, frame_index, i,j, last_frame_index,next_valid_page;
    int end_flag, total_lines = head->total_lines, evict_index = -1, insert_index;
    frame_t *cur_frame = NULL;
    frame_t *next_frame = NULL;
    FILE *file = NULL;

    // traverse backwards to find the last_frame_index that is in memory
    for(int k = 33; k > -1; k--){
        if(page_table[k] != -1){
            last_frame_index = page_table[k];
            break;
        }
    }
    // // initializa free spot array & taken_spots array
    // for(int k = 0; k < MAX_FRAMESIZE; k++) {
    //     free_spots[k] = -1;
    //     taken_spots[k] = -1;
    // }
    
    // // knows where we can insert a frame ex) free_spots = [1,3,5]
    // // -> frame 1 frame 3 frame 5 are free
    // i = 0;
    // j = 0;
    // for(int k = 0; k < MAX_FRAMESIZE; k++){
    //     if(frame_store[k] == NULL) {
    //         free_spots[i] = k;
    //         i++;
    //     } else {
    //         taken_spots[j] = k;
    //         num_taken_spots += 1;
    //         j++;
    //     }
    // }
    
    // if(free_spots[0] != -1) {
    //     free_spot_index = free_spots[0];   
    // }
    // // if the first index in free_spots array is -1, then we have no free spots, so need to have evict_index 
    // else evict_index = rand() % num_taken_spots;


    // run num_lines of lines
    // NOW line index references to which index of line we are working on in whole file

    for(j = 0; j < num_lines; j++){
        end_flag = 0;
        frame_index = head->frame_index;
        line_index = head->line_index;
        if(frame_index == -1) frame_index = LRU_arr[0];
        // if we are at the last frame that in MEMORY, then we are having page fault
        if(head->is_lastframe || frame_full){
            // frame is full, evict one page
            //if(evict_index != -1){
            file = fopen(head->filename,"rt");
            if(frame_full){
                // print the evicited page if frame is full
                evict_index = LRU_arr[MAX_FRAMESIZE-1];
                printf("Page fault! Victim page contents: \n");
                printf("<the contents of the page, line by line> \n");
                for(i = 0; i < 3; i++){
                    printf("%s",frame_store[evict_index]->lines[i]);
                }
                printf("End of victim page contents.\n");

                free(frame_store[evict_index]);
                frame_store[evict_index] = NULL;
                move_LRU(LRU_arr,evict_index);
                insert_index = LRU_arr[0];
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
            move_LRU(LRU_arr,insert_index);

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
            fclose(file);
            head->is_lastframe = 0; // no matter what stragtegy used to handle page fault, reset back to normal
            frame_full = 0;
            break;
        }

        // else, NO page fault
        cur_frame = frame_store[page_table[frame_index]];
        move_LRU(LRU_arr,frame_index);

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

        // // otherwise....
        // // new_line_index = 0 or 1
        // if(new_line_index != 2) {
        //     has_nextpage = 0;
        // }

        // // new_line_index = 2 -> we will be at next page after reading this line
        // else {
        //     // case 1: there is next page (might be page fault, but later) -> update frame_index
        //     // case 2: there is no next page (only read 1 line) -> do not update frame_index
        //     // if(frame_index != last_frame_index){
        //     //     //!!!!!!!!!!!!!!!!!!! Later page fault!!!!!!!!!!!!!!!!!!
                
        //     // }
        //     has_nextpage = 1;
        // }

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
        
        // at last line of last frame && there might be more lines coming up, which can cause page fault
        if(frame_index == last_frame_index && new_line_index == 2){
            if(frame_index == MAX_FRAMESIZE - 1) frame_full = 1;
            head->is_lastframe = 1;
        }
        memset(line,0,sizeof(line));

        head->line_index += 1;
        // move to next page in page table by increment the frame_index by 1
        if(new_line_index == 2) {
            head->frame_index = head->page_table[frame_index+1];
        }
        if(end_flag){
            break;
        }
    }
    if(end_flag) head->is_done = 1;
    
    return;

        
        

        // // line_index = 0 or 1
        // if((line_index + 1) % 3 != 0) {
        //     for(i = line_index; i <= line_index+1; i++){
        //         // clean up the line buffer to store the command
        //         memset(line,0,sizeof(line));
        //         if(cur_frame->lines[i] == NULL){
        //             end_flag = 1;
        //             break;
        //         }
        //         strcpy(line,cur_frame->lines[i]);
        //         // check if the line has ; or not
        //         ret = strchr(line,';');
        //         if(ret != NULL){
        //             line_piece = strtok(line,";");
        //             while(line_piece != NULL){
        //                 errorCode = parseInput(line_piece);	// which calls interpreter()
        //                 if (errorCode == -1) exit(99);
        //                 line_piece = strtok(NULL,";");
        //             }
        //         }
        //         // if it does not, then execute the line normally
        //         parseInput(line);
        //         memset(line,0,sizeof(line));
        //     }
        //     new_line_index = i % 3;
        //     if(new_line_index == 0){
        //         head->frame_index += 1;
        //     }
        //     head->line_index = new_line_index;
        // }

        // // line_index = 2
        // // case 1: there is next page (might be page fault, but later)
        // // case 2: there is no next page (only read 1 line)
        // // check if frame_index == last_frame_index
        // else{
        //     if(frame_index != last_frame_index){
        //         //!!!!!!!!!!!!!!!!!!! Later page fault!!!!!!!!!!!!!!!!!!
        //         next_frame = frame_store[page_table[frame_index+1]]; 
        //         //!!!!!!!!!!!!!!!!!!! Later page fault!!!!!!!!!!!!!!!!!!
        //         has_nextpage = 1;
        //     }

        //     // we assume next pages exists for now, and check for NULL lines
        //     if(!has_nextpage){
        //         next_valid_page = (cur_frame->lines[2] == NULL); 
        //     }
            
        //     // the last line of current page is NULL, break the loop
        //     if(next_valid_page) {
        //         head->is_done = 1;
        //         break;
        //     }
            
        //     // if the last line of current page is not NULL and has no next page, we done
        //     if(!next_valid_page && !has_nextpage) end_flag = 1;
        //     // run the last line in this page
        //     // clean up the line buffer to store the command
        //     memset(line,0,sizeof(line));
        //     strcpy(line,cur_frame->lines[2]);
        //     // check if the line has ; or not
        //     ret = strchr(line,';');
        //     if(ret != NULL){
        //         line_piece = strtok(line,";");
        //         while(line_piece != NULL){
        //             errorCode = parseInput(line_piece);	// which calls interpreter()
        //             if (errorCode == -1) exit(99);
        //             line_piece = strtok(NULL,";");
        //         }
        //     }
        //     // if it does not, then execute the line normally
        //     parseInput(line);
        //     memset(line,0,sizeof(line));

        //     if(has_nextpage){
        //         // run first line in next page
        //         // clean up the line buffer to store the command
        //         memset(line,0,sizeof(line));
        //         strcpy(line,next_frame->lines[0]);
        //         // check if the line has ; or not
        //         ret = strchr(line,';');
        //         if(ret != NULL){
        //             line_piece = strtok(line,";");
        //             while(line_piece != NULL){
        //                 errorCode = parseInput(line_piece);	// which calls interpreter()
        //                 if (errorCode == -1) exit(99);
        //                 line_piece = strtok(NULL,";");
        //             }
        //         }
        //         // if it does not, then execute the line normally
        //         parseInput(line);
        //         memset(line,0,sizeof(line));
        //     }
            
            
        //     head->frame_index += 1;
        //     head->line_index = 1;
        // }
    
    
}

// // clean up the current SCRIPT (one script at a time)
// void mem_cleanup(pcb_node *head){
//     int cur_index = head->spot_index;
//     int start_index = cur_index;
//     int total_lines = head->total_lines;

//     for(cur_index; cur_index < start_index + total_lines; cur_index++){
//         var_store[cur_index].var = "none";
//         memset(var_store[cur_index].line,0,sizeof(var_store[cur_index].line));
//     }

//     return;
// }

// void mem_print_dirtymem(){
//     int find_dirty = 0;
//     for(int i = 100; i<1000; i++){
//         if(strcmp(var_store[i].var, "none") != 0){
//             printf("Memory space at: %d,Var: %s, Line: %s,is not cleaned!\n",i,var_store[i].var,var_store[i].line);
//             find_dirty = 1;
//         }       
//     }
//     if(!find_dirty) printf("All codes from scripts are cleaned.\n");
// }
