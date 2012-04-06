#ifndef QUEUE_H_
#define QUEUE_H_

#include "ciclista.h"
typedef ciclista Item;

// FIFO queue

typedef struct node {
    struct node *next;
    Item *data;
} node;

typedef struct queue {
    node *head, *tail; 
    int size;
} queue;

void queue_init(queue *root);
void queue_put(queue *root, Item *data);
Item *queue_get(queue *root);
int queue_size(queue *root);

#endif
