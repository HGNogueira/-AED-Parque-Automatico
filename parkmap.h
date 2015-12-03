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
 *        mapDestroy
 *
 *    B) Properties
 *        ---
 *
 *    C) Navigation
 *        ---
 *
 *    D) Lookup
 *        ---
 *
 *
 *  Dependencies:
 *    dbg.h
 *    point.h
 *    stdlib.h
 *    stdio.h
 *    string.h
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
 *    generates the car path graph and peon path graphs using the given
 *    configuration
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
 *  Function:
 *    printGraph
 *
 *  Description:
 *    prints the car and peon graphs previously generated respectively
 *
 *  Arguments:
 *    FILE *fp - stream to print out info
 *    Map *parkMap - configuration map
 *
 *  Return value:
 *  void
 */

void printGraph(FILE *fp, Map *parkMap);


/*
 *  Function:
 *    getAccessPoints
 *
 *  Description:
 *    returns the int identifier of the special access type node
 *
 *  Arguments:
 *    Map *parkMap - configuration map
 *    char desc - access type descriptor character
 *
 *  Return value:
 *    int - number of node
 */

int getAccessTypeNode(Map *parkMap, char desc);

void writeOutput(FILE *fp, Map *parkMap, int *st, int cost, int time, char *ID, char accessType);


/*
 *  Function:
 *    getAccessPoints
 *
 *  Description:
 *    returns table with access points of a certain description
 *
 *  Arguments:
 *    Map *parkMap - configuration map
 *    char desc - descriptive character of desired type of access
 *    int *size - reference to a size variable, will save table size
 *
 *  Return value:
 *    Point * - table of access points
 */

char getMapRepDesc(Map *parkMap, int x, int y, int z);


/*
 *  Function:
 *    getAccessPoints
 *
 *  Description:
 *    returns table with access points of a certain description
 *
 *  Arguments:
 *    Map *parkMap - configuration map
 *    char desc - descriptive character of desired type of access
 *    int *size - reference to a size variable, will save table size
 *
 *  Return value:
 *    Point * - table of access points
 */

Point **getAccessPoints(Map *parkMap, char desc, int *size);


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
 *
 *  Return value:
 *    int *st - path vector
 */

int *findPath(Map *parkMap, char *ID, int ex, int ey, int ez, char accessType, int *cost);


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


/*  UNUSED FUNCTION!!!!!!! TAKE OUT
 *  Function:
 *    loadRestrictions
 *
 *  Description:
 *    loads restriction into Restriction vector of Map structure
 *
 *  Arguments:
 *    Map *parkMap - contains the parking map configuration
 *    char *restrictionFile - restriction type file with all restrictions
 *
 *  Return value:
 *    int
 */

/* void loadRestrictions(Map *parkMap, char *restrictionFile); */


/*
 *  Functions:
 *    mapDestroy
 *
 *  Description:
 *    frees all previously allocated memory
 *
 *  Arguments:
 *    pointer to map structure
 *
 *  Return value:
 *    void
 */

void mapDestroy(Map *parkMap);


#endif
