#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <pthread.h>

float singleThreadDotProd(float *a, float *b, int len) {
    float result = 0.0f;
    for (int i = 0; i < len; i++) {
        result += a[i] * b[i];
    }
    return result;
}

struct DotProductParams {
    

};

void *partialDotProduct(void *params) {

}

float multiThreadDotProduct(float *a, float *b, int len, int num_threads) {
    return 0.0f;
}

int main(void) {

    struct timespec start;
    struct timespec end;

    #define TIMESTAMP(ts) timespec_get(&ts, TIME_UTC)
    #define DURATION (1e9 * (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec))

    int vec_len = 1 << 25;

    float *A = malloc(sizeof(float) * vec_len);
    float *B = malloc(sizeof(float) * vec_len);

    for (int i = 0; i < vec_len; i++) {
        A[i] = (float) rand() / (float) RAND_MAX - 0.5f;
        B[i] = (float) rand() / (float) RAND_MAX - 0.5f;        
    }

    TIMESTAMP(start);
    float c0 = singleThreadDotProd(A, B, vec_len);
    TIMESTAMP(end);

    printf("Single Thread: %f ms\n", DURATION / 1e6);
    printf("Value: %f\n\n", c0);

    for (int i = 0; i < 5; i++) {
        TIMESTAMP(start);
        float c1 = multiThreadDotProduct(A, B, vec_len, 4);
        TIMESTAMP(end);

        printf("4 Threads: %f ms, Value = %f\n", DURATION / 1e6, c1);
    }
}
