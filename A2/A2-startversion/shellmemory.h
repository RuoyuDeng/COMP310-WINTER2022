#ifndef shellmemory_H
#define shellmemory_H
#include "scheduler.h"
void mem_init();
char **mem_get_value(char *var);
int mem_set_value(char *var, char **value);
int mem_set_lines(char *var, char *value);
void mem_run_lines(pcb_node *head, int num_lines);
void mem_cleanup(pcb_node *head);
void mem_print_dirtymem();
#endif
