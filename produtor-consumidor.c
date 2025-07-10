void * producer ( void * args ) {
int n = *(( int *) args ) ;

while (n - - > 0) {
// acessar buffer compartilhado ( produzir )
// sinalizar dados
// imprimir TID / dados
// esperar por um tempo aleatorio
}
// imprimir que finalizou
}

void * consumer ( void * args ) {
while ( /* tem produtoras ainda ? */ ) {
// esperar 5 dados
// acessar buffer compartilhado ( consumir )
// imprimir TID / dados
}
// imprimir que finalizou
}

int main ( void ) {
// criar threads produtoras
// criar thread consumidora
// esperar threads terminarem
}