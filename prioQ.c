#include"prioQ.h"
#include<stdlib.h>
#include<stdio.h>

struct _prioQ{
    int N;             /* number of elements in queue */
    int size;          /* maximum size of queue */
    int *wt;           /* indexed table with weights */
    int *heap;         /* table satisfying heap condition using wt's indexes */
    int *index;        /* indexed table to find an index in the heap table */
};

void FixDown(PrioQ *PQ, int heapIndex);
void FixUp(PrioQ *PQ, int heapIndex);

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

void PQupdateIndex(PrioQ *PQ, int index){
    /* verify if index corresponds to heap's highest priority */
    if(PQ->index[index] == 0)
        FixDown(PQ, 0);
    /* try to FixUp first */
    else if(PQ->heap[index] < PQ->heap[(index - 1)/2]) {
        FixUp(PQ, PQ->index[index]);
        return;
    }
    /* if we dont FixUp we can FixDown */
    FixDown(PQ, PQ->index[index]);
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

    FixDown(PQ, 0);

    PQ->N--;

    return heap[N - 1];
}

int PQempty(PrioQ *PQ){
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
                if( wt[heap[i]] >= wt[heap[j]]) {
                    aux = heap[j];

                    heap[j] = heap[i];
                    index[ heap[i] ] = j;

                    heap[i] = aux;
                    index[ aux ] = i;
                    continue;
                }
            }
            else{
                if( wt[heap[i]] >= wt[heap[j + 1]]) {
                    aux = heap[j + 1];

                    heap[j + 1] = heap[i];
                    index[ heap[i] ] = j + 1;

                    heap[i] = aux;
                    index[ aux ] = i;
                    continue;
                }
            }

            return;
        }
        /* if it has only one son */
        else{
            /* is that son smaller ?*/
            if( wt[heap[i]] >= wt[heap[j]]) {
                aux = heap[j];

                heap[j] = heap[i];
                index[ heap[i] ] = j;

                heap[i] = aux;
                index[ aux ] = i;
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
            continue;
        }
        return;
    }
    return;
}

void PQprintHeap(PrioQ* PQ) {
    int i = 0;

    for(i = 0; i < PQ->N; i++){
        fprintf(stdout, "%d ", PQ->heap[i]);
    fprintf(stdout, "\n");
    }
    return;
}
