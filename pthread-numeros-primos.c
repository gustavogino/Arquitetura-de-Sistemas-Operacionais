/******************************************************
 *          Gustavo Gino Scotton - 15201974           *
 *      Pthreads: verificador de números primos       *
 ******************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Modo de depuração, 1 para sim, 0 para não.
int DebugMode = 0; 

// Variaveis globais
int n, LowisPrime, UpperisPrime;
pthread_t low, upper;

// Definição das funções
void *LowPrimeNumberChecker(void *threadid);
void *UpperPrimeNumberChecker(void *threadid);

// Main thread
int main(){
    printf("Digite o número que deseja verificar se é primo: "); 
    scanf("%d", &n);
    
    if (n < 2){
        printf("O número digitado deve ser maior que 1."); 
        return -1;
    }
    
    pthread_create(&low, NULL, LowPrimeNumberChecker, NULL);
    pthread_create(&upper, NULL, UpperPrimeNumberChecker, NULL);
    
    pthread_join(low, NULL);
    pthread_join(upper, NULL);

    if(LowisPrime || UpperisPrime)
        printf("%d",1);
    else    
        printf("%d",0);
    
    return 0;
}

void *LowPrimeNumberChecker(void *threadid){
    if(DebugMode) printf("[TID = %lx]: Low Thread Checker foi criada.\n", pthread_self());
    
    int k = n/4+1;
    if(k < 2) k = 2;
    
    for (int i = 2; i <= k; i++) {
        if (n % i == 0 && n != i) {
            if(DebugMode) printf("Low Thread considera não primo.\n");
            LowisPrime = 0;
            pthread_cancel(upper);
            pthread_exit(NULL);
        }
    }
    
    if(DebugMode) printf("Low Thread descobriu que o número é primo.\n");
    LowisPrime = 1;
    pthread_exit(NULL);
    
}

void *UpperPrimeNumberChecker(void *threadid){
    if(DebugMode) printf("[TID = %lx]: Upper Thread Checker foi criada.\n", pthread_self());
    
    int k = n/4+1;
    if(k < 2) k = 2;
    
    int j = n/2+1;
    if(j < 2) j = 2;
    
    
    for (int i = k; i <= j; i++) {
        if (n % i == 0 && n != i) {
            if(DebugMode) printf("Upper Thread considera não primo.\n");
            pthread_cancel(low);
            UpperisPrime = 0;
            pthread_exit(NULL);
        }
    }
    
    if(DebugMode) printf("Upper Thread descobriu que o número é primo.\n");
    UpperisPrime = 1;
    pthread_exit(NULL);
}
