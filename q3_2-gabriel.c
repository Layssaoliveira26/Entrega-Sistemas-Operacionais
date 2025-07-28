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
double time_by_thread = 0.0;

typedef struct {
    int tid;
    long start_index;
} ThreadArgs;

double partialFormula (int start, int end) {
    double signal = (start % 2 == 0) ? 1.0 : -1.0;
    double local_sum = 0.0;
    
    for (long i = start; i < end; i++) {
        local_sum += signal / (2 * i + 1);
        signal *= -1.0;
    }
    
    return local_sum;
}

void* partialProcessing(void* args) {
    ThreadArgs* data = (ThreadArgs*)args;
    long start = data->start_index;
    long end = start + PARTIAL_NUM_TERMS;

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    double local_sum = partialFormula(start, end);

    clock_gettime(CLOCK_MONOTONIC, &t_end);

    double elapsed = (t_end.tv_sec - t_start.tv_sec) + 
                     (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    pthread_mutex_lock(&lock);
    global_sum += local_sum;
    time_by_thread += elapsed;
    pthread_mutex_unlock(&lock);

    printf("TID: %d: %.2lfs\n", data->tid, elapsed);

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
    double total_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Valor de PI: %.9lf\n", 4.0 * global_sum);
    //verificar com Botelho se Total Processo (Parelo) é a média do tempo das thread
    printf("Tempo Processo (Paralelo): %.2lf s\n", time_by_thread / NUM_THREADS);
    printf("Tempo Threads: %.2lf s\n", time_by_thread);

    pthread_mutex_destroy(&lock);
    return 0;
}