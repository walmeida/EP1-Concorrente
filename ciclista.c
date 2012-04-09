#include "ciclista.h"
#include <stdio.h>
#include <stdlib.h>
#include "threads.h"
#include "queue.h"

extern cleanup_queue cq;
extern queue *estrada;
extern int numthreads;
extern queue listadechegada;
extern queue *checkpoints;

/* Devolve a nova posicao do ciclista apos um minuto.
 * Não verifica se eh possivel chegar nesta posicao,
 * ou seja, nao verifica se ha espaco para o ciclista.
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
        if (tempo_trecho + tempo_simulado > 1) {
            posicao_atual += (1 - tempo_simulado) * vel / 60;
            break;
        }
        posicao_atual = 1 + (int) posicao_atual;
    }
    if (posicao_atual - cicl->posicao_estrada < 0.000001)
        posicao_atual += 0.000001;
    return posicao_atual;
}

/* Devolve a pontuacao de um checkpoint de acordo com a colocacao */
static int pontuacao(int colocacao) {
    int pontos;
    switch(colocacao) {
        case 1:
            pontos = 45;
            break;
        case 2:
            pontos = 35;
            break;
        case 3:
            pontos = 25;
            break;
        case 4:
            pontos = 15;
            break;
        case 5:
            pontos = 10;
            break;
        case 6:
            pontos = 5;
            break;
        default:
            pontos = 0;
            break;
    }
    return pontos;
}

/* Adiciona no ciclista a pontuacao de um checkpoint em um terreno de tipo "t", 
 * quando o ciclista "c" passa por ele nesta "colocacao"
 */
void adiciona_pontuacao(ciclista *c, Terreno t, int colocacao) {
    switch(t) {
        case PLANOCP:
            c->pontos_plano += pontuacao(colocacao);
            break;
        case DESCIDACP:
            c->pontos_montanha += pontuacao(colocacao);
            break;
    }
}

/* Funcao que simula um ciclista */
void *thread_ciclista(void *arg) {
    ciclista *cicl = (ciclista *) arg;
    int indice_checkpoint = 0;
    int colocacao;
    while (cicl->posicao_estrada < d) {
        double prox_posicao = calcula_proxima_posicao (cicl);
        int km_atual = (int) cicl->posicao_estrada;
        if (cicl->posicao_estrada < 0)
            km_atual = -1;
        /* secao critica da estrada */
        pthread_mutex_lock(&estrada_mutex);
        do {
            if (km_atual + 1 > prox_posicao) {
                /* Continua dentro do mesmo kilometro */
                cicl->posicao_estrada = prox_posicao;
                break;
            } else {
                if (km_atual + 1 == d) {
                    /* Terminou a corrida */
                    queue_remove(&estrada[km_atual], cicl);
                    cicl->posicao_estrada = d;
                    printf("ID: %d - Terminou a corrida\n", cicl->id);
                    queue_put(&listadechegada, cicl);
                    break;
                }
                if (queue_size(&estrada[km_atual+1]) >= n) {
                    /* Nao eh possivel ir para o proximo kilometro por falta de espaco */
                    cicl->posicao_estrada = km_atual + 0.9999999;
                    break;
                } else {
                    /* Andando para o proximo kilometro */
                    if (km_atual >= 0)
                        queue_remove(&estrada[km_atual], cicl);
                    km_atual++;
                    if (km_atual < d)
                        queue_put(&estrada[km_atual], cicl);
                    if (km_atual >= 0 && (terreno[km_atual] & CP)) {
                        queue_put(&checkpoints[indice_checkpoint], cicl);
                        colocacao = queue_size(&checkpoints[indice_checkpoint]);
                        if (colocacao <= 3) {
                            printf("ID: %d - Passando por Checkpoint no Km %d (%d lugar)\n",
                                    cicl->id, km_atual, colocacao);
                        }
                        adiciona_pontuacao(cicl, terreno[km_atual], colocacao);
                        indice_checkpoint++;
                    }
                }
            }
        } while (prox_posicao > cicl->posicao_estrada);
        pthread_mutex_unlock(&estrada_mutex);
        printf("ID: %d - Esta na posicao %.9lf\n", cicl->id, cicl->posicao_estrada);
        /* fim secao critica estrada */
        if (cicl->posicao_estrada >= d)
            break;
        /* secao critica do tempo */
        pthread_mutex_lock(&tempo_mutex);
        tempo_numthreads++;
        if (tempo_numthreads == numthreads) {
            printf("Terminou instante de tempo %d\n\n", tempo);
            tempo++;
            tempo_numthreads = 0;
            pthread_cond_broadcast(&tempo_cond);
        } else {
            /* Espera todas as threads simularem este minuto */
            pthread_cond_wait(&tempo_cond, &tempo_mutex);
        }
        pthread_mutex_unlock(&tempo_mutex);
        /* fim secao critica do tempo */
    }
    pthread_mutex_lock(&cq.mutex);
    queue_put(&cq.cleanup, cicl);
    pthread_mutex_unlock(&cq.mutex);
    pthread_cond_signal(&cq.cond);
    
    return NULL;
}    

/* Sorteia uma velocidade aleatoria entre 20 e 80 Km/h */
static int sorteia_velocidade() {
   int velocidade;
   velocidade = 20 + (int)( 60.0 * rand() / ( RAND_MAX + 1.0 ) );
   return velocidade;
}

/* Inicializa as velocidades de um ciclista
 * Se modo_vel == 'A', cria 3 velocidades aleatorias no intervalo [20,80)
 * Caso contrario, as 3 velocidades sao 50
 */
static void inicializa_vel(ciclista *c, char modo_vel) {
    if(modo_vel == 'A'){
        c->vel_plano = sorteia_velocidade();
        c->vel_sub = sorteia_velocidade();
        c->vel_desc = sorteia_velocidade();  
    }
    else{
        c->vel_plano = c->vel_sub = c->vel_desc = 50;
    }
}

/* Imprime as velocidades de um ciclista */
void imprime_ciclista(ciclista *cicl) {
    printf("ID: %5d\n", cicl->id);
    printf("Velocidade no Plano:   %2d\n", cicl->vel_plano);
    printf("Velocidade na Subida:  %2d\n", cicl->vel_sub);
    printf("Velocidade na Descida: %2d\n\n", cicl->vel_desc);
}

/* Cria ciclistas (threads) e imprime suas velocidades */
int cria_ciclistas(int m, char modo_vel, int * numciclistas) {
    int i;
    ciclista * cicl;
    *numciclistas = 0;
    queue ciclistas;
    queue_init(&ciclistas);
    void *j;

    printf("LISTA DE CICLISTAS\n\n");
    for (i = 0; i < m; ++i) {
        cicl = (ciclista *) malloc (sizeof (*cicl));
        cicl->id = i;
        cicl->pontos_plano = cicl->pontos_montanha = 0;
        cicl->posicao_estrada = -0.0000001;
        inicializa_vel(cicl, modo_vel);
        queue_put(&ciclistas, cicl);
        imprime_ciclista(cicl);
    }

    j = queue_get_iterator(&ciclistas);
    while(j) {
        cicl = (ciclista *) queue_get_iterator_data(j);
        if (pthread_create(&(cicl->tid), NULL, (void *) thread_ciclista, (void *) cicl))
            return 1;
        (*numciclistas)++;
        j = queue_iterator_next(j);
    }

    queue_destroy(&ciclistas);
    return 0;
}
