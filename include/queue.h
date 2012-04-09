#ifndef QUEUE_H_
#define QUEUE_H_

#include "ciclista.h"
typedef ciclista Item;

/* FIFO queue */

typedef struct node {
    struct node *next;
    Item *data;
} node;

typedef struct queue {
    node *head, *tail; 
    int size;
} queue;

/* Inicializa uma queue */
void queue_init(queue *root);
/* Libera memoria consumida pela queue.
 * Nao libera memoria dos itens (data) inseridos
 */
void queue_destroy(queue *root);
/* Insere um item no final da queue */
void queue_put(queue *root, Item *data);
/* Retorna um item do comeco da queue */
Item *queue_get(queue *root);
/* Remove um item da queue */
void queue_remove(queue *root, Item *data);
/* Retorna o numero de elementos na queue */
int queue_size(queue *root);
/* Retorna um iterador para os elementos da queue */
void *queue_get_iterator(queue *root);
/* Recebe um iterador para a queue e devolove o item desta posicao */
Item * queue_get_iterator_data (void * iterator);
/* Retorna um iterador para a proxima posicao da queue */
void *queue_iterator_next(void *iterator);

#endif
