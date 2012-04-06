#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "queue.h"
#include "ciclista.h"
#include "terreno.h"


int *estrada;           // Vetor Compartilhado da Estrada
int d;                  // Distância em Km

Terreno *terreno;       // Vetor do comprimento da estrada que indica o "tipo do solo"

queue * checkpoints;    // Vetor de filas. Cada elemento da fila é um ciclista que passou pelo CP.
int numtrechos;         // Serve para indicar o tamanho do vetor de checkpoints

struct cleanup_queue {
  //data_control control; // TODO: trocar por coisas de pthread (mutex)
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
    
    while(aux < d){
        trecho = 'E';
        while (trecho != 'P' && trecho != 'S' && trecho != 'D') {
            if (!fscanf(arq_entrada,"%c", &trecho)) {
                printf("Não foi possível ler o bang!\n");
                exit(-1);
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
        numtrechos++;
    }
    //Checkpoint final
    terreno[d-1] = terreno[d-1] + CP;
}


int main(int argc, char* argv[]){
    int m;              // Número de ciclistas
    int n;              // Largura da Pista
    char modo_vel;      // Modo de Criação da Velocidade:  'A' - Aleatório / 'U' - Uniforme 
    int numthreads = 0;
    int i;
    unsigned int iseed = (unsigned int)time(NULL);
    srand (iseed);
    
    numtrechos = 0;
    
    leitura_entrada(argv[1],&m,&n,&modo_vel);
    // Criando o vetor de CP
      checkpoints = malloc(numtrechos*sizeof(*checkpoints));
      for(i = 0; i < numtrechos; i++){
        queue_init(checkpoints[i]);
      }
    //
    
    if (cria_ciclistas(m, modo_vel, &numthreads)) {
        // Erro ao criar alguma thread
        fprintf(stderr, "Error starting threads!\n");
        // TODO: Join com threads restantes
    }
    // TODO: simulacao e join com threads finalizadas
        
    return 0;
}
