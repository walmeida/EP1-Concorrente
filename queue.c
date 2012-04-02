#include <stdio.h>
#include "queue.h"

void queue_init(queue *myroot) {
    myroot->head = NULL;
    myroot->tail = NULL;
}

void queue_put(queue *myroot, Item *mynode) {
    node *mynode = (node *) malloc(sizeof(*mynode));
    if (!mynode) {
        //TODO
    }
    mynode->next = NULL;
    if (myroot->tail != NULL)
        myroot->tail->next = mynode;
    myroot->tail = mynode;
    if (myroot->head == NULL)
        myroot->head = mynode;
}

// get from head
node *queue_get(queue *myroot) {
    node *mynode;
    mynode=myroot->head;
    if (myroot->head != NULL)
        myroot->head = myroot->head->next;
    if (myroot->head == NULL)
        myroot->tail = NULL;
    return mynode;
}
