#ifndef CICLISTA_H_
#define CICLISTA_H_

#include <pthread.h>

typedef struct biker {
    int id;             // Identificador do Ciclista
    pthread_t tid;      // Identificador da thread
    int vel_plano;      // Velocidade do Ciclista em trechos Planos em Km/h
    int vel_sub;        // Velocidade do Ciclista em trechos de Subida em Km/h
    int vel_desc;       // Velocidade do Ciclista em trechos de Descida em Km/h
    int pontos_plano;   // Pontuação do Ciclista para trechos Planos
    int pontos_montanha;// Pontuação do Ciclista para trechos de Montanha
} ciclista;

void *thread_ciclista(void *arg);
void inicializa_vel(ciclista *c, char modo_vel);
int cria_ciclistas(int m, char modo_vel, int * numthreads);

#endif
