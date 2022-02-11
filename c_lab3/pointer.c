#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    //  Address Operator
    int i;
    i = 3;
    printf ("====================================\n");
    printf (" The value of i = %d\n", i);
    printf (" The address of i = %p\n", &i);
    printf ("====================================\n");

    //  Pointer Declaration, Initialization & Dereferencing
    int *ptr;
    ptr = &i;
    // ptr = NULL;
    printf (" The value of ptr / address ptr is pointing to = %p\n", ptr);
    printf (" The address of ptr = %p\n", &ptr);
    printf (" The value stored in the address pointed by ptr = %d\n", *ptr);
    printf ("====================================\n");
    
    //  Memory Allocation
    ptr = malloc(sizeof(int));
    *ptr = 5;
    printf (" The value of ptr / address ptr is pointing to = %p\n", ptr);
    printf (" The value stored in the address pointed by ptr = %d\n", *ptr);
    printf ("====================================\n");

    // //  Memory Leak Prevention
    // free(ptr);
    // int j = 25;
    // ptr = &j;
    // printf (" The value of ptr / address ptr is pointing to = %p\n", ptr);
    // printf (" The value stored in the address pointed by ptr = %d\n", *ptr);
    // printf ("====================================\n");
    
    //  Pointer Arithmetic
    printf (" The value of ptr / address ptr is pointing to = %p\n", ptr);
    ptr++;
    printf (" The value of ptr / address ptr is pointing to = %p\n", ptr);
    printf ("====================================\n");

    return 0;
}