#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#define MAX_TOKEN_SIZE 6
#define MAX_LINE_SIZE 100


struct memory_struct{
    char *var;
    char *value[MAX_TOKEN_SIZE];
    char *line[MAX_LINE_SIZE]; // used to store a line for the file
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
