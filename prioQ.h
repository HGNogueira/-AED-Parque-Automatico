#ifndef __prioQ__h
#define __prioQ__h 1

typedef struct _prioQ PrioQ;

PrioQ *PQinit(int *wt, int size);
void PQupdate(PrioQ *PQ);
void PQupdateNode(PrioQ *PQ, int node);
void PQupdateNodeHighPrio(PrioQ *PQ, int node);

void PQReinsert(PrioQ *PQ, int node);
int PQdelmin(PrioQ* PQ);

void PQprintHeap(PrioQ* PQ);
int isPQreset(PrioQ *PQ, int *st, int *wt, int realNodes);

int PQisempty();
void PQreset(PrioQ *PQ, int *st, int *wt, int realNodes);
void PQdestroy(PrioQ *PQ);


#endif
