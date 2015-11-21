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
 *  Functions:
 *    printCGraph
 *    printPGraph
 *
 *  Description:
 *    prints the car path and peon path graphs previously generated respectively
 *
 *  Arguments:
 *    FILE *fp - stream to print out info
 *
 *  Return value:
 *  void
 */

void printGraph(FILE *fp, Map *parkMap);


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
