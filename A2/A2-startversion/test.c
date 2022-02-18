#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int sum(int x, int y){
    return x + y;
}

int mult(int x, int y){
    return x * y;
}


int main ()
{
    char word[100];
    char str[] ="set x abc; set y 123; print y; print x";
    char str2[] ="set x abc";
    char *pt;
    pt = strtok (str2,";");
    while (pt != NULL) {
        printf("%s\n",pt);
        // move on to next piece of string token
        pt = strtok (NULL, ";");
    }
    return 0;
}
