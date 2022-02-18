#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "shell.h"

int loadfile(char *filename){
    char line[1000];
    char *line_piece;
    char *ret;

    FILE *file = fopen(script,"rt");  // the program is in a file

    if(file == NULL){
		printf("%s\n", "Bad command: File not found");
        return 3;
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
    
}