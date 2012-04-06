#include "ciclista.h"
#include <stdio.h>
#include <stdlib.h>

void *thread_ciclista(void *arg) {
    ciclista *cicl = (ciclista *) arg;
    // TODO: simulacao do ciclista
    printf("Thread %d finished the simulation...\n", cicl->id);
    return NULL;
}    

void inicializa_vel(ciclista *c, char modo_vel) {
  if(modo_vel == 'A'){
    c->vel_plano = sorteia_velocidade();
    c->vel_sub = sorteia_velocidade();
    c->vel_desc = sorteia_velocidade();  
  }
  else{
    c->vel_plano = c->vel_sub = c->vel_desc = 50;
  }
}

int cria_ciclistas(int m, char modo_vel, int * numthreads) {
    int i;
    ciclista * cicl;
    for (i = 0; i < m; ++i) {
        cicl = (ciclista *) malloc (sizeof (*cicl));
        cicl->id = i;
        cicl->pontos_plano = cicl->pontos_montanha = 0;
        inicializa_vel(cicl, modo_vel);
        if (pthread_create(&(cicl->tid), NULL, thread_ciclista, (void *) cicl))
            return 1; //TODO: pensar em um jeito de as threads criadas se colocarem na lista de cleanup
        printf("created thread %d\n",i);
        numthreads++;
    }
    return 0;
}

int sorteia_velocidade(){
   int velocidade;
      
   velocidade = 20 + (int)( 60.0 * rand() / ( RAND_MAX + 1.0 ) );
   
   return velocidade;
    
}
