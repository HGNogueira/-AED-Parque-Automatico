#ifndef __parkmap__h
#define __parkmap__h 1

typedef struct _map Map;
typedef struct _point Point;

Map *mapInit(char *filename);
Point *newPoint(char* ID, char desc, int x, int y, int z);



#endif
