#ifndef __parkmap__h
#define __parkmap__h 1

#ifndef __point__h
#include"point.h"
#endif

typedef struct _map Map;

Map *mapInit(char *filename);
void mapPrintStd(Map *parkMap);

void mapDestroy(Map *parkMap);


#endif
