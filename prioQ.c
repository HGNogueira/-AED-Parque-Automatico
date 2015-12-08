#include"prioQ.h"
#include<stdlib.h>
#include<stdio.h>
#include"graphL.h"

struct _prioQ{
    int N;             /* number of elements in queue */
    int size;          /* maximum size of queue */
    int *wt;           /* indexed table with weights */
    int *heap;         /* table satisfying heap condition using wt's indexes */
    int *index;        /* indexed table to find an index in the heap table */
};

void FixDown(PrioQ *PQ, int heapIndex);
void FixUp(PrioQ *PQ, int heapIndex);
void FixUpHighPrio(PrioQ *PQ, int heapIndex);

PrioQ *PQinit(int *wt, int size){
    int i;
    PrioQ *PQ;
    PQ = (PrioQ*) malloc(sizeof(PrioQ));

    PQ->size = size;
    PQ->N = size;      /* initial heap size will be the same as his total size */
    PQ->wt = wt;       /* we shall be using the exact same pointer as given */

    PQ->heap = (int*) malloc(sizeof(int) * size);
    PQ->index = (int*) malloc(sizeof(int) * size);

    for(i = 0; i < size; i++){
        PQ->heap[i] = i;
        PQ->index[i] = i;
    }

    return PQ;
}

void PQupdate(PrioQ *PQ){
    int i = 0;
    
    for(i = PQ->N/2 -1; i >= 0; i--) {
        FixDown(PQ, i);
    }
}

void PQupdateNode(PrioQ *PQ, int node){
    int *heap = PQ->heap;
    int *wt = PQ->wt;
    int heapIndex = PQ->index[node];
    /* verify if index corresponds to heap's highest priority */
    if(heapIndex == 0)
        FixDown(PQ, 0);
    /* try to FixUp first */
    else if(wt[ heap[heapIndex] ] < wt[ heap[(heapIndex - 1)/2] ]) {
        FixUp(PQ, heapIndex);
        return;
    }
    /* if we dont FixUp we can FixDown */
    FixDown(PQ, heapIndex);
    return;
}

void PQupdateNodeHighPrio(PrioQ *PQ, int node){
    int *heap = PQ->heap;
    int *wt = PQ->wt;
    int heapIndex = PQ->index[node];
    /* verify if index corresponds to heap's highest priority */
    if(heapIndex == 0)
        FixDown(PQ, 0);
    /* try to FixUp first */
    else if(wt[ heap[heapIndex] ] < wt[ heap[(heapIndex - 1)/2] ]) {
        FixUp(PQ, heapIndex);
        return;
    }
    /* if we dont FixUp we can FixDown */
    FixDown(PQ, heapIndex);
    return;
}

int PQdelmin(PrioQ* PQ) {
    int *heap = PQ->heap;
    int *index = PQ->index;
    int aux;
    int N; 

    N = PQ->N;

    if(N == 0)
        return -1;


    /* switch places in table, and update index table */
    aux = heap[N - 1];
    
    heap[N - 1] = heap[0];
    index[ heap[0] ] = N - 1;

    heap[0] = aux;
    index[ aux ] = 0;

    PQ->N--;
    FixDown(PQ, 0);


    return heap[N - 1];
}

void PQReinsert(PrioQ *PQ, int node){
    int auxNode, auxIndex;
    int *heap = PQ->heap;
    int *index = PQ->index;
    int N;

    N = PQ->N;
    if(index[node] < N){
        PQupdateNode(PQ, node);
        return;
    }
    
    auxNode = heap[N];
    auxIndex = index[node];

    heap[N] = node;
    index[node] = N;

    heap[auxIndex] = auxNode;
    index[auxNode] = auxIndex;

    PQ->N++;

    FixUp(PQ, N);

    return;
}

int PQisempty(PrioQ *PQ){
    return PQ->N <= 0 ? 1 : 0;
}
    

void FixDown(PrioQ *PQ, int heapIndex) {
    int i, j, N;
    int *heap, *wt, *index;
    int aux;

    heap = PQ->heap;
    wt = PQ->wt;
    index = PQ->index;

    N = PQ->N;

    i = heapIndex;
    while((i + 1)*2  - 1< N) {
        j = (i + 1)*2 - 1;
        /* if the element has 2 sons */
        if( j + 1 < N) {
            /* check for the smallest weighted son */
            if( wt[ heap[j] ] <= wt[ heap[j + 1]]) {
                if( wt[heap[i]] > wt[heap[j]]) {
                    aux = heap[j];

                    heap[j] = heap[i];
                    index[ heap[i] ] = j;

                    heap[i] = aux;
                    index[ aux ] = i;
                    i = j;
                    continue;
                }
            }
            else{
                if( wt[heap[i]] > wt[heap[j + 1]]) {
                    aux = heap[j + 1];

                    heap[j + 1] = heap[i];
                    index[ heap[i] ] = j + 1;

                    heap[i] = aux;
                    index[ aux ] = i;
                    i = j + 1;
                    continue;
                }
            }

            return;
        }
        /* if it has only one son */
        else{
            /* is that son smaller ?*/
            if( wt[heap[i]] > wt[heap[j]]) {
                aux = heap[j];

                heap[j] = heap[i];
                index[ heap[i] ] = j;

                heap[i] = aux;
                index[ aux ] = i;
                i = j;
                continue;
            }
            /* if not we are done */
            return;
        }
    }
    /* has no more sons */
    return;
}


void FixUp(PrioQ *PQ, int heapIndex) {
    int i, j;
    int *heap, *wt, *index;
    int aux;

    heap = PQ->heap;
    wt = PQ->wt;
    index = PQ->index;

    i = heapIndex;
    while(i > 0){
        j = (i - 1)/2;
        if( wt[heap[i]] < wt[heap[j]] ) {
            aux = heap[j];

            heap[j] = heap[i];
            index[ heap[i] ] = j;

            heap[i] = aux;
            index[ aux ] = i;
            i = j;
            continue;
        }
        return;
    }
    return;
}


void FixUpHighPrio(PrioQ *PQ, int heapIndex) {
    int i, j;
    int *heap, *wt, *index;
    int aux;

    heap = PQ->heap;
    wt = PQ->wt;
    index = PQ->index;

    i = heapIndex;
    while(i > 0){
        j = (i - 1)/2;
        if( wt[heap[i]] < wt[heap[j]] ) {
            aux = heap[j];

            heap[j] = heap[i];
            index[ heap[i] ] = j;

            heap[i] = aux;
            index[ aux ] = i;
            i = j;
            continue;
        }
        return;
    }
    return;
}

void PQprintHeap(PrioQ* PQ) {
    int i = 0;

    for(i = 0; i < PQ->N; i++){
        fprintf(stdout, "%d %d\n", PQ->heap[i], PQ->wt[i]);
    }
    return;
}

 /* Functions:
  *     PQclean
  *     PQreset
  *
  * Description:
  *     The following functions serve as a way to reset wt, st and PQ to their
  * initial state, avoiding the unnapropriate reinitialization for reusage
  */

void PQclean(PrioQ *PQ, int *wt, int *st, int heapNode){
    if(heapNode >= PQ->N)
        return;
    if(wt[PQ->heap[heapNode]] == NOCON)
        return;
    wt[PQ->heap[heapNode]] = NOCON;
    st[PQ->heap[heapNode]] = -1;

    PQclean(PQ, wt, st, 2*heapNode + 1);
    PQclean(PQ, wt, st, 2*heapNode + 2);
}

void PQreset(PrioQ* PQ, int *st, int *wt, int realN){
    int i;
    for(i = PQ->N; i < realN; i++){
        wt[PQ->heap[i]] = NOCON;
        st[PQ->heap[i]] = -1;
        PQ->N++;
    }

    PQclean(PQ, wt, st, 0);

    return;
}

void PQdestroy(PrioQ* PQ){
    free(PQ->index);
    free(PQ->heap);
    free(PQ);
    return;
}
