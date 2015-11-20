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

Map *mapInit(char *filename);

void printCGraph(FILE *fp, Map *parkMap);
void printPGraph(FILE *fp, Map *parkMap);

void mapPrintStd(Map *parkMap);
void buildGraphs(Map *parkMap);

void mapDestroy(Map *parkMap);


#endif
