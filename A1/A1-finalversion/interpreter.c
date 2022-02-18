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
    } else return badcommand();
}

int help(){

    char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
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

int set(char* var, char** value){
    mem_set_value(var, value);
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
int run(char* script){
    int errCode = 0;

    
    char line[1000];
    char *line_piece;
    char *ret;


    //loadfile()
    


    FILE *p = fopen(script,"rt");  // the program is in a file

    if(p == NULL){
		return badcommandFileDoesNotExist();
	}
    // load all lines of code into memory space (set_file)
    // TODO:
    





    // read from memory space with the start index and max_len of codes to make sure every line gets executed
    // send to shell to execute based on PCB which is the return type of loadfile
	fgets(line,999,p);
	while(1){
		// break the line into pieces
        ret = strchr(line,';');
        if(ret != NULL){
            line_piece = strtok(line,";");
            while(line_piece != NULL){
                errCode = parseInput(line_piece);	// which calls interpreter()
                if (errCode == -1) exit(99);
                line_piece = strtok(NULL,";");
            }
        }
		
        errCode = parseInput(line);
		memset(line, 0, sizeof(line)); // Sets the first num bytes of the block of memory pointed by ptr to the specified value

		if(feof(p)){
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}