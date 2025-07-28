#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS_P 6
#define NUM_THREADS_C 2
#define B_SIZE 5

int buffer[B_SIZE];
int cont, in, out = 0; // inicializa o contador do buffer e seus controladores
int p_ativas = NUM_THREADS_P; // inicializa o número de produdores ativos

pthread_mutex_t mutex;
pthread_cond_t cond_consumer; // condição consumidor
pthread_cond_t cond_producer; // condição produtor

void *producer(void *args) {
    int n = *((int*) args); // quantidade de valores gerados
    pthread_t id = pthread_self(); // pega o ID da thread
   
    for (int i = 0; i < n; i++) {
        int preco = rand() % 1000 + 1; // faz com que os preços sejam aleatórios
       
        pthread_mutex_lock(&mutex);
        while (cont == B_SIZE) { // enquanto o buffer estiver cheio
            pthread_cond_wait(&cond_producer, &mutex); // produtor espera
        }
        buffer[in] = preco;
        in = (in + 1) % B_SIZE; // atualiza a entrada do buffer
        cont++; // adiciona item ao buffer

        printf("(P) TID: %lu | VALOR: R$ %d | ITERAÇÃO: %d\n", id, preco, i+1);
        pthread_cond_signal(&cond_consumer); // manda sinal de que já existe dado para consumidor, acordar apenas 1 é suficiente
        pthread_mutex_unlock(&mutex);
        sleep((rand() % 5) + 1); // adiciona o delay
    }

    pthread_mutex_lock(&mutex);
        p_ativas--; //reduz o número de threads produtoras ativas.
        pthread_cond_broadcast(&cond_consumer); // acorda as consumidoras para testar novamente. Todas precisam saber se a condição de finalizar foi atendida.
    pthread_mutex_unlock(&mutex);
    printf("(P) TID: %lu finalizou\n", id);
    return NULL;
}
    

void *consumer(void *args) {
    srand(time(NULL));
    int iteracao = 0;
    pthread_t id = pthread_self(); // pega o ID da thread

    while(1) {
        pthread_mutex_lock(&mutex);
        while (cont < B_SIZE && p_ativas > 0) { // espera enquanto o buffer não estiver cheio e ainda houverem produtoras ativas
            pthread_cond_wait(&cond_consumer, &mutex);
        }
        if (p_ativas == 0 && cont < B_SIZE) { // se não tiverem mais rodutoras ativas e não houver dados suficientes para a média (buffer não está cheio)
            pthread_mutex_unlock(&mutex);
            break; // termina
        }
        int soma = 0;
        for (int i = 0; i < B_SIZE; i++) { //percorre o buffer
            soma += buffer[out]; // adiciona o item da saída a soma
            out = (out + 1) % B_SIZE; // atualiza a posição de saída do buffer
            cont--; // remove item do buffer
        }
        pthread_cond_broadcast(&cond_producer); // avisa as produtoras que há espaço no buffer
        pthread_mutex_unlock(&mutex);
        float media = (float)soma / B_SIZE; // faz a media dos valores do buffer
        printf("(C) TID: %lu | MEDIA: R$ %.2f | ITERAÇÂO: %d\n", id, media, iteracao + 1);
        iteracao++;
    }
    printf("(C) TID: %lu finalizou\n", id);
    return NULL;
}

int main() {
    int quant [NUM_THREADS_P];
    pthread_mutex_init(&mutex, NULL);

    pthread_t producer_thread[NUM_THREADS_P];
    for (int i = 0; i < NUM_THREADS_P; i++) {
        quant[i] = 25;
        pthread_create(&producer_thread[i], NULL, &producer, &quant[i]);
    }

    pthread_t consumer_thread[NUM_THREADS_C];
    for (int i = 0; i < NUM_THREADS_C; i++) {
        pthread_create(&consumer_thread[i], NULL, &consumer, NULL);
    }

    for (int i = 0; i < NUM_THREADS_P; i++) {
        pthread_join(producer_thread[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS_C; i++) {
        pthread_join(consumer_thread[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}