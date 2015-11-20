/*
 *  Author: Beatriz Ferreira & Henrique Nogueira
 *
 *  Description: park map related functions
 *
 *  Implementation details:
 *      The Map structure represents a certain park Map configuration in terms
 *  of all the information that is relevant to solve the automatic parking 
 *  problem
 *
 *  Version: 1.0
 *
 *  Change log: N/A
 *
 */


#include"parkmap.h"
#include"dbg.h"
#include"point.h"
#include"graphL.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 256
    /* toIndex - macro to convert coordinates into an 1 dimensional index
     *
     * a - column
     * b - row
     * c - floor
     * A - num of columns
     * B - num of rows 
     * C - num of floors
     */
#define toIndex(a,b,c,A,B,C) a + A*(b) + A*B*c 


/*
 *  Data Type: Map
 *
 *  Description: 
 *      Structure with information on the park configuration, including its 
 *  graph representation
 *
 */

struct _map{
    int N, M; /* floor dimensions NxM */
    int P;    /* num of floors */
    int E, S; /* num of entrances (E) and peon access points (S) */

    char ***mapRep; /* table of matrices to represent multiple floor map */
    Point **accessPoints; /* table of map access points */
    Point **entrancePoints; /* table of map entrance points */

    GraphL *cGraph, *pGraph; /* car and pedestrian graphs */
};


/*
 *  Function:
 *      mapInit
 *  Description:
 *      Reads a map configuration file and saves information into its map 
 *  structure
 *
 *  Arguments:
 *      string with the name of the map configuration file
 *          char *filename
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      None
 */

Map *mapInit(char *filename) {
    FILE *fp;
    Map *parkMap; 
    int n, m, p;                             /* iteration variables */
    char auxChar, desc;                      /* auxiliary chars */
    char ID[SIZE], *auxString, skipLine[SIZE]; /* auxiliary strings */
    int x, y, z;                             /* point coordinate values */
    int atE = 0, atA = 0;                    /* control variables for access 
                                                and entrance tables */

    parkMap = (Map*) malloc(sizeof(Map));
    check_mem(parkMap);      /* memory check debug macro */

    fp = fopen( filename, "r");
    check_file(fp);          /* file check debug macro */                   

    fscanf(fp, "%d %d %d %d %d", &parkMap->N, &parkMap->M, &parkMap->P,
                &parkMap->E, &parkMap->S);

    fgets(skipLine, SIZE, fp); /* make file stream point to next line) */
    skipLine[0] = '\0';

    /* initializing representation matrices */
    parkMap->mapRep = (char***) malloc(sizeof(char**) * parkMap->N);
    check_mem(parkMap->mapRep);
    for(n = 0; n < parkMap->N; n++) {
        parkMap->mapRep[n] = (char**) malloc(sizeof(char*) * parkMap->M);
        check_mem(parkMap->mapRep[n]);
        for(m = 0; m < parkMap->M; m++){
            parkMap->mapRep[n][m] = (char*) malloc(sizeof(char) * parkMap->P);
            check_mem(parkMap->mapRep[n][m]);
        }
    }

    /* initialize access points and entrance points tables */
    parkMap->accessPoints = (Point**) malloc(sizeof(Point*) * parkMap->S);
    parkMap->entrancePoints = (Point**) malloc(sizeof(Point*) * parkMap->E);

    /* start reading rest of file */

    for(p = 0; p < parkMap->P; p++) {
        /* read first m lines starting from beggining of floor contruction */
        for(m = parkMap->M - 1; m >= 0 ; m--) {
            for(n = 0; n < parkMap->N; n++) {
                parkMap->mapRep[n][m][p] = (char) fgetc(fp); 
            }
            fgets(skipLine, SIZE, fp); /* make file stream point to next line) */
            skipLine[0] = '\0';
        }
        
        /* read access and entrance point info between floors */
        auxChar = (char) fgetc(fp);
        while(auxChar != '+') {
            switch(auxChar) {
                case 'E':
                    ungetc(auxChar, fp);
                    fscanf(fp, "%s %d %d %d %c", ID, &x, &y, &z, &desc);
                    auxString = strdup(ID);
                    ID[0] = '\0';
                    parkMap->entrancePoints[atE] = newPoint(auxString, desc, x, y, z);
                    atE++;
                    break;
                case 'A':
                    ungetc(auxChar, fp);   /* back to start of description */
                    fscanf(fp, "%s %d %d %d %c", ID, &x, &y, &z, &desc);
                    auxString = strdup(ID);
                    ID[0] = '\0';
                    parkMap->accessPoints[atA] = newPoint(auxString, desc, x, y, z);
                    atA++;
                    break;
            }
            auxChar = (char) fgetc(fp);    /* read next character from file */
        }
        fgets(skipLine, SIZE, fp); /* make file stream point to next line) */
        skipLine[0] = '\0';
    }

    fclose(fp);

    return parkMap;
}               

/*
 *  Function:
 *      generateGraphs
 *  Description:
 *      generates pedestrian and car graphs with list representation
 *
 *  Arguments:
 *      Pointer to struct Map
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      initializes and computes both cGraph and pGraph fields in struct Map
 */

void buildGraphs(Map *parkMap) {
    int n, m, p, i;    /* iteration variables */
    int N, M ,P;
    int x, y, z;       /* point coordinates */
    GraphL *cGraph, *pGraph; /* adjacency list graphs */

    N = parkMap->N;
    M = parkMap->M;
    P = parkMap->P;

    /* initializing the graphs */
    cGraph = graphInit(N * M * P);
    pGraph = graphInit(N * M * P);
    
    /* start computing using the representation matrix
     *
     * n will go from 1 to N - 2 to ignore walls
     * m will go from 1 to M - 2 to ignore walls
     *
     * later we will go over the entrance and access points
     * to complete the graph
     */

    /* useful macros to get neighbour chars */
    #define CENTER parkMap->mapRep[n][m][p]
    #define LEFT parkMap->mapRep[n-1][m][p]
    #define RIGHT parkMap->mapRep[n+1][m][p]
    #define TOP parkMap->mapRep[n][m+1][p]
    #define BOTTOM parkMap->mapRep[n][m-1][p]

    for(p = 0; p < P; p++){
        for(m = 1; m < M - 1; m++){
            for(n = 1; n < N - 1; n++){
                switch(parkMap->mapRep[n][m][p]){
                    case '@': break;
                    case 'x':
                        break;
                    case 'u':
                        /* connect car path with upper floor */
                        insertEdge(cGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p + 1,N,M,P), 1);
                        break;
                    case 'd':
                        /* connect car path with lower floor */
                        insertEdge(cGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p - 1,N,M,P), 1);
                        break;

                    case ' ':   /* if on a free way */

                        /* check for possibility of edge with neighbours */
                        /* first for the case of the car graph
                         * then for the peon graph
                         *
                         * the edge values are of 1 when in cGraph
                         * and of 3 when in pGraph
                         *
                         * check for all directions
                         */

                        /* strchr function from <string.h> will be
                         * used in order to check if a character 
                         * belongs in a set of forbidden chars
                         */

                        /* LEFT */
                        if( strchr("@exa", (int) LEFT) == NULL){
                            insertEdge(cGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n-1,m,p,N,M,P), 1);
                        } 
                        if( strchr("@xei.ud", (int) LEFT) == NULL){
                            insertEdge(pGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n-1,m,p,N,M,P), 3);
                        }

                        /* RIGHT */
                        if( strchr("@exa", (int) RIGHT) == NULL){
                            insertEdge(cGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n+1,m,p,N,M,P), 1);
                        } 
                        if( strchr("@xei.ud", (int) RIGHT) == NULL){
                            insertEdge(pGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n+1,m,p,N,M,P), 3);
                        }

                        /* TOP */
                        if( strchr("@exa", (int) TOP) == NULL){
                            insertEdge(cGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m+1,p,N,M,P), 1);
                        } 
                        if( strchr("@xei.ud", (int) TOP) == NULL){
                            insertEdge(pGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m+1,p,N,M,P), 3);
                        }

                        /* BOTTOM */
                        if( strchr("@exa", (int) BOTTOM) == NULL){
                            insertEdge(cGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m-1,p,N,M,P), 1);
                        } 
                        if( strchr("@xei.ud", (int) BOTTOM) == NULL){
                            insertEdge(pGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m-1,p,N,M,P), 3);
                        }
                        break;
                    case '.':
                        /* check for possibility of edge with neighbours */
                        /* 
                         * only checking peon graph because no car will 
                         * leave parking spot after parking unless is leaving
                         *
                         * check for all directions
                         */
                        
                        /* strchr function from <string.h> will be
                         * used in order to check if a character 
                         * belongs in a set of forbidden chars
                         */

                        /* LEFT */
                        if( strchr("@xei.ud", (int) LEFT) == NULL){
                            insertEdge(pGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n-1,m,p,N,M,P), 3);
                        }

                        /* RIGHT */
                        if( strchr("@xei.ud", (int) RIGHT) == NULL){
                            insertEdge(pGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n+1,m,p,N,M,P), 3);
                        }

                        /* TOP */
                        if( strchr("@xei.ud", (int) TOP) == NULL){
                            insertEdge(pGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m+1,p,N,M,P), 3);
                        }

                        /* BOTTOM */
                        if( strchr("@xei.ud", (int) BOTTOM) == NULL){
                            insertEdge(pGraph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m-1,p,N,M,P), 3);
                        }

                }
            }
        }

                    
    }
    /* end of for cycle */

    /* go through entrance points and make new edges */
    for(i = 0; i < parkMap->E; i++) {
        x = getx(parkMap->entrancePoints[i]);
        y = gety(parkMap->entrancePoints[i]);
        z = getz(parkMap->entrancePoints[i]);

        if(x == 0) /* at the left wall, add path to the right of the entrance */
            insertEdge(cGraph, toIndex(x,y,z,N,M,P),
                        toIndex(x+1,y,z,N,M,P), 1);
        if(x == parkMap->N - 1)                 /* at the right wall ... */
            insertEdge(cGraph, toIndex(x,y,z,N,M,P),
                        toIndex(x-1,y,z,N,M,P), 1);
        if(y == parkMap->M - 1)                 /* at the Top wall */
            insertEdge(cGraph, toIndex(x,y,z,N,M,P),
                        toIndex(x,y-1,z,N,M,P), 1);
        if(y == 0)                              /* at the bottom wall */
            insertEdge(cGraph, toIndex(x,y,z,N,M,P),
                        toIndex(x,y+1,z,N,M,P), 1);
    }

    /* go through access points and make new edges */
    for(i = 0; i < parkMap->E; i++) {
        x = getx(parkMap->accessPoints[i]);
        y = gety(parkMap->accessPoints[i]);
        z = getz(parkMap->accessPoints[i]);

        /* this time we connect the edge from the path to the access point
         * since the access point does not have an edge outward but inwards
         */
        if(x == 0) /* at the left wall, add path to the right of the entrance */
            insertEdge(pGraph, toIndex(x+1,y,z,N,M,P),
                        toIndex(x,y,z,N,M,P), 3);
        if(x == parkMap->N - 1)                 /* at the right wall ... */
            insertEdge(pGraph, toIndex(x-1,y,z,N,M,P),
                        toIndex(x,y,z,N,M,P), 3);
        if(y == parkMap->M - 1)                 /* at the Top wall */
            insertEdge(pGraph, toIndex(x,y-1,z,N,M,P),
                        toIndex(x,y,z,N,M,P), 3);
        if(y == 0)                              /* at the bottom wall */
            insertEdge(pGraph, toIndex(x,y+1,z,N,M,P),
                        toIndex(x,y,z,N,M,P), 3);
    }

    parkMap->cGraph = cGraph;
    parkMap->pGraph = pGraph;

    return;
}



/*
 *  Function:
 *      mapPrintStd
 *  Description:
 *      Prints the map representation in mapRep matrices
 *
 *  Arguments:
 *      Pointer to struct Map
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      Writes to stdout
 */

void mapPrintStd(Map *parkMap) {
    int n, m, p, i; /* iteration variables */
    Point *ap;

    if(!parkMap)
        return;
    
    for(p = 0; p < parkMap->P; p++) {
        for(m = parkMap->M - 1; m >= 0; m--) {
            for(n = 0; n < parkMap->N; n++)
                fprintf(stdout, "%c", parkMap->mapRep[n][m][p]);
            fprintf(stdout, "\n");
        }
        fprintf(stdout, "+\n");
    }

    for(i = 0; i < parkMap->E; i++) {
        ap = parkMap->entrancePoints[i];
        pointPrintStd(ap);
    }
    for(i = 0; i < parkMap->S; i++) {
        ap = parkMap->accessPoints[i];
        pointPrintStd(ap);
    }


    return;
}
/*
 *  Functions: (print graphs)
 *
 *  Description:
 *      prints the desired graph previously built in parkMap
 *
 *  Arguments:
 *      file stream pointer
 *      Pointer to struct Map
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      none
 */

void printCGraph(FILE *fp, Map *parkMap){
    GprintToFile(fp, parkMap->cGraph);
    return;
}

void printPGraph(FILE *fp, Map *parkMap){
    GprintToFile(fp, parkMap->pGraph);
    return;
}


/*
 *  Function:
 *      mapDestroy
 *  Description:
 *      Destroys all memory previously allocated to build map structure
 *
 *  Arguments:
 *      Pointer to struct Map
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      the pointer sent in becomes NULL
 */

void mapDestroy(Map *parkMap) {
    int i;
    int n, m;

    /* free special Points memory */
    for(i = 0; i < parkMap->S; i++)
        pointDestroy(parkMap->accessPoints[i]);
    free(parkMap->accessPoints);
    for(i = 0; i < parkMap->E; i++)
        pointDestroy(parkMap->entrancePoints[i]);
    free(parkMap->entrancePoints);

    for(n = 0; n < parkMap->N; n++) {
        for(m = 0; m < parkMap->M; m++) 
            free(parkMap->mapRep[n][m]);
        free(parkMap->mapRep[n]);
    }
    free(parkMap->mapRep);

    destroyGraph(parkMap->cGraph);
    destroyGraph(parkMap->pGraph);

    free(parkMap);
}
