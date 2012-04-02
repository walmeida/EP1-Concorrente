#ifndef QUEUE_H_
#define QUEUE_H_

#include "ciclista.h"
// TODO: criar arquivo ciclista.h com
// struct do ciclista
typedef ciclista Item;

typedef struct node {
  struct node *next;
  Item *data;
} node;

typedef struct queue {
  node *head, *tail; 
} queue;

void queue_init(queue *myroot);
void queue_put(queue *myroot, node *mynode);
Item *queue_get(queue *myroot);

#endif
