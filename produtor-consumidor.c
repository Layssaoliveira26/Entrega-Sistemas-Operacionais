#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define BUFFER_SIZE 5
#define NUM_PRODUTORES 3

int buffer[BUFFER_SIZE];      // Buffer compartilhado
int in = 0;                   // Índice de inserção
int out = 0;                  // Índice de remoção
int count = 0;                // Número atual de elementos no buffer

pthread_mutex_t mutexBuffer = PTHREAD_MUTEX_INITIALIZER;   // Protege o buffer
pthread_mutex_t mutexPrint = PTHREAD_MUTEX_INITIALIZER;    // Protege as impressões
pthread_cond_t condBufferCheio = PTHREAD_COND_INITIALIZER; // Condição para o consumidor saber que o buffer está cheio

int produtoresAtivos = NUM_PRODUTORES; // Contador de produtoras ativas

// Função para gerar número aleatório entre min e max
int aleatorio(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Função da thread produtora
void *produtora(void *arg) {
    int qtd = *((int *)arg); // Quantidade de valores que essa thread vai produzir
    pthread_t tid = pthread_self(); // ID da thread

    for (int i = 1; i <= qtd; i++) {
        int valor = aleatorio(1, 1000); // Gera valor de venda

        pthread_mutex_lock(&mutexBuffer);
        while (count == BUFFER_SIZE) {
            pthread_mutex_unlock(&mutexBuffer);
            usleep(100000); // Espera 100ms se buffer estiver cheio
            pthread_mutex_lock(&mutexBuffer);
        }

        buffer[in] = valor;
        in = (in + 1) % BUFFER_SIZE;
        count++;

        pthread_cond_signal(&condBufferCheio); // Avisa o consumidor

        pthread_mutex_lock(&mutexPrint);
        printf("(P) TID: %lu | VALOR: R$ %d | ITERACAO: %d\n", (unsigned long)tid, valor, i);
        pthread_mutex_unlock(&mutexPrint);

        pthread_mutex_unlock(&mutexBuffer);

        sleep(aleatorio(1, 5)); // Espera aleatória entre 1 e 5 segundos
    }

    // Finaliza produtora
    pthread_mutex_lock(&mutexBuffer);
    produtoresAtivos--; // Decrementa contador de produtoras ativas
    pthread_cond_signal(&condBufferCheio); // Avisa o consumidor
    pthread_mutex_unlock(&mutexBuffer);

    pthread_mutex_lock(&mutexPrint);
    printf("(P) TID: %lu finalizou\n", (unsigned long)tid);
    pthread_mutex_unlock(&mutexPrint);

    pthread_exit(NULL);
}

// Função da thread consumidora
void *consumidora(void *arg) {
    int iteracao = 1;
    pthread_t tid = pthread_self();

    while (1) {
        pthread_mutex_lock(&mutexBuffer);

        // Aguarda até ter 5 elementos ou até que produtoras acabem
        while (count < BUFFER_SIZE && produtoresAtivos > 0) {
            pthread_cond_wait(&condBufferCheio, &mutexBuffer);
        }

        // Encerra se não há produtoras e buffer está incompleto
        if (produtoresAtivos == 0 && count < BUFFER_SIZE) {
            pthread_mutex_unlock(&mutexBuffer);
            break;
        }

        // Consome 5 elementos
        int soma = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            soma += buffer[out];
            out = (out + 1) % BUFFER_SIZE;
            count--;
        }

        pthread_mutex_unlock(&mutexBuffer);

        int media = soma / BUFFER_SIZE;

        pthread_mutex_lock(&mutexPrint);
        printf("(C) TID: %lu | MEDIA: R$ %d | ITERACAO: %d\n", (unsigned long)tid, media, iteracao++);
        pthread_mutex_unlock(&mutexPrint);
    }

    pthread_mutex_lock(&mutexPrint);
    printf("(C) TID: %lu finalizou\n", (unsigned long)tid);
    pthread_mutex_unlock(&mutexPrint);

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));

    pthread_t produtores[NUM_PRODUTORES];
    pthread_t consumidor;

    int producoes[NUM_PRODUTORES];

    // Cria produtoras com quantidades aleatórias de produções
    for (int i = 0; i < NUM_PRODUTORES; i++) {
        producoes[i] = aleatorio(20, 30);
        pthread_create(&produtores[i], NULL, produtora, &producoes[i]);
        sleep(1); // Garante que cada thread leia sua quantidade corretamente
    }

    // Cria thread consumidora
    pthread_create(&consumidor, NULL, consumidora, NULL);

    // Espera produtoras finalizarem
    for (int i = 0; i < NUM_PRODUTORES; i++) {
        pthread_join(produtores[i], NULL);
    }

    // Espera consumidor finalizar
    pthread_join(consumidor, NULL);

    return 0;
}
