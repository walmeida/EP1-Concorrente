#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "queue.h"
#include "ciclista.h"
#include "terreno.h"

int *estrada;           // Vetor Compartilhado da Estrada
int d;                  // Distância em Km

Terreno *terreno;       // Vetor do comprimento da estrada que indica o "tipo do solo"

struct cleanup_queue {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  queue cleanup;
} cq;

void monta_terreno(int ini, int fim, int checkpoint, Terreno trecho){
  int i;
  for(i = ini; i < fim; i++){
    terreno[i] = trecho;
  }
  if(checkpoint != -1) terreno[checkpoint] = trecho + CP;
}

void leitura_entrada(char *nome_arquivo, int *m, int *n, char *modo_vel) {
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
        printf ("%c", *modo_vel);
        if (feof (arq_entrada)) {
            printf ("Oh oh!\n");
            exit (-2);
        }
    }
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
            case 'P': // Trecho Plano
                monta_terreno(aux, aux + k, (aux + aux + k)/2, PLANO);
                break;
            case 'S': // Trecho Subida
                monta_terreno(aux, aux + k, aux + k, SUBIDA);
                break;
            case 'D': // Trecho Descida
                monta_terreno(aux, aux + k, -1, DESCIDA);
                break;
            default:
                break;
        }
        aux += k;
    }
    //Checkpoint final
    terreno[d-1] = terreno[d-1] + CP;
}

void join_threads(int numthreads) {
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
      free(curnode);
      numthreads--;
  }
}

int main(int argc, char* argv[]){
    int m;              // Número de ciclistas
    int n;              // Largura da Pista
    char modo_vel;      // Modo de Criação da Velocidade:  'A' - Aleatório / 'U' - Uniforme 
    int numthreads = 0;
    
    if (argc <= 1) {
        fprintf(stderr, "Forneca o nome do arquivo de entrada como parametro.\n");
        return 1;
    }
    leitura_entrada(argv[1],&m,&n,&modo_vel);
    if (cria_ciclistas(m, modo_vel, &numthreads)) {
        // Erro ao criar alguma thread
        fprintf(stderr, "Error starting threads!\n");
        join_threads(numthreads);
    }
    // TODO: simulacao
    join_threads(numthreads);
        
    return 0;
}
