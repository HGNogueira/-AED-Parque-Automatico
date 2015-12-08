/*
 *  Author: Beatriz Ferreira & Henrique Nogueira
 *
 *  Description: This auxiliary module defines the Points ADT which contains the
 *  following information
 *          
 *          -Tridimensional Coordinates
 *          -String descriptor (ID)
 *          -character descriptor
 *
 *  Version: 1.0
 *
 *  Change log: N/A
 *
 */


#ifndef __point__h
#define __point__h 1
#include"defs.h"

/*
 *  Data Type: Point
 *
 *  Description: 
 *      struct containing information about an important Point located in a map
 *
 */

typedef struct _point Point;


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
 */

Point *newPoint(char* ID, char desc, int x, int y, int z);


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
 */

char* getID(Point *);
int getx(Point *);
int gety(Point *);
int getz(Point *);
char getDesc(Point *);


/*
*  Function:
*      pointPrintStd
*  description:
*      prints Point content field in pleasant way to stdout
*
*  arguments:
*      pointer to struct point
*  return value:
*      void
*
*/

void pointPrintStd(Point *);


/*
 *  function:
 *      pointdestroy
 *  description:
 *      frees all memory previously allocated in point structure
 *
 *  arguments:
 *      pointer to struct point
 *  return value:
 *      void
 *
 */

void pointDestroy(Item myPoint);

#endif
