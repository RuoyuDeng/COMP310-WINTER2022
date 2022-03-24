#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "ta.h"

int printid(ta_t *ta){
    printf("His id is: %d \n",ta->taid);
    return ta->taid;
}
void grade_assignment(int grade){
    printf("The student's grade is %d \n",grade);
}
