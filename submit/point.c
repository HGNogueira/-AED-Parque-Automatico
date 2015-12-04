/*
 *  Author: Beatriz Ferreira & Henrique Nogueira
 *
 *  Description: coordinate point related function for important map points
 *
 *  Implementation details:
 *      The Point structure represents an abstract data type for any important
 *  point to be used in the parking map. It contains its coordinates, ID and
 *  the character description
 *
 *  Version: 1.0
 *
 *  Change log: N/A
 *
 */

#include"point.h"
#include"dbg.h"
#include"defs.h"
#include<stdlib.h>
#include<string.h>


/*
 *  Data Type: Point
 *
 *  Description: 
 *      struct containing information about an important Point located in a map
 *
 */

struct _point{
    int x, y, z; /* coordinates */
    char* id;  /* Point identificator */
    char desc; /* optional description character */
};


/*
 *  Function:
 *      newPoint
 *  Description:
 *      Creates a new point structure pointer
 *
 *  Arguments:
 *      All the required fields in a Point structure:
 *      char* ID  - Identifier
 *      char desc - Description
 *      int x, y, z - x, y, z coordinates
 *  Return value:
 *      struct Point *
 *
 *  Secondary effects:
 *      None
 */
Point *newPoint(char *ID, char desc, int x, int y, int z) {
    Point *myPoint;
    int idSize;
    int i;

    myPoint = (Point*) malloc(sizeof(Point));
    check_mem(myPoint);

    idSize = strlen(ID);
    myPoint->id = (char*) malloc(sizeof(char) * idSize + 1);
    for(i = 0; i < idSize;i++)
        myPoint->id[i] = ID[i];
    myPoint->id[idSize] = '\0';

    myPoint->desc = desc;
    myPoint->x = x;
    myPoint->y = y;
    myPoint->z = z;
    
    return myPoint;
}


/*
 *  Functions: (getter functions)
 *  
 *  Description:
 *      Simple Point field access functions
 *
 *  Arguments:
 *      Point *
 *      char * - in the case of getID
 *  Return value:
 *      int, (char in case of getDesc)
 *
 *  Secondary effects:
 *      None
 */

char *getID(Point *myPoint) {
    return myPoint->id;
}

int getx(Point *myPoint){
    return myPoint->x;
}
int gety(Point *myPoint){
    return myPoint->y;
}
int getz(Point *myPoint){
    return myPoint->z;
}

char getDesc(Point *myPoint){
    return myPoint->desc;
}

 /*
 *  function:
 *      pointPrintStd
 *  description:
 *      prints Point content field in pleasant way to stdout
 *
 *  arguments:
 *      pointer to struct point
 *  return value:
 *      void
 *
 *  secondary effects:
 *      none
 */

void pointPrintStd(Point *myPoint) {
    fprintf(stdout, "%s %d %d %d %c\n", myPoint->id, myPoint->x, myPoint->y,
            myPoint->z, myPoint->desc);
    return;
}


/*
 *  function:
 *      pointdestroy
 *  description:
 *      destroys all memory previously allocated in point structure
 *
 *  arguments:
 *      pointer to struct point
 *  return value:
 *      void
 *
 *  secondary effects:
 *      turns argument into null pointer
 */

void pointDestroy(Item myPoint) {
    free( ((Point *) myPoint)->id );
    free((Point *) myPoint);
}
