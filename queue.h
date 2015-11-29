/*
 *  File name: queue.h
 *
 *  Author: Beatriz Ferreira & Henrique Nogueira
 *
 *  Description: Implements the queue data structure applying to any type of
 *          ADT;
 *               
 *  Abstract Data Type: Queue
 *              Contains Data elements to implement firt-in-first queue type
 *
 *  Dependencies:
 *    stdlib.h
 *    defs.h
 *
 */


#ifndef __queue__h
#define __queue__h 1
#include"defs.h"

typedef struct _queue Queue;

Queue *Qinit();
void *Qpush(Queue *, Item);
Item Qpop(Queue *);


#endif
