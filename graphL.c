#include"graphL.h"
#include"LinkedList.h"
#include<stdio.h>
#include<stdlib.h>
#include"defs.h"
#include"prioQ.h"

struct _edge{
    int v;
    int w;
    int value;
};

struct _graphL{
    int nodes;
    LinkedList **adjL;
};

void freeEdge(Item e) {
    free((Edge *) e);
    return;
}


GraphL *graphInit(int nodes) {
    int i = 0;
    GraphL *g;

    g = (GraphL *) malloc(sizeof(GraphL));
    if(g == NULL){
        fprintf(stderr, "Memory error\n");
        return NULL;
    }

    g->nodes = nodes;

    g->adjL = (LinkedList **) malloc(sizeof(LinkedList *) * nodes);
    if(g->adjL == NULL) {
        fprintf(stderr, "Memory error\n");
        free(g);
        return NULL;
    }

    for(i = 0; i < nodes; i++)
        g->adjL[i] = initLinkedList();

    return g;
}

void GprintToFile(FILE *fp, GraphL *g) {
    int i = 0;
    LinkedList *aux;
    Edge *e;

    for(i = 0; i < g->nodes; i++){
        fprintf(fp, "%d -",i);
        aux = g->adjL[i];
        while(aux != NULL){
            e = (Edge *) getItemLinkedList(aux);
            fprintf(fp, " %d:%d", e->w, e->value);
            aux = getNextNodeLinkedList(aux);
        }
        fprintf(fp, "\n");
    }
}


void insertEdge(GraphL *g, int v, int w, int value){
    Edge *e;
    e = (Edge *) malloc(sizeof(Edge));

    e->v = v;
    e->w = w;
    e->value = value;


    g->adjL[v] = insertUnsortedLinkedList(g->adjL[v], (Item) e);
    return;
}

void deleteEdge(GraphL *g, int v, int w){
    Edge *e;
    LinkedList *aux;
    
    e = (Edge *) getItemLinkedList(g->adjL[v]);
    if(e->w == w) {
        aux = g->adjL[v];
        g->adjL[v] = getNextNodeLinkedList(g->adjL[v]);
        freeLinkedListNode(aux, freeEdge);
    }

    aux = g->adjL[v];

    while(aux != NULL) {
        if(getNextNodeLinkedList(aux) != NULL) {
            e = (Edge *) getItemLinkedList( getNextNodeLinkedList( aux ));
            if(e->w == w)
                deleteNextNode(aux, freeEdge);
            aux = getNextNodeLinkedList(aux);
        }
    }
    return;
}

void destroyGraph(GraphL *g) {
    int i = 0;
    
    for(i = 0; i < g->nodes; i++)
        freeLinkedList(g->adjL[i], freeEdge);

    free(g->adjL);
    free(g);
}

int getNodes(GraphL *g){
    return g->nodes;
}

LinkedList *getEdgesOfNode(GraphL *g, int v) {
    LinkedList *aux = g->adjL[v];
    LinkedList *edges;
    Edge *eAux, *e;

    edges = initLinkedList();

    while(aux != NULL) {
        eAux = (Edge *) getItemLinkedList(aux);
        e = (Edge *) malloc(sizeof(Edge));
        e->v = eAux->v;
        e->w = eAux->w;
        e->value = eAux->value;
        edges = insertUnsortedLinkedList(edges, (Item) e);
        aux = getNextNodeLinkedList(aux);
    }

    return edges;
}

int getOrigNode(Edge *e) {
    return e->v;
}

int getDestNode(Edge *e) {
    return e->w;
}

int getValueEdge(Edge *e) {
    return e->value;
}

/*
 *  Function:
 *    dijkstra
 *
 *  Description:
 *    uses the Dijkstra algorithm to generate the shortest path tree starting
 *    at the designated root
 *    Will stop once the toFind node is reached
 *
 *  Arguments:
 *    GraphL *g - graph to compute
 *
 *  Return value:
 *    int indexed table st, delineating the path to take
 */

int *GLDrijkstra(GraphL *g, int root, int dest) {
    int i; 
    int v, w;
    int *st;             /* indexed table, saves the origin node */
    int *wt;             /* indexed table, saves length from tree */
    int N;
    PrioQ *PQ;

    N = g->nodes;   /* get total number of nodes */

    /* initialize and write to indexed tables */
    st = (int*) malloc(sizeof(int) * N);
    if(st == NULL) {
        fprintf(stderr, "Memory error!\n");
    }
    
    wt = (int*) malloc(sizeof(int) * N);
    if(wt == NULL) {
        fprintf(stderr, "Memory error!\n");
    }

    /* initialize new priority Queue */
    PQ = NewPrioQ(N);

    for(i = 0; i < N; i++) {

        
        st[i] = -1;

        Insert(heap, wt[i]);
    }
}
