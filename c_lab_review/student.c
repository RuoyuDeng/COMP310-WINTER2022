#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "ta.h"

ta_t *createTA(char *name){
    ta_t *ta = malloc(sizeof(ta_t));
    ta->name = malloc(strlen(name));
    strcpy(ta->name,name);
    return ta;
}

int main(char *argv[], int argc){
    ta_t ta;
    ta_t *ta2;
    char *name = "Ruoyu123";
    ta2 = createTA(name);
    printf("%s \n",ta2->name);
    int x = 2022;
    ta.balance = 20;
    ta.taid = &x;
    ta.name = malloc(strlen(ta.name));
    
    strcpy(ta.name,"Ruoyu");
    printf("%ld", strlen(ta.name));
    printf("%s \n",ta.name);
    printf("%d, %d\n",ta.balance,*(ta.taid));
    grade_assignment(10);
}
