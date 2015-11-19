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
#include<stdlib.h>


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

    myPoint = (Point*) malloc(sizeof(Point));
    check_mem(myPoint);

    myPoint->id = ID;
    myPoint->desc = desc;
    myPoint->x = x;
    myPoint->y = y;
    myPoint->z = z;
    
    return myPoint;
}

void pointPrintStd(Point *myPoint) {
    fprintf(stdout, "%s %d %d %d %c\n", myPoint->id, myPoint->x, myPoint->y,
            myPoint->z, myPoint->desc);
    return;
}
