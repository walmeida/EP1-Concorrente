#ifndef CICLISTA_H_
#define CICLISTA_H_

#include <pthread.h>
#include "terreno.h"

extern int tempo;
extern int tempo_numthreads;
extern pthread_mutex_t tempo_mutex;
extern pthread_cond_t tempo_cond;
extern int d;
extern int n;
extern Terreno *terreno;
extern pthread_mutex_t estrada_mutex;

typedef struct biker {
    int id;                 /* Identificador do Ciclista */
    pthread_t tid;          /* Identificador da thread */
    int vel_plano;          /* Velocidade do Ciclista em trechos Planos em Km/h */
    int vel_sub;            /* Velocidade do Ciclista em trechos de Subida em Km/h */
    int vel_desc;           /* Velocidade do Ciclista em trechos de Descida em Km/h */
    int pontos_plano;       /* Pontuação do Ciclista para trechos Planos */
    int pontos_montanha;    /* Pontuação do Ciclista para trechos de Montanha */
    double posicao_estrada; /* Km da estrada em que esta. */
} ciclista;

/* Funcao executada por uma thread ciclista
 * Simula um ciclista
 */
void *thread_ciclista(void *arg);
/* Cria as threads ciclistas e imprime suas velocidades */
int cria_ciclistas(int m, char modo_vel, int * numthreads);

#endif
