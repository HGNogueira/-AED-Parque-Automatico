#ifndef __prioQ__h
#define __prioQ__h 1

typedef struct _prioQ PrioQ;

PrioQ *PQinit(int *wt, int size);
void PQupdate(PrioQ *PQ);
void PQupdateNode(PrioQ *PQ, int node);
void PQReinsert(PrioQ *PQ, int node);
int PQdelmin(PrioQ* PQ);

void PQprintHeap(PrioQ* PQ);

int PQisempty();
void PQdestroy(PrioQ *PQ);


#endif
