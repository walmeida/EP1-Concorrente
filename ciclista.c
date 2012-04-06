#include "ciclista.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "threads.h"

extern cleanup_queue cq;

void *thread_ciclista(void *arg) {
    ciclista *cicl = (ciclista *) arg;
    // TODO: simulacao do ciclista

    printf("Thread %d finished the simulation...\n", cicl->id);
    pthread_mutex_lock(&cq.mutex);
    queue_put(&cq.cleanup, cicl);
    pthread_mutex_unlock(&cq.mutex);
    pthread_cond_signal(&cq.cond);
    
    return NULL;
}    

void inicializa_vel(ciclista *c, char modo_vel) {
    c->vel_plano = 50;
    c->vel_sub = 50;
    c->vel_desc = 50;
}

int cria_ciclistas(int m, char modo_vel, int * numthreads) {
    int i;
    ciclista * cicl;
    for (i = 0; i < m; ++i) {
        cicl = (ciclista *) malloc (sizeof (*cicl));
        cicl->id = i;
        cicl->pontos_plano = cicl->pontos_montanha = 0;
        inicializa_vel(cicl, modo_vel);
        if (pthread_create(&(cicl->tid), NULL, (void *) thread_ciclista, (void *) cicl))
            return 1; //TODO: pensar em um jeito de as threads criadas se colocarem na lista de cleanup
        printf("created thread %d\n",cicl->id);
        (*numthreads)++;
    }
    return 0;
}
