
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"


int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[]) {

	printf("%s\n", "Shell version 1.1 Created January 2022");
	help();

	FILE *tty = fopen("/dev/tty", "r"); // default interact FILE stream to read user inputs
	FILE *file_toread;					
	
	
	char prompt = '$';  				// Shell prompt
	char userInput[MAX_USER_INPUT];		// user's input stored here
	int errorCode = 0;					// zero means no error, default
	char *ret;
	char *input_piece;
	//init user input
	for (int i=0; i<MAX_USER_INPUT; i++)
		userInput[i] = '\0';
	
	//init shell memory
	mem_init();


	while(1) {							
		printf("%c ",prompt);
		// when we have a input file redirected to stdin, change
		// the default write place
		if (stdin != NULL) {
			file_toread = stdin;
		}

		if (feof(stdin)) file_toread = tty;
		// by default, read from the interactive input tty
		fgets(userInput, MAX_USER_INPUT-1, file_toread);

		// split the chained and execute them in a loop IF found a ';'
		ret = strchr(userInput,';');
		if(ret!=NULL){
			input_piece = strtok(userInput,";");
			while(input_piece != NULL){
				errorCode = parseInput(input_piece);
				if (errorCode == -1) exit(99);	// ignore all other errors
				// memset(input_piece, 0, sizeof(input_piece));
				input_piece = strtok(NULL,";");
			}
		}

		errorCode = parseInput(userInput);
		memset(userInput, 0, sizeof(userInput));
		char x = 'c';
	}

	return 0;

}

// Extract words from the input then call interpreter
int parseInput(char ui[]) {
 
	char tmp[200];
	char *words[100];							
	int a,b;							
	int w=0; // extraced word's index (word0,word1,...)


	memset(words, 0, sizeof(words));
	for(a=0; ui[a]==' ' && a<1000; a++);		// skip white spaces at begining of the line

	while(ui[a] != '\0' && a<1000) { // keep iterating till the end of the line

		for(b=0; ui[a]!='\0' && ui[a]!=' ' && a<1000; a++, b++)
			tmp[b] = ui[a];						// extract a word
	 
		tmp[b] = '\0';

		words[w] = strdup(tmp);

		w++;
		
		if(ui[a] == '\0'){
			break;
		}
		a++; 
	}

	return interpreter(words, w);
}
