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
#define NOCON 999999

typedef struct _edge Edge;
typedef struct _graphL GraphL;

GraphL *Ginit(int nodes);

void GinsertEdge(GraphL *g, int v, int w, int value);
void GdeleteEdge(GraphL *g, int v, int w);

void Gprint(FILE *fp,GraphL *g);

LinkedList *GedgesOfNode(GraphL *, int);

int Gnodes(GraphL *g);
int GorigOfEdge(Edge *e);
int GdestOfEdge(Edge *e);
int GvalOfEdge(Edge *e);

int *GDijkstra(GraphL *g, int *cost, int root, int dest);

void GfreeEdge(Item e);
void Gdestroy(GraphL *g);

#endif
