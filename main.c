#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *estrada;           // Vetor Compartilhado da Estrada
int d;                  // Distância em Km

struct cleanup_queue {
  data_control control; // TODO: trocar por coisas de pthread (mutex)
  queue cleanup;
} cq;

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
        switch(trecho){
            case 'P': // Trecho Plano
                break;
            case 'S': // Trecho Subida
                break;
            case 'D': // Trecho Descida
                break;
            default:
                break;
        }
        fscanf(arq_entrada,"%d", &k);
        printf("trecho: %c - k: %d\n",trecho,k);
        aux += k;
    }
}


int main(int argc, char* argv[]){
    int m;              // Número de ciclistas
    int n;              // Largura da Pista
    char modo_vel;      // Modo de Criação da Velocidade:  'A' - Aleatório / 'U' - Uniforme 
    int numthreads = 0;
    
    leitura_entrada(argv[1],&m,&n,&modo_vel);
    if (cria_ciclistas(m, modo_vel, &numthreads)) {
        // Erro ao criar alguma thread
        fprintf(stderr, "Error starting threads!\n");
        // TODO: Join com threads restantes
    }
    // TODO: simulacao e join com threads finalizadas
        
    return 0;
}
