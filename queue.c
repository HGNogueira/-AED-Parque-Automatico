#include"queue.h"
#include"defs.h"
#include<stdlib.h>
#include<stdio.h>

/* 
 * Internal Data Type: Node
 *
 * Description:
 *      Represents a single element of a Queue Data structure. Contains the
 *  Item and a connection to the next member of the queue.
 */

typedef struct _node{
    Item t;
    struct _node *next;
} Node;


/*
 * Data Type: Queue
 *
 * Description:
 *      Contains a pointer to the first and last nodes in a queue, these will
 * be linked in a ordered way so no element is lost
 */

struct _queue{
    Node *first, *last;
};


/*
 *  Function:
 *    Qinit
 *
 *  Description:
 *    Initializes Queue data structure
 *
 *  Arguments:
 *      void
 *
 *  Return value:
 *      Queue *
 *
 *  Side-effects:
 *      New memory is allocated for Queue structure
 */


Queue *Qinit(){
    Queue *Q;
    Q = (Queue *) malloc(sizeof(Queue));

    Q->first = NULL;
    Q->last = NULL;
    return Q;
}


/*
 *  Function:
 *    Qpush
 *
 *  Description:
 *    Pushes into queue data structure a new Item with the lowest priority - 
 *    - last out
 *
 *  Arguments:
 *    Queue *Q - Queue with data elements
 *    Item t - item to enter Queue
 *
 *  Return value:
 *    void
 *
 *  Side-effects:
 *    New item added to queue as last member
 */

void *Qpush(Queue *Q, Item t){
    Node *node;

    node = (Node *) malloc(sizeof(Node));
    node->t = t;
    node->next = NULL;

    if(Q->first == NULL)
        Q->first = node;
    else if(Q->last == NULL){
        Q->last = node;
        Q->first->next = Q->last;      /* if first node, points to self */
    }
    if(Q->last != NULL){
        Q->last->next = node;
        Q->last = node;
    }

    return;
}


/*
 *  Function:
 *    Qpop
 *
 *  Description:
 *    Pops out the highest priority Item aka first in Item
 *
 *  Arguments:
 *    Queue *Q - Queue with data elements
 *
 *  Return value:
 *    Item
 *
 *  Side-effects:
 *    Queue order is changed
 */

Item Qpop(Queue *Q){
    Node *node;
    Item t;
    if(Q->first == NULL)
        return NULL;

    node = Q->first;
    if(node->next != NULL)
        Q->first = node->next;
    t = node->t;

    free(node);
    return t;
}


/*
 *  Function:
 *    Qdestroy
 *
 *  Description:
 *    Frees the memory allocated to a EMPTY Queue
 *    Warning! Non-empty queues will result in memory loss
 *    client is advised to first empty the Queue and free Item memory
 *
 *  Arguments:
 *    Queue *Q - Queue to deallocate memory
 *
 *  Return value:
 *    None
 *
 *  Side-effects:
 *    Queue *Q given in argument is no longer existant
 */

void Qdestroy(Queue *Q){
    if(Q->first != NULL){
        fprintf(stderr, "Queue isn't empty, can't destroy\n");
        return;
    }
    free(Q);
}
