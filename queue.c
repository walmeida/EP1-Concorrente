#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void queue_init(queue *root) {
    root->head = NULL;
    root->tail = NULL;
    root->size = 0;
}

/* Libera memoria consumida pela queue.
 * Nao libera memoria dos itens (data) inseridos
 */
void queue_destroy(queue *root) {
    if(!root)
        return;
    node *mynode, *prevnode;
    mynode = root->head; 
    prevnode = NULL;
    while (mynode) {
        prevnode = mynode;
        mynode = mynode->next;
        free(prevnode);
    }
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

void queue_remove(queue *root, Item *data) {
    if (!root)
        return;
    node *mynode = root->head;
    node *prev_node = NULL;
    while (mynode != NULL) {
        if (mynode->data == data) {
            if (prev_node) {
                prev_node->next = mynode->next;
            } else {
                root->head = mynode->next;
            }
            if (root->tail == mynode) {
                root->tail = prev_node;
            }
            free(mynode);
            root->size--;
            break;
        }
        prev_node = mynode;
        mynode = mynode->next;
    }
}

int queue_size(queue *root) {
    if (root)
        return root->size;
    return -1;
}

void *queue_get_iterator(queue *root) {
    if (!root)
        return NULL;
    return root->head;
}

Item * queue_get_iterator_data (void * iterator) {
    node *it = (node *) iterator;
    if (!it)
        return NULL;
    return it->data;
}

void *queue_iterator_next(void *iterator) {
    node *it = (node *) iterator;
    if (!it)
        return NULL;
    return it->next;
}
