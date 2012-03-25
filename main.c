#include <stdio.h>

int *estrada;			// Vetor Compartilhado da Estrada
int d; 					// Distância em Km

typedef struct biker{
	int id;				// Identificador
	int vel_pl; 		// Velocidade do Ciclista em trechos Planos em Km/h
	int vel_sub; 		// Velocidade do Ciclista em trechos de Subida em Km/h
	int vel_desc; 		// Velocidade do Ciclista em trechos de Descida em Km/h
}ciclista;

int main(int argc, char* argv[]){
	FILE *arq_entrada;	// Arquivo de Entrada
	int m; 				// Número de ciclistas
	int n; 				// Largura da Pista
	char modo_vel;		// Modo de Criação da Velocidade:  'A' - Aleatório / 'U' - Uniforme 
	
	int aux = 0;
	char trecho;
	
	// Leitura do arquivo de entrada
	arq_entrada = fopen(argv[1],"r");
	fscanf(arq_entrada,"%d", &m);
	fscanf(arq_entrada,"%d", &n);
	fscanf(arq_entrada,"%c", &modo_vel);
	fscanf(arq_entrada,"%d", &d);
	
	while(aux < d){
		fscanf(arq_entrada,"%c", &trecho);
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
	}
	// Fim da Leitura
		
	return 0;
}