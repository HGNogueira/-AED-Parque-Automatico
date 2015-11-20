#ifndef __graphL__h
#define __graphL__h 1
#ifndef LinkedListHeader
#include"LinkedList.h"
#include<stdio.h>
#endif


typedef struct _edge Edge;
typedef struct _graphL GraphL;

GraphL *graphInit(int nodes);

void insertEdge(GraphL *g, int v, int w, int value);
void deleteEdge(GraphL *g, int v, int w);

void GprintToFile(FILE *fp,GraphL *g);

LinkedList *getEdgesOfNode(GraphL *, int);
int getOrigNode(Edge *e);
int getDestNode(Edge *e);
int getValueEdge(Edge *e);

void freeEdge(Item e);
void destroyGraph(GraphL *g);

#endif
