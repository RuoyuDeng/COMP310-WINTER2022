#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <pthread.h>

struct Matrix {
    unsigned int rows;
    unsigned int cols;
    float *data;
};

struct Matrix *allocMatrix(int rows, int cols) {
    struct Matrix *result = malloc(sizeof(struct Matrix));
    result->rows = rows;
    result->cols = cols;
    result->data = malloc(sizeof(float) * rows * cols);

    for (unsigned int i = 0; i < rows * cols; i++) {
        result->data[i] = (float) rand() / (float) RAND_MAX - 0.5f;
    }

    return result;
}

void singleThreadMatMul(struct Matrix *a, struct Matrix *b, struct Matrix *c) {
    // should have been one shape check, but now we ignore it...
    float *columnbuffer = malloc(sizeof(float) * b->rows);
    for (unsigned int i = 0; i < c->cols; i++) {

        for (unsigned int j = 0; j < b->rows; j++) {
            columnbuffer[j] = b->data[j * b->cols + i];
        }

        for (unsigned int j = 0; j < c->rows; j++) {
            float acc = 0.0f;
            for (unsigned int k = 0; k < a->cols; k++) {
                acc += a->data[j * a->cols + k] * columnbuffer[k];
            }

            c->data[j * c->cols + i] = acc;
        }
    }
    free(columnbuffer);
}

struct PartialMatMulParams {
    struct Matrix *a;
    struct Matrix *b;
    struct Matrix *c;
    unsigned int fromColumn;
    unsigned int toColum;
};

void *partialMatMul(void *params) {
    struct PartialMatMulParams *p = (struct PartialMatMulParams *) params;
    struct Matrix *a = p->a;
    struct Matrix *b = p->b;
    struct Matrix *c = p->c;

    float *columnbuffer = malloc(sizeof(float) * b->rows);
    for (unsigned int i = p->fromColumn; i < p->toColum; i++) {

        for (unsigned int j = 0; j < b->rows; j++) {
            columnbuffer[j] = b->data[j * b->cols + i];
        }

        for (unsigned int j = 0; j < c->rows; j++) {
            float acc = 0.0f;
            for (unsigned int k = 0; k < a->cols; k++) {
                acc += a->data[j * a->cols + k] * columnbuffer[k];
            }

            c->data[j * c->cols + i] = acc;
        }
    }
    free(columnbuffer);
    return NULL;
}

void multiThreadMatMul(struct Matrix *a, struct Matrix *b, struct Matrix *c, unsigned int num_threads) {
    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    struct PartialMatMulParams *params = malloc(sizeof(struct PartialMatMulParams) * num_threads);

    for (int i = 0; i < num_threads; i++){
        params[i].a = a;
        params[i].b = b;
        params[i].c = c;

        params[i].fromColumn = i * (c->cols / num_threads);
        params[i].toColum = (i+1) * (c->cols / num_threads);
        pthread_create(&threads[i],NULL,partialMatMul,&params[i]);
    }

    for (unsigned int i = 0; i < num_threads; i++){
        pthread_join(threads[i],NULL);
    }
    free(threads);
    free(params);

}

float maxDiff(struct Matrix *a, struct Matrix *b) {
    float maxdiff = 0.0f;
    for (int i = 0; i < a->rows * a->cols; i++) {
        float diff = fabsf(a->data[i] - b->data[i]);
        if (diff > maxdiff) {
            maxdiff = diff;
        }
    }
    return maxdiff;
}

int main(void) {

    struct timespec start;
    struct timespec end;

    #define TIMESTAMP(ts) timespec_get(&ts, TIME_UTC)
    #define DURATION (1e9 * (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec))

    struct Matrix *A = allocMatrix(2048, 2048);
    struct Matrix *B = allocMatrix(2048, 2048);
    struct Matrix *C0 = allocMatrix(2048, 2048);
    struct Matrix *C1 = allocMatrix(2048, 2048);

    TIMESTAMP(start);
    singleThreadMatMul(A, B, C0);
    TIMESTAMP(end);

    printf("Single thread: %f s\n", DURATION / 1e9);

    int threadcounts[6] = {1, 2, 4, 8, 16, 32};

    for (int i = 0; i < 6; i++) {
        TIMESTAMP(start);
        multiThreadMatMul(A, B, C1, threadcounts[i]);
        TIMESTAMP(end);

        printf("%d thread: %f s \t\t", threadcounts[i], DURATION / 1e9);
        printf("Max Diff = %f\n\n", maxDiff(C0, C1));
    }
}
