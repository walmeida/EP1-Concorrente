#ifndef QUEUE_H_
#define QUEUE_H_

#include "ciclista.h"
typedef ciclista Item;

typedef struct node {
  struct node *next;
  Item *data;
} node;

typedef struct queue {
  node *head, *tail; 
} queue;

void queue_init(queue *myroot);
void queue_put(queue *myroot, Item *data);
Item *queue_get(queue *myroot);

#endif
