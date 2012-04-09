#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "queue.h"
#include "ciclista.h"
#include "terreno.h"
#include "threads.h"


queue *estrada;         /* Vetor Compartilhado da Estrada */
pthread_mutex_t estrada_mutex = PTHREAD_MUTEX_INITIALIZER;
int d;                  /* Distância em Km */
int n;                  /* Largura da Pista */

Terreno *terreno;       /* Vetor do comprimento da estrada que indica o "tipo do solo" */

queue *checkpoints;    /* Vetor de filas. Cada elemento da fila é um ciclista que passou pelo CP. */
int tempo;              /* Variável compartilhada do minuto sendo simulado; */
int tempo_numthreads;   /* Variável compartilhada de threads que simularam aquele instante de tempo. */
pthread_mutex_t tempo_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tempo_cond = PTHREAD_COND_INITIALIZER;
int numthreads;
queue listadechegada;  /* Lista de chegada xD */

cleanup_queue cq;

void monta_terreno(int ini, int fim, int checkpoint, Terreno trecho){
    int i;
    for(i = ini; i < fim; i++){
        terreno[i] = trecho;
    }
    if(checkpoint != -1) terreno[checkpoint] = trecho + CP;
}

void leitura_entrada(char *nome_arquivo, int *m, int *n, char *modo_vel,
        int *numcheckpoints) {
  FILE *arq_entrada;
  int k;
    int aux = 0;
    char trecho;
    
    arq_entrada = fopen(nome_arquivo,"r");
    fscanf(arq_entrada,"%d", m);
    fscanf(arq_entrada,"%d", n);
    *modo_vel = 'B';
    while (*modo_vel != 'A' && *modo_vel != 'U') {
        fscanf(arq_entrada,"%c", modo_vel);
        if (feof (arq_entrada)) {
            printf ("Oh oh!\n");
            exit (-2);
        }
    }
    printf ("Modo vel: %c\n", *modo_vel);
    fscanf(arq_entrada,"%d", &d);
    terreno = (Terreno *) malloc(d*sizeof(*terreno));
    if (!terreno) {
        fprintf(stderr, "Erro fazendo malloc na leitura da entrada.\n");
        exit(-1);
    }
    
    while(aux < d){
        trecho = 'E';
        while (trecho != 'P' && trecho != 'S' && trecho != 'D') {
            if (!fscanf(arq_entrada,"%c", &trecho)) {
                printf("Não foi possível ler o bang!\n");
                exit(-2);
            }
        }
        
        fscanf(arq_entrada,"%d", &k);
        printf("trecho: %c - k: %d\n",trecho,k);
        
        switch(trecho){
            case 'P': /* Trecho Plano */
                monta_terreno(aux, aux + k, aux + k/2, PLANO);
                (*numcheckpoints)++;
                break;
            case 'S': /* Trecho Subida */
                monta_terreno(aux, aux + k, -1, SUBIDA);
                break;
            case 'D': /* Trecho Descida */
                monta_terreno(aux, aux + k, aux, DESCIDA);
                (*numcheckpoints)++;
                break;
            default:
                break;
        }
        aux += k;
    }
}

void join_threads() {
  ciclista *curnode;
  printf("joining threads...\n");
  while (numthreads) {
      pthread_mutex_lock(&cq.mutex);
      /* below, we sleep until there really is a new cleanup node.  This
         takes care of any false wakeups... even if we break out of
         pthread_cond_wait(), we don't make any assumptions that the
         condition we were waiting for is true.  */
      while (cq.cleanup.head==NULL) {
          pthread_cond_wait(&cq.cond, &cq.mutex);
      }
      /* at this point, we hold the mutex and there is an item in the
         list that we need to process.  First, we remove the node from
         the queue.  Then, we call pthread_join() on the tid stored in
         the node.  When pthread_join() returns, we have cleaned up
         after a thread.  Only then do we free() the node, decrement the
         number of additional threads we need to wait for and repeat the
         entire process, if necessary */
      curnode = (ciclista *) queue_get(&cq.cleanup);
      pthread_mutex_unlock(&cq.mutex);
      pthread_join(curnode->tid, NULL);
      printf("joined with thread %d\n", curnode->id);
      /*free(curnode);*/
      numthreads--;
      pthread_mutex_lock(&tempo_mutex);
      if (tempo_numthreads == numthreads) {
          printf("Terminou instante de tempo %d\n", tempo);
          tempo++;
          tempo_numthreads = 0;
          pthread_cond_broadcast(&tempo_cond);
      }
      pthread_mutex_unlock(&tempo_mutex);
  }
}

int cleanup_queue_init() {
    if (pthread_mutex_init(&(cq.mutex),NULL))
        return 1;
    if (pthread_cond_init(&(cq.cond),NULL))
        return 1;
    queue_init(&(cq.cleanup));
    return 0;
}

int cleanup_queue_destroy() {
    if (pthread_mutex_destroy(&(cq.mutex)))
        return 1;
    if (pthread_cond_destroy(&(cq.cond)))
        return 1;
    return 0;
}

void imprime_relatorio(int numcheckpoints){
    void *j = queue_get_iterator(&listadechegada);
    int i = 1;
    int k;
    ciclista *c;
    printf("LISTA DE CHEGADA!!!!!\n");
    printf("ID\t Colocacao Chegada\t Pontos Camisa Verde\t Pontos Camisa Vermelha\n");
    while(j){
        c =  (ciclista *) queue_get_iterator_data(j);
        printf("%2d\t %17d\t %19d\t %22d\n", c->id, i, c->pontos_plano, c->pontos_montanha);
        j = queue_iterator_next(j);
        i++;
    }


    for(k = 0; k < numcheckpoints; ++k) {
        printf("\nImprimindo checkpoint %d\n", k);
        j = queue_get_iterator(&checkpoints[k]);
        i = 1;
        while(j) {
            c =  (ciclista *) queue_get_iterator_data(j);
            printf("%5d lugar. - ID: %d\n",i,c->id);
            j = queue_iterator_next(j);
            i++;
        }
    }
}

int main(int argc, char* argv[]){
    int m;              /* Número de ciclistas */
    char modo_vel;      /* Modo de Criação da Velocidade:  'A' - Aleatório / 'U' - Uniforme  */
    int i;
    unsigned int iseed = (unsigned int) time(NULL);
    srand (iseed);
    int numciclistas = 0;
    
    int numcheckpoints = 0;
    
    if (argc <= 1) {
        fprintf(stderr, "Forneca o nome do arquivo de entrada como parametro.\n");
        return 1;
    }
    cleanup_queue_init ();
    leitura_entrada(argv[1], &m, &n, &modo_vel, &numcheckpoints);
    /* Criando o vetor de CP */
    checkpoints = malloc(numcheckpoints*sizeof(*checkpoints));
    for (i = 0; i < numcheckpoints; i++) {
        queue_init(&checkpoints[i]);
    }
    /* Criando o vetor de estrada */
    estrada = malloc(d*sizeof(*estrada));
    for (i = 0; i < d; ++i) {
        queue_init(&estrada[i]);
    }
    /* Criando a fila de chegada */
    queue_init(&listadechegada);

    tempo = 0;
    tempo_numthreads = 0;
    numthreads = m;
    if (cria_ciclistas(m, modo_vel, &numciclistas)) {
        /* Erro ao criar alguma thread */
        numthreads = numciclistas;
        fprintf(stderr, "Error starting threads!\n");
        join_threads();
        cleanup_queue_destroy ();
        return 2;
    }
    numthreads = numciclistas;
    printf ("numthreads: %d\n", numthreads);
    join_threads();
    /* TODO: imprimir relatorio; */
    imprime_relatorio(numcheckpoints);
    cleanup_queue_destroy();
    if(pthread_mutex_destroy(&tempo_mutex))
        printf("Erro destruindo tempo_mutex\n");
    if(pthread_cond_destroy(&tempo_cond))
        printf("Erro destruindo tempo_cond\n");
        
    return 0;
}
