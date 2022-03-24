#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include"helper.h"
#include"scheduler.h"

// swap 2 nodes
void swappcb(pcb_node *largeNode, pcb_node *smallNode){
    int tmp_pid = smallNode->pid;
    int tmp_spotindex = smallNode->spot_index;
    int tmp_lineindex = smallNode->line_index;
    int tmp_totallines = smallNode->total_lines;
    int tmp_jobscore = smallNode->job_score;

    smallNode->pid = largeNode->pid;
    smallNode->spot_index = largeNode->spot_index;
    smallNode->line_index = largeNode->line_index;
    smallNode->total_lines = largeNode->total_lines;
    smallNode->job_score = largeNode->job_score;

    largeNode->pid = tmp_pid;
    largeNode->spot_index = tmp_spotindex;
    largeNode->line_index = tmp_lineindex;
    largeNode->total_lines = tmp_totallines;
    largeNode->job_score = tmp_jobscore;
}

void sortReadyQueue(pcb_node *head, char *mode){
    int done = 1;

    pcb_node *queue_end = NULL;
    pcb_node *cur_node;

    // NULL head check
    if(head == NULL) return;
    
    while(done){
        // assume we can finish sorting this iteration -> no swapping happened means it is sorted (small to large)
        done = 0;
        cur_node = head;
        
        // keep swapping until the max total lines of pcb goes to the end of queue
        while(cur_node->next != queue_end){
            // sort based on total lines
            if(strcmp(mode,"SJF") == 0){
                if(cur_node->total_lines > cur_node->next->total_lines){
                    swappcb(cur_node,cur_node->next);
                    // some swap happened, keep iterating
                    done = 1;
                }
            }
            // sort based on job age
            else if (strcmp(mode,"AGING") == 0){
                if(cur_node->job_score > cur_node->next->job_score){
                    swappcb(cur_node,cur_node->next);
                    // some swap happened, keep iterating
                    done = 1;
                }
            }
            
            cur_node = cur_node->next;
        }
        // inner loop terminates when cur_node->next at the end, treat cur_node as the new end
        queue_end = cur_node;
    }
}
