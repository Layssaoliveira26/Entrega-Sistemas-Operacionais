// q3_2.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_TERMS 2000000000
#define NUM_THREADS 16
#define PARTIAL_NUM_TERMS (NUM_TERMS / NUM_THREADS)

pthread_mutex_t lock;
double global_sum = 0.0;

typedef struct {
    int tid;
    long start_index;
} ThreadArgs;

void* partialProcessing(void* args) {
    ThreadArgs* data = (ThreadArgs*)args;
    long start = data->start_index;
    long end = start + PARTIAL_NUM_TERMS;
    double signal = (start % 2 == 0) ? 1.0 : -1.0;
    double local_sum = 0.0;

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    for (long i = start; i < end; i++) {
        local_sum += signal / (2 * i + 1);
        signal *= -1.0;
    }

    pthread_mutex_lock(&lock);
    global_sum += local_sum;
    pthread_mutex_unlock(&lock);

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double elapsed = (t_end.tv_sec - t_start.tv_sec) + 
                     (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    printf("TID: %d: %.2lf s\n", data->tid, elapsed);

    free(data);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 0; i < NUM_THREADS; i++) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->tid = i;
        args->start_index = i * PARTIAL_NUM_TERMS;
        pthread_create(&threads[i], NULL, partialProcessing, (void*)args);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double total_time = (end_time.tv_sec - start_time.tv_sec) + 
                        (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Valor de pi: %.9lf\n", 4.0 * global_sum);
    printf("Tempo total (Paralelo): %.2lf s\n", total_time);

    pthread_mutex_destroy(&lock);
    return 0;
}
