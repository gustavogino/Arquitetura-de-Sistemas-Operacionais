/******************************************************
 *          Gustavo Gino Scotton - 15201974           *
 *     pthread - Problema do Jantar dos filosofos     *
 ******************************************************/

// DEFINA MUTEX 1 CASO QUEIRA QUE O PROBLEMA DOS FISOLOFOS SEJA RESOLVIDO 
// DEFINA MUTEX 0 CASO QUEIRA QUE O PROBLEMA PROPOSTO SEJA RESOLVIDO
#define MUTEX 1

#define LEFT (id + 5 - 1) % 5
#define RIGHT (id + 1) % 5 

// Variáveis Globais
int estado[5] = {0,0,0,0,0};
pthread_mutex_t mutex;

// Definição dos filósofos
void *Socrates(void *threadid);
void *Platao(void *threadid);
void *Aristoteles(void *threadid);
void *Nietzsche(void *threadid);
void *Kant(void *threadid);

// Definição das funções úteis
void PegaGarfo(int id);
void *VerificaComendos(void *threadid);
int Aleatoriedade(int min, int max);

const char *names[] = {"Socrates", "Platao", "Aristoteles", "Nietzsche", "Kant"};


// Main thread
int main(){
    srand(time(NULL) * getpid());
    
    printf("[Main Thread] Os estados são:\n\t[0] Pensando\n\t[1] Faminto\n\t[2] Comendo\n\n");
    printf("[Main Thread] Os filosofos são:\n\t[0] Socrates\n\t[1] Platao\n\t[2] Aristoteles\n\t[3] Nietzsche\n\t[4] Kant\n\n");
    pthread_t t_Socrates, t_Platao, t_Aristoteles, t_Nietzsche, t_Kant;
  
    // Inicializa o mutex
    if(MUTEX) pthread_mutex_init(&mutex, NULL);
    
    // Cria as threads dos filosofos
    pthread_create(&t_Socrates, NULL, Socrates, NULL);
    pthread_create(&t_Platao, NULL, Platao, NULL);
    pthread_create(&t_Aristoteles, NULL, Aristoteles, NULL);
    pthread_create(&t_Nietzsche, NULL, Nietzsche, NULL);
    pthread_create(&t_Kant, NULL, Kant, NULL);
    
#if !MUTEX 
    pthread_t t_falha;
    pthread_create(&t_falha, NULL, VerificaComendos, NULL);
#endif

    // Suspende a ação da Main Thread até que as outras threads terminem
    pthread_join(t_Socrates, NULL);
    pthread_join(t_Platao, NULL);
    pthread_join(t_Aristoteles, NULL);
    pthread_join(t_Nietzsche, NULL);
    pthread_join(t_Kant, NULL);
    
#if !MUTEX 
    pthread_join(t_falha, NULL);
#endif

    // Finaliza o mutex
    if(MUTEX) pthread_mutex_destroy(&mutex);
  
    return 0;
}

void *Socrates(void *threadid){
    printf("[ID: 0] Socrates Thread criada com o TID: %lx\n", pthread_self());
    while (true){
        usleep(Aleatoriedade(1, 5) * 1000000); 
        int newEstado = Aleatoriedade(0, 1);
        
        if(newEstado == 0 && estado[0] == 2){
          printf("Socrates está pensando!\n");
          if(MUTEX) pthread_mutex_lock(&mutex);
          estado[0] = 0;
          if(MUTEX) pthread_mutex_unlock(&mutex);
        }else{
            PegaGarfo(0);
        }
    }
    pthread_exit(NULL);
}

void *Platao(void *threadid){
    printf("[ID: 1] Platão Thread criada com o TID: %lx\n", pthread_self());
    while (true){
        usleep(Aleatoriedade(1, 5) * 1000000); 
        int newEstado = Aleatoriedade(0, 1);
        
        if(newEstado == 0 && estado[1] == 2){
          printf("Platão está pensando!\n");
          if(MUTEX) pthread_mutex_lock(&mutex);
          estado[1] = 0;
          if(MUTEX) pthread_mutex_unlock(&mutex);
        }else{
            PegaGarfo(1);
        }
    }
    pthread_exit(NULL);
}

void *Aristoteles(void *threadid){
    printf("[ID: 2] Aristoteles Thread criada com o TID: %lx\n", pthread_self());
    while (true){
        usleep(Aleatoriedade(1, 5) * 1000000); 
        int newEstado = Aleatoriedade(0, 1);
        
        if(newEstado == 0 && estado[2] == 2){
          printf("Aristoteles está pensando!\n");
          if(MUTEX) pthread_mutex_lock(&mutex);
          estado[2] = 0;
          if(MUTEX) pthread_mutex_unlock(&mutex);
        }else{
            PegaGarfo(2);
        }
    }
    pthread_exit(NULL);
}

void *Nietzsche(void *threadid){
    printf("[ID: 3] Nietzsche Thread criada com o TID: %lx\n", pthread_self());
    while (true){
        usleep(Aleatoriedade(1, 5) * 1000000); 
        int newEstado = Aleatoriedade(0, 1);
        
        if(newEstado == 0 && estado[3] == 2){
          printf("Nietzsche está pensando!\n");
          if(MUTEX) pthread_mutex_lock(&mutex);
          estado[3] = 0;
          if(MUTEX) pthread_mutex_unlock(&mutex);
        }else{
            PegaGarfo(3);
        }
    }
    pthread_exit(NULL);
}
void *Kant(void *threadid){
    printf("[ID: 4] Kant Thread criada com o TID: %lx\n", pthread_self());
    while (true){
        usleep(Aleatoriedade(1, 5) * 1000000); 
        int newEstado = Aleatoriedade(0, 1);
        
        if(newEstado == 0 && estado[4] == 2){
          printf("Kant está pensando!\n");
          if(MUTEX) pthread_mutex_lock(&mutex);
          estado[4] = 0;
          if(MUTEX) pthread_mutex_unlock(&mutex);
        }else{
            PegaGarfo(4);
        }
    }
    pthread_exit(NULL);
}

#if MUTEX
void PegaGarfo(int id)
{
   printf("%s está faminto e tentando comer!\n", names[id]);
   if((estado[LEFT] != 2) && (estado[RIGHT] != 2 ))
   {
      
      printf("%s conseguiu os garfos e está comendo!\n", names[id]);
      pthread_mutex_lock(&mutex);
      estado[id] = 2;
      pthread_mutex_unlock(&mutex);
   }else{
      printf("%s não conseguiu os garfos e continua faminto!\n", names[id]);
      pthread_mutex_lock(&mutex);
      estado[id] = 1;
      pthread_mutex_unlock(&mutex);
   }
}
#else
void PegaGarfo(int id)
{
    printf("%s está comendo!\n", names[id]);
    estado[id] = 2;
}
#endif  

#if !MUTEX 
void *VerificaComendos(void *threadid){
    printf("[CHECK] Thread criada com o TID: %lx\n\n", pthread_self());
    while (true){
        usleep(1000000);
        for (int id = 0; id < 4; id++){
            if(estado[id] == 2 && estado[LEFT] == 2){
                printf("[FAIL]  %s está comendo junto com %s (esquerda).\n", names[id], names[LEFT]);
                printf("\tComo não é possível utilizar o mesmo garfo, o programa será encerrado!");
                exit(0);
            }else if(estado[id] == 2 && estado[RIGHT] == 2){
                printf("[FAIL]  %s está comendo junto com %s (direita).\n", names[id], names[RIGHT]);
                printf("\tComo não é possível utilizar o mesmo garfo, o programa será encerrado!");
                exit(0);
            }
  
        }
        
    }
    pthread_exit(NULL);
}
#endif

int Aleatoriedade(int min, int max){

    return (rand() % ((max + 1) - min) + min);
}
