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

int *GLDijkstra(GraphL *G, int *cost, int root, int dest) {
    int i; 
    int *wt;             /* indexed table, saves length from tree */
    int *st;
    int N;
    int hP;              /* to save highest priority index */
    LinkedList *t;       /* to go through a linked list without modifying */
    Edge *e;             /* to read adjL information contained in Edge * */
    PrioQ *PQ;

    N = G->nodes;   /* get total number of nodes */

    /* initialize and write to indexed tables */
    st = (int*) malloc(sizeof(int) * N);
    if(st == NULL) {
        fprintf(stderr, "Memory error!\n");
    }
    
    wt = (int*) malloc(sizeof(int) * N);
    if(wt == NULL) {
        fprintf(stderr, "Memory error!\n");
    }

    /* add values to indexed tables */
    for(i = 0; i < N; i++) {
        wt[i] = NOCON;    /* all weight values will be init as not connected */
        st[i] = -1;
    }

    /* initialize new priority Queue */
    PQ = PQinit(wt, N);

    st[root] = root;
    wt[root] = 0;
    PQupdateIndex(PQ, root);

    while(!PQisempty(PQ)) {
        hP = PQdelmin(PQ);
        /* if highest priority is our destiny
         * we have found our ideal path
         */
        if(hP == dest)
            break;
        if( wt[hP] == NOCON)
            break;
        for(t = G->adjL[ hP ]; t != NULL; t = getNextNodeLinkedList(t)){
            e = getItemLinkedList(t);
            if( wt[ e->w ] > wt[hP] + e->value) {
                wt[ e->w ] = wt[hP] + e->value;
                PQupdateIndex(PQ, e->w);
                st[e->w] = hP;
            }
        }
    }
    if(st[dest] == -1) {
        fprintf(stderr, "Couldn't get to destiny\n");
        exit(1);
    }

    PQdestroy(PQ);
    *cost = wt[dest];

    free(wt);

    return st;
}
