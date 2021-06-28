/******************************************************
 *          Gustavo Gino Scotton - 15201974           *
 *             Pthreads: contador e mutex             *
 ******************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Variáveis Globais
int g_count = 0;
pthread_mutex_t mutex;

// Definição da Função
void *PrintInThread(void *threadid);
 
// Main thread
int main(){
    int tx, i;
  
    // Faz a leitura da quantidade de Thread desejadas
    printf("Digite a quantidade de threads que deseja: ");
    scanf("%d", &tx);
  
    // Define a quantidade de threads com base na entrada
    pthread_t thread[tx];
  
    // Inicializa o mutex
    pthread_mutex_init(&mutex, NULL);

    // Cria o número de threads informado pelo user
    for (i = 0; i < tx; i++)
        pthread_create(&thread[i], NULL, PrintInThread, NULL);

    // Suspende a ação da Main Thread até que as outras threads terminem
    for (i = 0; i < tx; i++)
        pthread_join(thread[i], NULL);

    // Exite o valor do contador global
    printf("[Main Thread]: Contador Global: %d\n", g_count);
  
    // Finaliza o mutex
    pthread_mutex_destroy(&mutex);
  
    return 0;
}

void *PrintInThread(void *threadid){
    // Exibe a thread id de forma hexadecimal
    printf("[TID = %lx]: Nova thread criada.\n", pthread_self());
    
    pthread_mutex_lock(&mutex);     // Bloqueia os recursos 
    for (int i = 0; i < 100; i++)   // De 0 ~ 99 (100 iterações)
        g_count++;                  // Incrementa o contador global
    pthread_mutex_unlock(&mutex);   // Desbloqueia os recursos 
    
    pthread_exit(NULL);             // Finaliza a thread
}
