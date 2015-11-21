#ifndef __point__h
#define __point__h 1
#include"defs.h"

typedef struct _point Point;

Point *newPoint(char* ID, char desc, int x, int y, int z);

int getID(Point *, char *);
int getx(Point *);
int gety(Point *);
int getz(Point *);
char getDesc(Point *);

void pointPrintStd(Point *);

void pointDestroy(Item myPoint);

#endif
