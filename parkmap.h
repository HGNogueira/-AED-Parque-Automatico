#ifndef __parkmap__h
#define __parkmap__h 1

typedef struct _map Map;

Map *mapInit(char *filename);
void mapPrintStd(Map *parkMap);


#endif
