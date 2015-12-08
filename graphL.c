#include"graphL.h"
#include"LinkedList.h"
#include<stdio.h>
#include<stdlib.h>
#include"defs.h"
#include"prioQ.h"

typedef struct _action{
    int node;
    int wt;
} action;

void actionDestroy(Item act){
    free((action *) act);
}

struct _edge{
    int v;
    int w;
    int value;
};

struct _graphL{
    int nodes;              /* number of total nodes in a graph */
    LinkedList **adjL;      /* ajacency list representation of a graph */
    int *active;            /* node indexed table - 1 represents active
                               and 0 an inactive node */
};

void freeEdge(Item e) {
    free((Edge *) e);
    return;
}


GraphL *Ginit(int nodes) {
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

    g->active = (int *) malloc(sizeof(int) * nodes);

    for(i = 0; i < nodes; i++){
        g->adjL[i] = initLinkedList();
        g->active[i] = 1;
    }

    return g;
}

void Gprint(FILE *fp, GraphL *g) {
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


void GinsertEdge(GraphL *g, int v, int w, int value){
    Edge *e;
    e = (Edge *) malloc(sizeof(Edge));

    e->v = v;
    e->w = w;
    e->value = value;


    g->adjL[v] = insertUnsortedLinkedList(g->adjL[v], (Item) e);
    return;
}

void GdeleteEdge(GraphL *g, int v, int w){
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

void Gdestroy(GraphL *g) {
    int i = 0;
    
    for(i = 0; i < g->nodes; i++)
        freeLinkedList(g->adjL[i], freeEdge);

    free(g->active);
    free(g->adjL);
    free(g);
}

int Gnodes(GraphL *g){
    return g->nodes;
}

LinkedList *GedgesOfNode(GraphL *g, int v) {
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

int GorigOfEdge(Edge *e) {
    return e->v;
}

int GdestOfEdge(Edge *e) {
    return e->w;
}

int GvalOfEdge(Edge *e) {
    return e->value;
}

void GactivateNode(GraphL *g, int v){
    g->active[v] = 1;
    return;
}

void GdeactivateNode(GraphL *g, int v){
    g->active[v] = 0;
    return;
}

int GisNodeActive(GraphL *g, int v){
    return g->active[v];
}

void GDijkstraCleanToDest(GraphL *G, Map *parkMap, LinkedList *actions, int *st,
                                            int *wt, PrioQ *PQ, int cleanDest){
    action *a;
    LinkedList *t;

    t = actions;
    while(t != NULL){ 
        a = (action *) getItemLinkedList(t);
        wt[ a->node ] = a->wt;
        PQReinsert(PQ, a->node);
        if(a->node == cleanDest)
            break;
        t = getNextNodeLinkedList(t);
    }

    freeLinkedList(actions, actionDestroy);

    return;
}


/*
 *  Function:
 *    GDijkstra
 *
 *  Description:
 *    uses the Dijkstra algorithm to generate the shortest path tree starting
 *    at the designated root towards the destiny root
 *    Will stop once the toFind node is reached
 *
 *  Arguments:
 *    GraphL *g - graph to compute
 *    int root - tree's origin
 *    int dest - destiny node, the node you want to get to
 *    int *st - pre-Initialized path table
 *    int *wt - pre-Initialized weight table
 *    PrioQ *PQ - pre-Initialized priority queue
 *
 *  Return value:
 *    int indexed table st, delineating the path to take
 */

int GDijkstra(GraphL *G,int root, int dest, int *st, int *wt, PrioQ *PQ, Map *parkMap) {
    int hP;              /* to save highest priority index */
    LinkedList *t;       /* to go through a linked list without modifying */
    Edge *e;             /* to read adjL information contained in Edge * */
    char desc, prevDesc;

    while(!PQisempty(PQ)) {
        hP = PQdelmin(PQ);
        /* if highest priority is our destiny
         * we have found our ideal path
         */
        /* ignore if node is an inactive node */
        if(G->active[ hP ] == 0)
            continue;
        if(hP == dest || wt[hP] == NOCON)
            break;
        /* check to see if it is a ramp */
        desc = getMapRepDesc(parkMap, hP);
        if( desc == 'u' || desc == 'd'){
            prevDesc = getMapRepDesc(parkMap, st[hP]);
            /* if previous was a ramp as well, we may go in all directions */
            if( prevDesc == 'u' || prevDesc == 'd'){
                for(t = G->adjL[ hP ]; t != NULL; t = getNextNodeLinkedList(t)){
                    e = getItemLinkedList(t);
                    if( wt[ e->w ] > wt[hP] + e->value) {
                        wt[ e->w ] = wt[hP] + e->value;
                        PQupdateNode(PQ, e->w);
                        st[e->w] = hP;
                    }
                }
            } else{
                for(t = G->adjL[ hP ]; t != NULL; t = getNextNodeLinkedList(t)){
                    e = getItemLinkedList(t);
                    if( (e->w - hP != PgetN(parkMap) * PgetM(parkMap)) &&
                        (hP - e->w != PgetN(parkMap) * PgetM(parkMap)) )
                        continue;
                    if( wt[ e->w ] > wt[hP] + e->value) {
                        wt[ e->w ] = wt[hP] + e->value;
                        PQupdateNode(PQ, e->w);
                        st[e->w] = hP;
                    }
                    break;
                }
            
            }
            continue;
        }
        for(t = G->adjL[ hP ]; t != NULL; t = getNextNodeLinkedList(t)){
            e = getItemLinkedList(t);
            if( wt[ e->w ] > wt[hP] + e->value) {
                wt[ e->w ] = wt[hP] + e->value;
                PQupdateNode(PQ, e->w);
                st[e->w] = hP;
            }
        }
    }
    return wt[dest];
}
