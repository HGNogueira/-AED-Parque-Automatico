#ifndef __graphL__h
#define __graphL__h 1
#include"LinkedList.h"
#include<stdio.h>
#include"prioQ.h"
#include"parkmap.h"

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

void GactivateNode(GraphL *g, int v);
void GdeactivateNode(GraphL *g, int v);
int GisNodeActive(GraphL *g, int v);


/*
 *  Function:
 *    GDijkstra
 *
 *  Description:
 *    calculates ideal path in a Graph from origin (root) node towards 
 *    destiny node
 *
 *  Arguments:
 *    Map *parkMap - configuration map
 *    int root - node to start from
 *    int dest - node to arrive at
 *    int *st  - previously initialized and loaded path tree
 *            At first usage all indexes would be set to -1 
 *
 *    int *wt  - previously initialized and loaded weight tree
 *          At first usage all indexes but the origin would be set
 *          to NOCON value (No connection), the origin would be set to 0
 *
 *    PrioQ *PQ - previously initialized priority queue
 *          Note: after first updating the origin's weight one must update
 *          the priority queue:
 *              example:
 *                  PQ = PQinit(wt, N);
 *                  wt[origin] = 0;
 *                  PQupdate(PQ, origin);
 *
 *  Return value:
 *    total cost of calculated path 
 */

int GDijkstra(GraphL *g, int root, int dest, int* st, int *wt, PrioQ *PQ, Map *parkMap);


void GfreeEdge(Item e);
void Gdestroy(GraphL *g);

#endif
