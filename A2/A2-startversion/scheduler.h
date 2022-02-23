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
int loadfile(char *filename,pcb_node *ready_head);
pcb_node* pophead_pcb(pcb_node **ptr_head);
void append_pcb(pcb_node *head,int spot_index,int line_index,int total_lines,int job_score);
#endif