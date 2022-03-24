#include <stdio.h>
#include <stdlib.h>

// void swap(int a, int b)
// {
//     int temp;
//     temp = a;
//     a = b;
//     b = temp;
// }

void swap(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

float compute(int r, float *p)
{
    float a;
    a = 3.14 * r * r;
    *p = 2 * 3.14 * r;
    return a;
}

int main()
{
    int i = 1, j = 2;
    printf("Initial Value of i = %d, and j = %d\n", i, j);
    // swap(i, j);
    swap(&i, &j);
    printf("Final Value of i = %d, and j = %d\n", i, j);

    int r = 2;
    float area, perimeter;
    area = compute(r, &perimeter);
    printf("Area = %f, & Perimeter = %f\n", area, perimeter);

    return 0;
}
