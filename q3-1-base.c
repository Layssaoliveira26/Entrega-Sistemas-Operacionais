#include <stdio.h>
#include <time.h>

#define NUM_TERMS 2000000000

int main() {
    clock_t start = clock();
    double sum = 0.0;
    double signal = 1.0;

    for (long long k = 0; k < NUM_TERMS; k++) {
        sum += signal / (2 * k + 1);
        signal *= -1.0;
    }

    double pi = 4.0 * sum;

    clock_t end = clock();  
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Valor de pi: %.9lf\n", pi);
    printf("Tempo total (Sequencial): %.2lf s\n", time_spent);

    return 0;
}
