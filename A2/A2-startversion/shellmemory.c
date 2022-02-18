#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "scheduler.h"
#include "shell.h"

#define MAX_TOKEN_SIZE 6
#define MAX_LINE_SIZE 100


struct memory_struct{
    char *var;
    char *value[MAX_TOKEN_SIZE]; // ls[0] = "word", ls[1] = "word2"
    char line[MAX_LINE_SIZE]; // used to store a line for the file, "set x 10"
};
//a.txt
// var = a.txt1 value= line1
/*
var a.txt2 value line2
*/
struct memory_struct shellmemory[1000];

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

void mem_init(){

    int i;
    for (i=0; i<1000; i++){		
        shellmemory[i].var = "none";
    }
}

// Set key value pair
void mem_set_value(char *var, char **value) {

    int i, j;

    for (i=0; i<1000; i++){
        if (strcmp(shellmemory[i].var, var) == 0){
            for (j = 0; value[j] != NULL; j++) {
                shellmemory[i].value[j] = strdup(value[j]);
            }
            shellmemory[i].value[j] = NULL;
            return;
        }
    }

    //Value does not exist, need to find a free spot.
    for (i=0; i<1000; i++){
        if (strcmp(shellmemory[i].var, "none") == 0){
            shellmemory[i].var = strdup(var);
            for (j = 0; value[j] != NULL; j++) {
                shellmemory[i].value[j] = strdup(value[j]);
            }
            return;
        } 
    }

    return;

}

// return the first free spot to insert variable 
// return -1 if the shell memory space is full
int mem_set_lines(char *var, char *value) {

    int i;
    // check if the current variable exists in the memory space
    for (i=0; i<1000; i++){
        if (strcmp(shellmemory[i].var, var) == 0){
            strcpy(shellmemory[i].line,value);
            return i;
        }
    }

    //Value does not exist, need to find a free spot.
    for (i=0; i<1000; i++){
        if (strcmp(shellmemory[i].var, "none") == 0){
            shellmemory[i].var = strdup(var);
            strcpy(shellmemory[i].line,value);
            return i;
        } 
    }

    return -1;

}

//get value based on input key
char **mem_get_value(char *var_in) {
    int i;

    for (i=0; i<1000; i++){
        if (strcmp(shellmemory[i].var, var_in) == 0){
            return shellmemory[i].value;
        } 
    }
    return NULL;
    /* return "Variable does not exist"; */
}

// run all the lines from script stored in shell memory space
void mem_run_lines(pcb_node *head){
    // int cur_index = start_index;
    int cur_index = head->spot_index;
    int total_lines = head->total_lines;
    int errorCode = 0;
    char line[100];
    char *line_piece;
    char *ret;
    for(cur_index; cur_index<total_lines; cur_index++){
        // clean up the line buffer to store the command
        memset(line,0,sizeof(line));
        // fgets(line,100,shellmemory[cur_index].line);
        strcpy(line,shellmemory[cur_index].line);
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
        // done with the current line, record in PCB that we need to do next line
        head->line_index++;
        memset(line,0,sizeof(line));
    }
    return;
    
}


void mem_cleanup(pcb_node *head){
    int cur_index = head->spot_index;
    int total_lines = head->total_lines;

    for(cur_index; cur_index < total_lines; cur_index++){
        shellmemory[cur_index].var = "none";
        memset(shellmemory[cur_index].line,0,sizeof(shellmemory[cur_index].line));
    }

}
