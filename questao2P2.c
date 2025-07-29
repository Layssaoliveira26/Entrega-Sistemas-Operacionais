#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_TERMS 2000000000L
#define NUM_THREADS 16
#define TERMS_PER_THREAD (NUM_TERMS / NUM_THREADS)

pthread_mutex_t mutex;
double resultado = 0.0;
double tempo_threads = 0.0;

double calcular_parcial(long long inicio, long long fim) {
    double soma = 0.0;
    double sinal = (inicio % 2 == 0) ? 1.0 : -1.0;

    for (long long i = inicio; i < fim; i++) {
        soma += sinal / (2 * i + 1);
        sinal *= -1.0;
    }

    return soma;
}

void *calcular_pi(void *arg) {
    int indice = *((int *)arg);
    long long inicio = indice * TERMS_PER_THREAD;
    long long fim = inicio + TERMS_PER_THREAD;

    struct timeval inicio_thread, fim_thread;
    gettimeofday(&inicio_thread, NULL);

    double resultado_parcial = calcular_parcial(inicio, fim);

    gettimeofday(&fim_thread, NULL);

    double tempo = (fim_thread.tv_sec - inicio_thread.tv_sec) + (fim_thread.tv_usec - inicio_thread.tv_usec) / 1000000.0;

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

    struct timeval inicio_total, fim_total;
    gettimeofday(&inicio_total, NULL);

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        indices[i] = i;
        pthread_create(&threads[i], NULL, calcular_pi, &indices[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&fim_total, NULL);

    pthread_mutex_destroy(&mutex);

    double tempo_total = (fim_total.tv_sec - inicio_total.tv_sec) + (fim_total.tv_usec - inicio_total.tv_usec) / 1000000.0;

    printf("Total Processo (Paralelo): %.2f s\n", tempo_total);
    printf("Total Threads: %.2f s\n", tempo_threads);
    printf("Valor de PI: %.9lf\n", 4.0 * resultado);

    return 0;
}
