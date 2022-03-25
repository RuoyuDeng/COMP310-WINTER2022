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
int exec(char* filenames[],int arg_size);
int badcommandFileDoesNotExist();
int badcommandSameFile();
int outofMemoryError();


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
        system("rm -r backing_store");
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
            // // check for identical files
            // if(args_size > 3){
            //     for(int i = 1; i<args_size-1; i++){
            //         for(int j = i+1; j<args_size-1; j++){
            //             if(strcmp(command_args[i],command_args[j]) == 0){
            //                 return badcommandSameFile();
            //             }       
            //         }
            //     }
            // }
            return exec(command_args,args_size); // no need to check for identical files now
        }
        return badcommand();
    } else if (strcmp(command_args[0], "resetmem")==0) {
        if (args_size != 1) return badcommand();
        return var_store_init();
    }
    
    else return badcommand();
}

int help(){

    char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n \
exec prog1 prog2 prog3 POLICY     Executes up to 3 concurrent programs, according to a given scheduling policy\n \
resetmem              Deletes the content of the variable store.";
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

int badcommandSameFile(){
    printf("%s\n","Bad command: same file name");
    return 2;
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
    pcb_node **ptr_head = malloc(8);
    pcb_node *ready_head;
    *ptr_head = NULL;

    //loadfile()
    errCode = loadfile(filename,ptr_head);
    ready_head = *ptr_head;
    if(errCode == -1) {
        return outofMemoryError();
    }else if (errCode == 3){
        return badcommandFileDoesNotExist();
    }
    
    while(!ready_head->is_done){
        // mem_run_lines in RR style
        mem_run_lines(ready_head,3);
    }
    

    // clean up
    // 1. remove all lines of code from shell memory space
    // mem_cleanup(ready_head);
    free(ready_head);
    free(ptr_head);
    
    // print to check if there is any dirty memory
    //mem_print_dirtymem();
    return errCode;
}

int exec(char* filenames[], int args_size){
    // exec prog1 prog2 prog3 FCFS (command_args)
    // all polies (1. the address of first file, 2. total number of files to read)

    if(strcmp(filenames[args_size-1],"RR") == 0){
        return rrpoly(filenames,args_size-2);
    }
    // else if(strcmp(filenames[args_size-1],"FCFS") == 0){
    //     return fcfspoly(filenames,args_size-2);
    // }
    // else if (strcmp(filenames[args_size-1],"SJF") == 0){
    //     return sjfpoly(filenames,args_size-2);
    // }
    // else if(strcmp(filenames[args_size-1],"AGING") == 0){
    //     return agingpoly(filenames,args_size-2);
    // }
    return badcommand();
}




