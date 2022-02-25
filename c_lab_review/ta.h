#ifndef TA_H
#define TA_H


void grade_assignment(int grade);
typedef struct course{
    int courseid;
    char *coursename;
}course_t;
typedef struct ta{
    int balance;
    int taid;
    char *name;
    int (*printid) (struct ta* ta);
    course_t *course_taking;
}ta_t;
int printid(ta_t* ta);

#endif