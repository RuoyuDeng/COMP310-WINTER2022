#ifndef scheduler_H
#define scheduler_H

typedef struct pcb{
    // each pcb stores info about 1 prog
    
    // int spot_index; // which line is the first line
    // int line_index; // which line to read and execute now
    
    // int job_score; // job score for aging
    
    int pid;
    int total_lines;  // what is the max number of lines
    int frame_index;      // current working frame of program
    int line_index;      // current working line of current working frame
    int *page_table;  // all frame indexes which locate where are frames of prog1 are in frame_store
    int *active_pagetable; // the meaning of active_pagetable[i] if active_pagetable[i] >= 0: active / -1: not in use page /-2: used, already finished executing
    int is_done;
    int is_lastframe;
    char *filename;
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