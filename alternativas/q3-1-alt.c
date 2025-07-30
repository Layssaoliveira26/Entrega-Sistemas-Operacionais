#include <stdio.h>
#include <sys/time.h>

#define NUM_TERMS 2000000000LL

int main() {
    struct timeval inicio, fim;
    gettimeofday(&inicio, NULL);

    double soma = 0.0;
    double sinal = 1.0;

    for (long long k = 0; k < NUM_TERMS; k++) {
        soma += sinal / (2 * k + 1);
        sinal *= -1.0;
    }
    gettimeofday(&fim, NULL);
    
    //aqui estamos somando a diferença de segundos e microssegundos, e, por fim, dividindo por 1000000.0 para
    //obter uma resposta em segundos
    double tempo_total = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
    
    printf("Valor de PI: %.9lf\n", 4.0 * soma);
    printf("Tempo Processo (Sequencial): %.2lf s\n", tempo_total);

    return 0;
}