#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_TERMS 2000000000LL
#define NUM_THREADS 16
#define TERMS_PER_THREAD (NUM_TERMS / NUM_THREADS)

pthread_mutex_t mutex;
double resultado = 0.0;
double tempo_threads = 0.0;

double partialFormula(long long first_term, long long last_term) {
    double pi_approximation = 0.0;
    double signal = (first_term % 2 == 0) ? 1.0 : -1.0;

    for (long long i = first_term; i < last_term; i++) {
        pi_approximation += signal / (2 * i + 1);
        signal *= -1.0;
    }

    return pi_approximation;
}

void *partialProcessing(void *args) {
    int indice = *((int *)args);
    long long first_term = indice * TERMS_PER_THREAD;
    long long last_term = first_term + TERMS_PER_THREAD;

    struct timeval first_term_thread, last_term_thread;
    gettimeofday(&first_term_thread, NULL);

    double resultado_parcial = partialFormula(first_term, last_term);

    gettimeofday(&last_term_thread, NULL);

    double tempo = (last_term_thread.tv_sec - first_term_thread.tv_sec) + (last_term_thread.tv_usec - first_term_thread.tv_usec) / 1000000.0;

    pthread_mutex_lock(&mutex);
    resultado += resultado_parcial;
    tempo_threads += tempo;
    pthread_mutex_unlock(&mutex);

    printf("TID: %d: %.2f s\n", indice, tempo);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int indices[NUM_THREADS];

    struct timeval first_term_total, last_term_total;
    gettimeofday(&first_term_total, NULL);

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        indices[i] = i;
        pthread_create(&threads[i], NULL, partialProcessing, &indices[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&last_term_total, NULL);

    pthread_mutex_destroy(&mutex);

    double tempo_total = (last_term_total.tv_sec - first_term_total.tv_sec) + (last_term_total.tv_usec - first_term_total.tv_usec) / 1000000.0;

    printf("Valor de PI: %.9lf\n", 4.0 * resultado);
    printf("Total Processo (Paralelo): %.2f s\n", tempo_total);
    printf("Total Threads: %.2f s\n", tempo_threads);

    return 0;
}