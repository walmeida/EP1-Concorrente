#include "ciclista.h"
#include <stdio.h>
#include <stdlib.h>
#include "threads.h"
#include "queue.h"

extern cleanup_queue cq;
extern queue *estrada;
extern int numthreads;

/* Devolve a nova posição do ciclista após um minuto.
 * Não verifica se é possível chegar nesta posição,
 * ou seja, não verifica se há espaço para o ciclista.
 */
static double calcula_proxima_posicao(ciclista * cicl) {
    Terreno terr;
    int vel;
    double tempo_simulado = 0;
    double tempo_trecho;
    double posicao_atual = cicl->posicao_estrada;
    double dist_ate_prox_terreno;
    for (tempo_simulado = 0;
            tempo_simulado < 1; tempo_simulado += tempo_trecho) {
        if (posicao_atual >= d) {
            posicao_atual = d;
            break;
        }
        dist_ate_prox_terreno = 1 + (int) posicao_atual - posicao_atual;
        terr = TIPO_TERR_MASK & terreno[(int) posicao_atual];
        switch (terr) {
            case PLANO:
                vel = cicl->vel_plano;
                break;
            case SUBIDA:
                vel = cicl->vel_sub;
                break;
            case DESCIDA:
                vel = cicl->vel_desc;
                break;
        }
        tempo_trecho = (60 * dist_ate_prox_terreno) / vel;
        printf("ID: %d - dist_prox: %lf, vel: %d, temp_t: %lf, temp_sim: %lf\n",
                cicl->id, dist_ate_prox_terreno, vel, tempo_trecho, tempo_simulado);
        if (tempo_trecho + tempo_simulado > 1) {
            posicao_atual += (1 - tempo_simulado) * vel / 60;
            printf("ID: %d - terminou em posicao %.8lf\n", cicl->id, posicao_atual);
            break;
        }
        printf("ID: %d - posicao_atual: %lf\n", cicl->id, posicao_atual);
        posicao_atual = 1 + (int) posicao_atual;
        printf("ID: %d - posicao_atual atualizada: %lf\n", cicl->id, posicao_atual);
    }
    if (posicao_atual - cicl->posicao_estrada < 0.000001)
        posicao_atual += 0.000001;
    return posicao_atual;
}

/* Função que simula um ciclista */
void *thread_ciclista(void *arg) {
    ciclista *cicl = (ciclista *) arg;
    while (cicl->posicao_estrada < d) {
        double prox_posicao = calcula_proxima_posicao (cicl);
        int km_atual = (int) cicl->posicao_estrada;
        /* seção crítica da estrada */
        pthread_mutex_lock(&estrada_mutex);
        do {
            if (km_atual + 1 > prox_posicao) {
                /* TODO Verificar checkpoints (???) */
                cicl->posicao_estrada = prox_posicao;
                break;
            } else {
                /* TODO Verificar checkpoints */
                if (km_atual + 1 == d) {
                    cicl->posicao_estrada = d;
                    break;
                }
                if (queue_size(&estrada[km_atual+1]) >= n) {
                    cicl->posicao_estrada = km_atual + 0.9999999;
                    break;
                } else {
                    cicl->posicao_estrada = prox_posicao;
                    if (km_atual >= 0)
                        queue_remove(&estrada[km_atual++], cicl);
                    if (km_atual < d)
                        queue_put(&estrada[km_atual], cicl);
                }
            }
        } while (prox_posicao > cicl->posicao_estrada);
        pthread_mutex_unlock(&estrada_mutex);
        /* fim seção crítica estrada */
        /* seção crítica do tempo */
        /* TODO simular tempo */
        pthread_mutex_lock(&tempo_mutex);
        tempo_numthreads++;
        if (tempo_numthreads == numthreads) {
            tempo_numthreads = 0;
            pthread_cond_broadcast(&tempo_cond);
        } else {
            pthread_cond_wait(&tempo_cond, &tempo_mutex);
        }
        pthread_mutex_unlock(&tempo_mutex);
        /* fim seção crítica do tempo */
    }
    printf("Thread %d finished the simulation...\n", cicl->id);
    pthread_mutex_lock(&cq.mutex);
    queue_put(&cq.cleanup, cicl);
    pthread_mutex_unlock(&cq.mutex);
    pthread_cond_signal(&cq.cond);
    
    return NULL;
}    

/* Sorteia uma velocidade aleatória entre 20 e 80 Km/h */
static int sorteia_velocidade() {
   int velocidade;
   velocidade = 20 + (int)( 60.0 * rand() / ( RAND_MAX + 1.0 ) );
   return velocidade;
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
        cicl->posicao_estrada = -0.0000001;
        inicializa_vel(cicl, modo_vel);
        if (pthread_create(&(cicl->tid), NULL, (void *) thread_ciclista, (void *) cicl))
            return 1;
        printf("created thread %d\n",cicl->id);
        (*numthreads)++;
    }
    return 0;
}
