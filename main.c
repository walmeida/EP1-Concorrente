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

/* Struct para ordenar ciclistas por pontuaco */
typedef struct cicl_colocacao {
    int id;
    int pontos;
} cicl_colocacao;

cleanup_queue cq;

int compara_pontos_ciclistas(const void * a, const void * b) {
    cicl_colocacao *ca = (cicl_colocacao *) a;
    cicl_colocacao *cb = (cicl_colocacao *) b;
    if (ca->pontos == cb->pontos) {
        return (ca->id - cb->id);
    }
    return cb->pontos - ca->pontos;
}

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
            printf ("Erro no arquivo de entrada!\n");
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
                printf("Erro na leitura da entrada!\n");
                exit(-2);
            }
        }
        
        fscanf(arq_entrada,"%d", &k);
        
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
    fclose(arq_entrada);
}

void join_threads() {
  ciclista *curnode;
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
         after a thread.  Only then do we decrement the
         number of additional threads we need to wait for and repeat the
         entire process, if necessary */
      curnode = (ciclista *) queue_get(&cq.cleanup);
      pthread_mutex_unlock(&cq.mutex);
      pthread_join(curnode->tid, NULL);
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

void imprime_relatorio(int numciclistas, int numcheckpoints){
    void *j;
    int i;
    int k;
    ciclista *c;
    cicl_colocacao *colocacao = malloc(numciclistas*sizeof(*colocacao));
    if (!colocacao) {
        printf("Erro alocando memoria para colocacao final\n");
        return;
    }

    printf("\nCOLOCACOES NOS CHECKPOINTS\n");
    for (k = 0; k < numcheckpoints; ++k) {
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

    printf("\nLISTA DE CHEGADA\n");
    printf("ID\t Colocacao\n");
    i = 1;
    j = queue_get_iterator(&listadechegada);
    while(j){
        c =  (ciclista *) queue_get_iterator_data(j);
        printf("%2d\t %9d\n", c->id, i);
        j = queue_iterator_next(j);
        colocacao[i-1].id = c->id;
        colocacao[i-1].pontos = c->pontos_plano;
        i++;
    }

    qsort(colocacao, numciclistas, sizeof(*colocacao), compara_pontos_ciclistas);
    printf("\nLISTA DE PONTOS DA CAMISA VERDE (etapas planas)\n");
    printf("ID\t Pontos\n");
    for (i = 0; i < numciclistas; ++i) {
        printf("%2d\t %6d\n", colocacao[i].id, colocacao[i].pontos);
    }

    i = 1;
    j = queue_get_iterator(&listadechegada);
    while(j){
        c =  (ciclista *) queue_get_iterator_data(j);
        j = queue_iterator_next(j);
        colocacao[i-1].id = c->id;
        colocacao[i-1].pontos = c->pontos_montanha;
        i++;
    }

    qsort(colocacao, numciclistas, sizeof(*colocacao), compara_pontos_ciclistas);
    printf("\nLISTA DE PONTOS DA CAMISA BRANCA COM BOLAS VERMELHAS (etapas de montanha)\n");
    printf("ID\t Pontos\n");
    for (i = 0; i < numciclistas; ++i) {
        printf("%2d\t %6d\n", colocacao[i].id, colocacao[i].pontos);
    }

    free(colocacao);
}

void libera_memoria(int numcheckpoints) {
    ciclista * cicl;
    int i;
    void *j = queue_get_iterator(&listadechegada);
    while(j) {
        cicl = (ciclista *) queue_get_iterator_data(j);
        free(cicl);
        j = queue_iterator_next(j);
    }
    queue_destroy(&listadechegada);
    for(i = 0; i < numcheckpoints; ++i) {
        queue_destroy(&checkpoints[i]);
    }
    free(checkpoints);
    free(terreno);
    free(estrada);
}

int main(int argc, char* argv[]){
    int m;              /* Numero de ciclistas */
    char modo_vel;      /* Modo de Criacao da Velocidade:  'A' - Aleatorio / 'U' - Uniforme  */
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
    join_threads();
    /* TODO: imprimir relatorio; */
    imprime_relatorio(m, numcheckpoints);
    libera_memoria(numcheckpoints);
    cleanup_queue_destroy();
    if(pthread_mutex_destroy(&tempo_mutex))
        printf("Erro destruindo tempo_mutex\n");
    if(pthread_cond_destroy(&tempo_cond))
        printf("Erro destruindo tempo_cond\n");
        
    return 0;
}
