/*
 *  File name: parkmap.h
 *
 *  Author: Beatriz Ferreira & Henrique Nogueira
 *
 *  Description: Header file for Map structure operations. The structure
 *          contains all extracted information from a parking station map
 *          and this interface simple user operations over the structure
 *               
 *  Abstract Data Type: Map
 *              This structure contains all required information over the
 *          parking station's composition as given by its input file
 *
 *  Function list:
 *    A) Initialization & Termination
 *        mapInit
 *        buildGraphs
 *        mapDestroy
 *
 *    B) Lookup
 *        getMapRepDesc
 *        PgetN
 *        PgetM
 *        PgetP
 *        isParkFull
 *
 *    C) Output
 *        mapPrintStd
 *        writeOutput
 *        WriteOutputAfterIn
 *    
 *    D) Modify
 *        clearSpotCoordinates
 *        clearSpotIDandWrite
 *        restrictMapCoordinate
 *        freeRestrictionMapCoordinate
 *        restrictMapFloor
 *        freeRestrictionMapFloor
 *
 *    E) Compute
 *        findPath
 *
 *  Non-standard dependencies:
 *    point.h
 *    graphL.h
 *    queue.h
 *    prioQ.h
 *    htable.h
 *    escreve_saida.h
 *
 *  Version: 1.0
 *
 *  Change log: N/A
 *
 */

/* multiple inclusion prevention */
#ifndef __parkmap__h
#define __parkmap__h 1

#include"point.h"
#include<stdio.h>

typedef struct _map Map;

/*
 *  Function:
 *    mapInit
 *
 *  Description:
 *    saves all the information read from the map configuration file in
 *    a Map structure
 *
 *  Arguments:
 *    char *filename - name of the map configuration file
 *
 *  Return value:
 *    returns pointer to new Map
 */

Map *mapInit(char *filename);


/*
 *  Function:
 *    buildGraphs
 *
 *  Description: 
 *    generates the car path graph and peon path  weighted directed graphs
 *    using the already built mapRep matrices
 *
 *  Arguments:
 *    pointer to map structure
 *
 *  Return value:
 *    void
 */

void buildGraphs(Map *parkMap);


/*
 *  Function:
 *    mapPrintStd 
 *
 *  Description: 
 *    prints map configuration information to stdout
 *
 *  Arguments:
 *    pointer to map structure to print
 *
 *  Return value:
 *    void
 */

void mapPrintStd(Map *parkMap);


/* 
 * Functions:
 *     writeOutput
 *     writeOutputAfterIN
 *
 * Description:
 *     using the escreve_saidas module, these functions serve as a way to 
 *  easily write the output according to protocol format.
 *     While the first one will write include the first point of the path
 *  the second one wont
 *
 * Arguments:
 *      FILE *fp - file to print
 *      Map *parkMap - Map structure respective to the path
 *      int *st - path vector
 *      int cost - total cost of a path according to protocol metric system
 *      int time - time indicating the beggining
 *      char *ID - vehicle string identifier
 *      char accessType - desired destiny of the vehicle
 *      int origTime - original entrance time according to input file (AfterIn)
 *      int pathSize - number of moves contained in a path
 *
 *  return value:
 *      none
 *
 */

void writeOutput(FILE *fp, Map *parkMap, int *st, int cost, int time, char *ID, 
                                              char accessTypem, int pathSize);
void writeOutputAfterIn(FILE *fp, Map *parkMap, int *st, int cost, int time,
                 char *ID, char accessType, int origTime, int pathSize);


/* Functions:
 *     getMapRepDesc
 *     PgetN
 *     PgetM
 *     PgetP
 *
 * Description:
 *     field getter functions:
 *
 *         getMapRepDesc - returns respective node character descriptor
 *         PgetN - returns number of xx axis columns in the park configuration
 *         PgetM - returns number of yy rows ...
 *         PgetP - returns the number of floors ....
 *
 * Arguments:
 *     Map *parkMap - configuration Map
 *     int node - respective node  (getMapRepDesc)
 *
 * Return value:
 *     char - respective character descriptor of the map representation
 */

char getMapRepDesc(Map *parkMap, int node);
int PgetN(Map *parkMap);
int PgetM(Map *parkMap);
int PgetP(Map *parkMap);


/*
 *  Functions:
 *    clearSpotCoordinates
 *    clearSpotID
 *
 *  Description:
 *    Frees parking spots coordinates
 *
 *  Arguments:
 *    Map *parkMap - configuration map
 *    int x, y, z - parking spot's coordinates (clearSpotCoordinates)
 *    char *ID - car identifier (clearSpotID)
 *
 *  Return value:
 *    Point * - table of access points
 */

void clearSpotCoordinates(Map *parkMap, int x, int y, int z);
void clearSpotIDandWrite(FILE *fp, Map *parkMap, char *ID, int time);


/*
 *  Function:
 *    findPath
 *
 *  Description:
 *    finds the ideal path from an entrance point to an access point
 *    in a given Map
 *    returns the total cost of the path
 *
 *  Arguments:
 *    Map *parkMap - contains the parking map configuration
 *    char *ID - identifier of the car
 *    int ex, ey, ez - entrance point coordinates
 *    Point *access - contains information about the destiny node
 *    int *cost - reference of int to use as total cost of path 
 *    int *stSize - size of the path
 *
 *  Return value:
 *    int *st - path vector
 */

int *findPath(Map *parkMap, char *ID, int ex, int ey, int ez, 
                                    char accessType, int *cost, int *stSize);


/*
 * Function:
 *     restrictMapCoordinate
 *
 * Description:
 *     restricts a location in the map, location is no longer available for
 *  usage
 *
 *  Arguments:
 *     Map *parkMap - contains the current parking map configuration
 *     int x, y, z - coordinates for applied restriction
 *  return value:
 *     void
 */

void restrictMapCoordinate(Map *parkMap, int x, int y, int z);


/*
 * Function:
 *     freeRestrictionMapCoordinate
 *
 * Description:
 *     unrestricts a map coordinate, it is again usable for calculating ideal
 *  paths
 *
 *  Arguments:
 *     Map *parkMap - contains the current parking map configuration
 *     int x, y, z - coordinates to free from restriction
 *  return value:
 *     void
 */

void freeRestrictionMapCoordinate(Map *parkMap, int x, int y, int z);


/*
 * Function:
 *     restrictMapFloor
 *
 * Description:
 *     restricts a whole floor of the car map configuration, cars and peons 
 *  may no longer transit in said floor
 *
 *  Arguments:
 *     Map *parkMap - contains the current parking map configuration
 *     int floor - floor number
 *  return value:
 *     void
 */

void restrictMapFloor(Map *parkMap, int floor);


/*
 * Function:
 *     freeRestrictionMapFloor
 *
 * Description:
 *     frees previously applied restriction to whole floor
 *
 *  Arguments:
 *     Map *parkMap - contains the current parking map configuration
 *     int floor - floor number
 *  return value:
 *     void
 */

void freeRestrictionMapFloor(Map *parkMap, int floor);


/*
 * Function:
 *     isParkFull
 *
 * Description:
 *     by keeping track of the available spots in a parking lot we are able to
 *  say if the parking lot is available or not
 */

int isParkFull(Map *parkMap);


/*
 *  Functions:
 *    mapDestroy
 *
 *  Description:
 *    frees all previously allocated memory in Map struct
 *
 *  Arguments:
 *    pointer to map struct
 *
 *  Return value:
 *    void
 */

void mapDestroy(Map *parkMap);


#endif
