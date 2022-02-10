#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int sum(int x, int y){
    return x + y;
}

int mult(int x, int y){
    return x * y;
}


int main(void) {

    char buf[1000];
    char buf2[1000];

    FILE *tty = fopen("/dev/tty", "r");
    if (!tty) {
        perror("/dev/tty");
        exit(1);
    }
    while (1) {
        fgets(buf, 1000, tty);
        fgets(buf2,1000,stdin);
        printf("%s",buf2);
        if(feof(tty)) break;
    }
}
