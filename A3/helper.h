#ifndef HELPER_H
#define HELPER_H
#include"scheduler.h"
void swappcb(pcb_node *largeNode, pcb_node *smallNode);
void sortReadyQueue(pcb_node *head, char* mode);

#endif