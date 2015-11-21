#ifndef __graphL__h
#define __graphL__h 1
#include"LinkedList.h"
#include<stdio.h>

/* this value will indicate that there is no connection between
 * current tree and a node while performing the algorithm 
 * 
 * Warning! The client may want to change this value according to preference
 * in the application
 */
#define NOCON -1

typedef struct _edge Edge;
typedef struct _graphL GraphL;

GraphL *graphInit(int nodes);

void insertEdge(GraphL *g, int v, int w, int value);
void deleteEdge(GraphL *g, int v, int w);

void GprintToFile(FILE *fp,GraphL *g);

LinkedList *getEdgesOfNode(GraphL *, int);

int getNodes(GraphL *g);
int getOrigNode(Edge *e);
int getDestNode(Edge *e);
int getValueEdge(Edge *e);

int *GLDrijkstra(GraphL *g, int root, int dest);

void freeEdge(Item e);
void destroyGraph(GraphL *g);

#endif
