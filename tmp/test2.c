#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct memory_struct{
    char *var;
    char *line[100]; // used to store a line for the file
};

int main(){
    // char filename[] = "1.txt";
    // char cpyname[20];
    // FILE *file = fopen(filename,"rt");
    // char line[1000];
    // char *line_piece;
    // char *ret;
    // int lineindex = 0;
    // int start_line_index = -2;
    // char file_var_buffer[30];
    // fgets(line,999,file);
    // while(1){
    //     strcpy(cpyname,filename);
    //     // make sure the variable name is correct
    //     memset(file_var_buffer,0,sizeof(file_var_buffer));
    //     // add the line of index to the end of file name and set it into shell memory
    //     snprintf(file_var_buffer,sizeof file_var_buffer,"%d",lineindex);
    //     strcat(cpyname,file_var_buffer);

    //     printf("%s \n",cpyname);

    //     if(feof(file)) break;
    //     fgets(line,999,file);
    //     lineindex++;
    // }

    // fclose(file);

    char *line[10];
    char newline[20];
    memset(newline,0,sizeof(newline));
    line[0] = "set";
    line[1] = " ";
    line[2] = "xyz";
    strcat(newline,line[0]);
    strcat(newline,line[1]);
    strcat(newline,line[2]);
    printf("%s\n",newline);
}