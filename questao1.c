#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS_P 3
#define B_SIZE 5

int buffer[B_SIZE];
int cont, in, out = 0; 
int p_ativas = NUM_THREADS_P; 

pthread_mutex_t mutex;
pthread_cond_t cond_consumer; 
pthread_cond_t cond_producer; 

void *producer(void *args) {
    int n = *((int*) args); 
    pthread_t id = pthread_self(); /
   
    for (int i = 0; i < n; i++) {
        int preco = rand() % 1000 + 1; 
       
        pthread_mutex_lock(&mutex);
        while (cont == B_SIZE) { 
            pthread_cond_wait(&cond_producer, &mutex); 
        }
        buffer[in] = preco;
        in = (in + 1) % B_SIZE; 
        cont++; 

        printf("(P) TID: %lu | VALOR: R$ %d | ITERAÇÃO: %d\n", id, preco, i+1);
        pthread_cond_signal(&cond_consumer); 
        pthread_mutex_unlock(&mutex);
        sleep((rand() % 5) + 1); 
    }

    pthread_mutex_lock(&mutex);
        p_ativas--;
        pthread_cond_signal(&cond_consumer); 
    pthread_mutex_unlock(&mutex);
    printf("(P) TID: %lu finalizou\n", id);
    return NULL;
}
    

void *consumer(void *args) {
    int iteracao = 0;
    pthread_t id = pthread_self(); 

    while(1) {
        pthread_mutex_lock(&mutex);
        while (cont < B_SIZE && p_ativas > 0) { 
            pthread_cond_wait(&cond_consumer, &mutex);
        }
        if (p_ativas == 0 && cont < B_SIZE) { 
            pthread_mutex_unlock(&mutex);
            break; 
        }
        int soma = 0;
        for (int i = 0; i < B_SIZE; i++) { 
            soma += buffer[out]; 
            out = (out + 1) % B_SIZE; 
            cont--; 
        }
        pthread_cond_broadcast(&cond_producer); 
        pthread_mutex_unlock(&mutex);
        float media = (float)soma / B_SIZE; 
        printf("(C) TID: %lu | MEDIA: R$ %.2f | ITERAÇÂO: %d\n", id, media, iteracao + 1);
        iteracao++;
    }
    printf("(C) TID: %lu finalizou\n", id);
    return NULL;
}

int main() {
    srand(time(NULL));
    int quant [NUM_THREADS_P];
    pthread_mutex_init(&mutex, NULL);

    pthread_t producer_thread[NUM_THREADS_P];
    for (int i = 0; i < NUM_THREADS_P; i++) {
        quant[i] = 25;
        pthread_create(&producer_thread[i], NULL, &producer, &quant[i]);
    }

    pthread_t consumer_thread;
    pthread_create(&consumer_thread, NULL, &consumer, NULL);

    for (int i = 0; i < NUM_THREADS_P; i++) {
        pthread_join(producer_thread[i], NULL);
    }

    pthread_join(consumer_thread, NULL);
    pthread_mutex_destroy(&mutex);

    return 0;
}
