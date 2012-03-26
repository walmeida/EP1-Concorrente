#include <stdio.h>
#include <stdlib.h>

int *estrada;			// Vetor Compartilhado da Estrada
int d; 					// Distância em Km

typedef struct biker{
	int id;				// Identificador
	int vel_plano; 		// Velocidade do Ciclista em trechos Planos em Km/h
	int vel_sub; 		// Velocidade do Ciclista em trechos de Subida em Km/h
	int vel_desc; 		// Velocidade do Ciclista em trechos de Descida em Km/h
}ciclista;

int main(int argc, char* argv[]){
	FILE *arq_entrada;	// Arquivo de Entrada
	int m; 				// Número de ciclistas
	int n; 				// Largura da Pista
	char modo_vel;		// Modo de Criação da Velocidade:  'A' - Aleatório / 'U' - Uniforme 
	
	int k;
	int aux = 0;
	char trecho;
	
	// Leitura do arquivo de entrada
	arq_entrada = fopen(argv[1],"r");
	fscanf(arq_entrada,"%d", &m);
	fscanf(arq_entrada,"%d", &n);
	modo_vel = 'B';
	while (modo_vel != 'A' && modo_vel != 'U') {
		fscanf(arq_entrada,"%c", &modo_vel);
		printf ("%c", modo_vel);
		if (feof (arq_entrada)) {
			printf ("Oh oh!\n");
			exit (-2);
		}
	}
	fscanf(arq_entrada,"%d", &d);
	
	printf("Oi estou vivo %d %d %c %d\n",m,n,modo_vel,d);
	
	while(aux < d){
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
	// Fim da Leitura
		
	return 0;
}