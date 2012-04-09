#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void queue_init(queue *root) {
    root->head = NULL;
    root->tail = NULL;
    root->size = 0;
}

/* put on tail */
void queue_put(queue *root, Item *data) {
    node *mynode;
    if (!root)
        return;
    mynode = (node *) malloc(sizeof(*mynode));
    if (!mynode) {
        fprintf(stderr, "Erro fazendo malloc.\n");
        return;
    }
    mynode->next = NULL;
    mynode->data = data;
    if (root->tail != NULL)
        root->tail->next = mynode;
    root->tail = mynode;
    if (root->head == NULL)
        root->head = mynode;
    root->size++;
}

/* get from head */
Item *queue_get(queue *root) {
    if (!root)
        return NULL;
    node *mynode;
    mynode = root->head;
    if (root->head != NULL)
        root->head = root->head->next;
    if (root->head == NULL)
        root->tail = NULL;
    Item *d = mynode->data;
    free(mynode);
    root->size--;
    return d;
}

int queue_size(queue *root) {
    if (root)
        return root->size;
    return -1;
}
