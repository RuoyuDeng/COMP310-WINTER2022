#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "scheduler.h"
#include "shell.h"

#define MAX_TOKEN_SIZE 6
#define MAX_LINE_SIZE 100
#define FRAME_SIZE 3
int max_framestore = 240;
int max_varstore = 200;

typedef struct memory_struct{
    char *var;
    char *value[MAX_TOKEN_SIZE]; // ls[0] = "word", ls[1] = "word2"
    // char line[MAX_LINE_SIZE]; // used to store a line for the file, "set x 10"
} memvar_t;

typedef struct memory_frame{
    char *lines[FRAME_SIZE];
} frame_t;

memvar_t var_store[240];
frame_t *frame_store[200];

// Helper functions
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
    for (i = 0; i < max_varstore; i++){
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
//     for (i = 0; i < max_framestore; i++){
//         frame_store[i] = NULL;
//     }
// }


// Set key value pair
// return the spot index where we set the variable (>=0)
// return -1 if unable to set the variable (memory full)
// reserve the first 100 spots for variables (index 0 -> 99)
int mem_set_value(char *var, char **value) {
    
    int i, j;
    for (i=0; i<max_varstore; i++){
        if (strcmp(var_store[i].var, var) == 0){
            for (j = 0; value[j] != NULL; j++) {
                var_store[i].value[j] = strdup(value[j]);
            }
            var_store[i].value[j] = NULL;
            return i;
        }
    }

    //Value does not exist, need to find a free spot.
    for (i=0; i<max_varstore; i++){
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

    int i, j;
    for (i = 0; i < max_framestore; i++){
        if(frame_store[i] == NULL){
            frame_store[i] = malloc(sizeof(frame_t));
            for(j = 0; j < FRAME_SIZE; j++){
                frame_store[i]->lines[j] = malloc(1000); // each line has maximum 100 chars
                frame_store[i]->lines[j] = store_lines[j];
            }
            return i; // return the frame index where we insert the frame
        }
        
    }
    return -1; // all full
}

//get value based on input key
char **mem_get_value(char *var_in) {
    int i;

    for (i=0; i<max_varstore; i++){
        if (strcmp(var_store[i].var, var_in) == 0){
            return var_store[i].value;
        } 
    }
    return NULL;
    /* return "Variable does not exist"; */
}

// run all the lines from script stored in shell memory space
void mem_run_lines(pcb_node *head){
    // int cur_index = start_index;
    // int cur_index;
    // int start_index;
    int errorCode = 0;
    char line[100];
    char *line_piece;
    char *ret, all_lines[2];
    int *page_table = head->page_table;
    int max_frame_index = (sizeof(head->page_table) / 4)-1;
    int line_index, new_line_index, frame_index, i;
    int end_flag = 0;
    frame_t *cur_frame = NULL;
    frame_t *next_frame = NULL;
    while(1){
        frame_index = head->frame_index;
        line_index = head->line_index;
        cur_frame = frame_store[page_table[frame_index]];
        // line_index = 0 or 1
        if(end_flag) break;
        if((line_index + 1) % 3 != 0) {
            for(i = line_index; i < line_index+1; i++){
                // clean up the line buffer to store the command
                memset(line,0,sizeof(line));
                if(cur_frame->lines[i] == NULL){
                    end_flag = 1;
                    break;
                }
                strcpy(line,cur_frame->lines[i]);
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
                // if it does not, then execute the line normally
                parseInput(line);
                memset(line,0,sizeof(line));
            }
            new_line_index = line_index % 3;
            if(new_line_index == 0){
                head->frame_index += 1;
            }
            head->line_index = new_line_index;
        }

        // line_index = 2
        else{
            //!!!!!!!!!!!!!!!!!!! Later page fault!!!!!!!!!!!!!!!!!!
            next_frame = frame_store[page_table[frame_index+1]]; 
            //!!!!!!!!!!!!!!!!!!! Later page fault!!!!!!!!!!!!!!!!!!
            all_lines[0] = malloc(1000);
            all_lines[1] = malloc(1000);
            if(next_frame->lines[0] == NULL){
                end_flag = 1;
                break;
            }
            strcpy(all_lines[0],cur_frame->lines[2]);
            strcpy(all_lines[1],next_frame->lines[0]);
            for (i = 0; i <= 1; i++){
                // clean up the line buffer to store the command
                memset(line,0,sizeof(line));
                strcpy(line,all_lines[i]);
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
                // if it does not, then execute the line normally
                parseInput(line);
                memset(line,0,sizeof(line));
            }
            head->frame_index += 1;
            head->line_index = 1;
        }
    }
    
    return;
    
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
