#ifndef __prioQ__h
#define __prioQ__h 1

typedef struct _prioQ PrioQ;

PrioQ *PQinit(int *wt, int size);
void PQupdate();
void PQupdateIndex(PrioQ *PQ, int index);
int PQdelmin(PrioQ* PQ);

int PQempty();


#endif
