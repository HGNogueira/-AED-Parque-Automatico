#ifndef __htable__h
#define __htable__h 1

#include"defs.h"

typedef struct _hashtable HashTable;

HashTable *HTinit(int n, int p);

int HTget(HashTable *ht, char *key);
int HTinsert(HashTable *ht, int t, char *key);

void HTdestroy();

#endif
