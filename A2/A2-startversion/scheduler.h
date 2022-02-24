#ifndef scheduler_H
#define scheduler_H

typedef struct pcb{
    int pid;
    int spot_index; // which line is the first line
    int line_index; // which line to read and execute now
    int total_lines;  // what is the max number of lines
    int job_score; // job score for aging
    struct pcb *next;

} pcb_node;
// load the file into memory and create a pcb, if there is a ready queue head
// append such pcb, otherwise make it ready queue head
int loadfile(char *filename,pcb_node **ptr_head);
int fcfspoly(char* filenames[], int filenum);
int sjfpoly(char* filenames[], int filenum);
int rrpoly(char* filenames[], int filenum);
int agingpoly(char* filenames[], int filenum);
#endif