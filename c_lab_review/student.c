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
    ta_t ta ={
        .balance = 0,
        .taid = 3001,
        .name = "John",
        .printid = printid
    };
    ta.course_taking = malloc(5 * sizeof(course_t));
    course_t *courses = ta.course_taking+sizeof(course_t);
    courses->courseid = 101;
    courses->coursename = "comp";
    printf("%s %d\n",courses->coursename,courses->courseid);

    courses += sizeof(course_t);
    courses->courseid = 102;
    courses->coursename = "comp";
    printf("%s %d\n",courses->coursename,courses->courseid);
    


    

    
    // arr + 4
    // arr + 1
    ta_t TAs[] = {
        {.taid = 200, .balance = 0},
        {.taid = 201, .balance = 1}
    };
    printf("%zd\n",sizeof(TAs[0]));
    printf("%zd\n",sizeof(ta_t));
    printf("%d\n",TAs[0].taid);
    printf("%d\n",TAs->taid);
    printf("%p\n",&(TAs[0]));

    printf("%d\n",TAs[1].taid);
    printf("%p\n",&(TAs[1]));

    ta_t *curTA = TAs;
    curTA += sizeof(ta_t);
    printf("%d\n",curTA->taid);

    ta_t *ta2;
    int x = 3000;
    int id;
    char *name = "Ruoyu123";
    ta2 = createTA(name);
    ta2->taid = x;
    ta2->printid = printid;
    id = ta2->printid(ta2);

    printf("%d\n",ta.printid(&ta));
    printf("%d\n",id);


    // printf("%s \n",ta2->name);
    // int x = 2022;
    // ta.balance = 20;
    // ta.taid = &x;
    // ta.name = malloc(strlen(ta.name));
    
    // strcpy(ta.name,"Ruoyu");
    // printf("%ld", strlen(ta.name));
    // printf("%s \n",ta.name);
    // printf("%d, %d\n",ta.balance,*(ta.taid));
    // grade_assignment(10);
}
