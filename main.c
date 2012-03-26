#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *estrada;			// Vetor Compartilhado da Estrada
int d; 					  // Distância em Km

typedef struct biker {
	int id;				      // Identificador do Ciclista
	pthread_t tid;      // Identificador da thread
	int vel_plano; 		  // Velocidade do Ciclista em trechos Planos em Km/h
	int vel_sub; 		    // Velocidade do Ciclista em trechos de Subida em Km/h
	int vel_desc; 		  // Velocidade do Ciclista em trechos de Descida em Km/h
	int pontos_plano;   // Pontuação do Ciclista para trechos Planos
	int pontos_montanha;// Pontuação do Ciclista para trechos de Montanha
} ciclista;

void leitura_entrada(char *nome_arquivo, int *m, int *n, char *modo_vel){
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

void inicializa_vel(ciclista * c) {
  c->vel_plano = 50;
  c->vel_sub = 50;
  c->vel_desc = 50;
}

void cria_ciclistas (int m) {
  int i;
  ciclista * cicl;
  for (i = 0; i < m; ++i) {
    cicl = (ciclista *) malloc (sizeof (*cicl));
    cicl->id = i;
    cicl->pontos_plano = cicl->pontos_montanha = 0;
    inicializa_vel(cicl);
  }
}

int main(int argc, char* argv[]){
	int m; 				    // Número de ciclistas
	int n; 				    // Largura da Pista
	char modo_vel;		// Modo de Criação da Velocidade:  'A' - Aleatório / 'U' - Uniforme 
	
	leitura_entrada(argv[1],&m,&n,&modo_vel);
		
	return 0;
}
