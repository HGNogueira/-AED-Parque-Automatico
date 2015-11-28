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
#define CHARSIZE 256
    /* toIndex - macro to convert coordinates into an 1 dimensional index
     *
     * a - column
     * b - row
     * c - floor
     * A - num of columns
     * B - num of rows 
     * C - num of floors
     */
#define toIndex(a,b,c,A,B,C) a + (A)*(b) + (A)*(B)*(c) 


/*
 *  Internal Data Type: Restriction
 *
 *  Description: 
 *      Characterizes a certain restriction given by the client, this can be 
 *  applied on a whole floor or just a certain coordinate
 *
 */

typedef struct _restriction{
    int ta;
    int tb;
    int floor;             /* indicates restricted floor, -1 if not applied */
    int x, y, z;           /* restricted coordinates, uninitialized if 
                            * not applied */

} Restriction;


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
    int difS; /* num of different type of peon access points */
    int n_spots, n_av; /* total number of spots, number of available */

    char ***mapRep; /* table of matrices to represent multiple floor map */
    Point **accessPoints; /* table of map access points */
    Point **entrancePoints; /* table of map entrance points */

    int *accessTable; /* lookup table for accesspoints descriptors */
    LinkedList *accessTypes;  /* lookup table with acccess Type descriptors
                                 to save some sort of order */

    LinkedList **ramps; /* table to save ramp Points, index corresponds
                           to the floor */

    int R;              /* number of restrictions */
    Restriction **rcts; /* table of Restriction pointers, appliccable if needed*/

    GraphL *Graph; /* car and pedestrian graph */
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
    int n, m, p, i;                               /* iteration variables */
    char auxChar, desc, *auxPChar;             /* auxiliary chars */
    char ID[SIZE], *auxString, skipLine[SIZE]; /* auxiliary strings */
    int x, y, z;                               /* point coordinate values */
    int atE = 0, atA = 0;                      /* control variables for access 
                                                and entrance tables */

    parkMap = (Map*) malloc(sizeof(Map));
    check_mem(parkMap);      /* memory check debug macro */

    fp = fopen( filename, "r");
    check_file(fp);          /* file check debug macro */                   

    fscanf(fp, "%d %d %d %d %d", &parkMap->N, &parkMap->M, &parkMap->P,
                &parkMap->E, &parkMap->S);

    parkMap->n_spots = 0;
    parkMap->n_av = 0;

    /* initialize number of diferent access types to zero */
    parkMap->difS = 0;

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

    /* initialize auxiliar accessPoint variables
     * accessTable has size CHARSIZE for it must be able to house any index
     * in (int) char
     * 
     * it is initialized to -1 to check if it has been accessed (usefull further
     * down when trying to figure out number of diferent types)
     */
    parkMap->accessTable = (int *) malloc(sizeof(int) * CHARSIZE);
    for(i = 0; i < CHARSIZE; i++)
        parkMap->accessTable[i] = -1;
    parkMap->accessTypes = initLinkedList();
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
                    free(auxString);
                    atE++;
                    break;
                case 'A':
                    ungetc(auxChar, fp);   /* back to start of description */
                    fscanf(fp, "%s %d %d %d %c", ID, &x, &y, &z, &desc);
                    auxString = strdup(ID);
                    ID[0] = '\0';
                    parkMap->accessPoints[atA] = newPoint(auxString, desc, x, y, z);
                    free(auxString);

                    /* check if first time in Lookup table */
                    if(parkMap->accessTable[ (int) desc ] == -1) {
                        parkMap->difS++;
                        auxPChar = (char *) malloc(sizeof(char));
                        *auxPChar= desc;
                        parkMap->accessTypes =  insertUnsortedLinkedList(
                                                parkMap->accessTypes,
                                                (Item) auxPChar);
                        parkMap->accessTable[ (int) desc ] = 0;
                    }

                    atA++;
                    break;
            }
            auxChar = (char) fgetc(fp);    /* read next character from file */
        }
        fgets(skipLine, SIZE, fp); /* make file stream point to next line) */
        skipLine[0] = '\0';
    }


    parkMap->R = 0;                 /* initialize parkMap restrictions to 
                                       non-existent */

    fclose(fp);

    return parkMap;
}               

/*
 *  Function:
 *      generateGraphs
 *  Description:
 *      generates pedestrian and car graph with list representation
 *
 *  Arguments:
 *      Pointer to struct Map
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      initializes and computes both the car and pedestrian directed
 *      weighted graph
 */

void buildGraphs(Map *parkMap) {
    int n, m, p, i;    /* iteration variables */
    int N, M ,P;
    int x, y, z;       /* point coordinates */
    Point *auxRamp, *auxAccess;
    GraphL *Graph; /* adjacency list graphs */
    char* auxPChar;
    int gSize;
    LinkedList *t;

    N = parkMap->N;
    M = parkMap->M;
    P = parkMap->P;

    /* initializing the graphs
     * the total number of nodes is twice the number of points in
     * a map configuration
     *
     * this is simply because the graph results in the concatenation
     * of 2 graph: one in the car, and one in peon mode
     *
     * this way, from 0 to N - 1, the adjacency list table will be regarding 
     * the car situation and from N to 2*N -1 regarding the peon situation
     *
     * this simpliflies the passage from one situation to the other later on
     *
     * it is easy to see that the only way to pass from one situation to the 
     * other is by going over a '.' point
     *
     * --------------------------
     *
     *  Last parkMap->difS spots are specially reserved for access point types
     */
    gSize = N * M * P * 2 + parkMap->difS;
    Graph = Ginit(gSize);

    /* load values into parkMap->accessTable */
    t = parkMap->accessTypes;
    for(i = gSize - parkMap->difS; i < gSize; i++) {
        auxPChar = (char*) getItemLinkedList(t);
        parkMap->accessTable[(int) *auxPChar] = i;
    }

    /* initialize ramps table in parkMap */
    parkMap->ramps = (LinkedList**) malloc(sizeof(LinkedList*) * P);
    for(i = 0; i < P; i++)
        parkMap->ramps[i] = initLinkedList();
    
    /* start computing using the representation matrix
     *
     * n will go from 1 to N - 2 to ignore walls
     * m will go from 1 to M - 2 to ignore walls
     *
     * later we will go over the entrance points
     * to complete the graph
     * 
     * wont need to go over access points because they are just incident nodes
     */

    /* useful macros to get neighbour chars */
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
                        parkMap->n_spots++;
                        break;
                    case 'u':
                        /* insert upper ramp in appropriate floor ramps list */
                        auxRamp = newPoint("Ramp", 'u', n, m, p);
                        parkMap->ramps[p] = insertUnsortedLinkedList(
                                                parkMap->ramps[p], 
                                                (Item) auxRamp);

                        /* connect car path with upper floor */
                        GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p + 1,N,M,P), 1);

                        /* check for possibility of edge with neighbours */
                        /* 
                         * only eligible for cars
                         *
                         * the edge values are of 1 when counting for the car 
                         * path
                         *
                         * check for all directions
                         */

                        /* strchr function from <string.h> will be
                         * used in order to check if a character 
                         * belongs in a set of forbidden chars
                         */

                        /* LEFT */
                        if( strchr("@exa", (int) LEFT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n-1,m,p,N,M,P), 1);
                        } 

                        /* RIGHT */
                        if( strchr("@exa", (int) RIGHT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n+1,m,p,N,M,P), 1);
                        } 

                        /* TOP */
                        if( strchr("@exa", (int) TOP) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m+1,p,N,M,P), 1);
                        } 

                        /* BOTTOM */
                        if( strchr("@exa", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m-1,p,N,M,P), 1);
                        } 
                        break;
                    case 'd':
                        /* insert upper ramp in appropriate floor ramps list */
                        auxRamp = newPoint("Ramp", 'd', n, m, p);
                        parkMap->ramps[p] = insertUnsortedLinkedList(
                                                parkMap->ramps[p], 
                                                (Item) auxRamp);

                        /* connect car path with lower floor */
                        GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p - 1,N,M,P), 1);

                        /* check for possibility of edge with neighbours */
                        /* 
                         * only eligible for cars
                         *
                         * the edge values are of 1 when counting towards the
                         * car path
                         *
                         * check for all directions
                         */

                        /* strchr function from <string.h> will be
                         * used in order to check if a character 
                         * belongs in a set of forbidden chars
                         */

                        /* LEFT */
                        if( strchr("@exa", (int) LEFT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n-1,m,p,N,M,P), 1);
                        } 

                        /* RIGHT */
                        if( strchr("@exa", (int) RIGHT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n+1,m,p,N,M,P), 1);
                        } 

                        /* TOP */
                        if( strchr("@exa", (int) TOP) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m+1,p,N,M,P), 1);
                        } 

                        /* BOTTOM */
                        if( strchr("@exa", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m-1,p,N,M,P), 1);
                        } 
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
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n-1,m,p,N,M,P), 1);
                        }
                        /*insert in peon path by adding N*M*P to index */
                        if( strchr("@xei.ud", (int) LEFT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n-1,m,p,N,M,P) + N*M*P, 3);
                        }

                        /* RIGHT */
                        if( strchr("@exa", (int) RIGHT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n+1,m,p,N,M,P), 1);
                        } 
                        /*insert in peon path by adding N*M*P to index */
                        if( strchr("@xei.ud", (int) RIGHT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n+1,m,p,N,M,P) + N*M*P , 3);
                        }

                        /* TOP */
                        if( strchr("@exa", (int) TOP) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m+1,p,N,M,P), 1);
                        } 
                        /*insert in peon path by adding N*M*P to index */
                        if( strchr("@xei.ud", (int) TOP) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m+1,p,N,M,P) + N*M*P, 3);
                        }

                        /* BOTTOM */
                        if( strchr("@exa", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m-1,p,N,M,P), 1);
                        } 
                        /*insert in peon path by adding N*M*P to index */
                        if( strchr("@xei.ud", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m-1,p,N,M,P) + N*M*P, 3);
                        }
                        break;
                    case '.':
                        /* increase in number of available spots */
                        parkMap->n_spots++;
                        parkMap->n_av++;

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
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n-1,m,p,N,M,P) + N*M*P, 3);
                        }

                        /* RIGHT */
                        if( strchr("@xei.ud", (int) RIGHT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n+1,m,p,N,M,P) + N*M*P, 3);
                        }

                        /* TOP */
                        if( strchr("@xei.ud", (int) TOP) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m+1,p,N,M,P) + N*M*P, 3);
                        }

                        /* BOTTOM */
                        if( strchr("@xei.ud", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m-1,p,N,M,P) + N*M*P, 3);
                        }

                        /* finally we can link the car paths and pedestrian
                         * paths together
                         *
                         * these edges have the value 0, we can think of them
                         * as leaving the car
                         */
                        /* LEFT */
                        if( strchr("@xei.ud", (int) LEFT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p,N,M,P) + N*M*P, 0);
                        }

                        /* RIGHT */
                        if( strchr("@xei.ud", (int) RIGHT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p,N,M,P) + N*M*P, 0);
                        }

                        /* TOP */
                        if( strchr("@xei.ud", (int) TOP) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p,N,M,P) + N*M*P, 0);
                        }

                        /* BOTTOM */
                        if( strchr("@xei.ud", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p,N,M,P) + N*M*P, 0);

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
            GinsertEdge(Graph, toIndex(x,y,z,N,M,P),
                        toIndex(x+1,y,z,N,M,P), 1);
        if(x == parkMap->N - 1)                 /* at the right wall ... */
            GinsertEdge(Graph, toIndex(x,y,z,N,M,P),
                        toIndex(x-1,y,z,N,M,P), 1);
        if(y == parkMap->M - 1)                 /* at the Top wall */
            GinsertEdge(Graph, toIndex(x,y,z,N,M,P),
                        toIndex(x,y-1,z,N,M,P), 1);
        if(y == 0)                              /* at the bottom wall */
            GinsertEdge(Graph, toIndex(x,y,z,N,M,P),
                        toIndex(x,y+1,z,N,M,P), 1);
    }

    /* connect each of the access points to the its special type node */
    for(i = 0; i < parkMap->S; i++) {
        auxAccess = parkMap->accessPoints[i];
        /* inserting edge on node correspondent to the access point 
         * towards the general type node as given per parkMap->accessTable
         * lookup table
         * */
        GinsertEdge(Graph,
                    toIndex(getx(auxAccess), gety(auxAccess), getz(auxAccess),
                        N, M, P) + N * M * P,
                    parkMap->accessTable[(int) getDesc(auxAccess) ], 0);
    }


    parkMap->Graph = Graph;

    #undef LEFT
    #undef RIGHT
    #undef BOTTOM
    #undef TOP

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
 *  Functions: 
 *      printGraph
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

void printGraph(FILE *fp, Map *parkMap){
    int i = 0;
    LinkedList *auxLink;
    Point *auxPoint;
    Gprint(fp, parkMap->Graph);

    for(i = 0; i < parkMap->P; i++) {
        auxLink = parkMap->ramps[i];
        while(auxLink != NULL) {
            auxPoint = (Point *) getItemLinkedList(auxLink);
            fprintf(fp, "%c at (%d, %d, %d)\n", 
                    getDesc(auxPoint),
                    getx(auxPoint),
                    gety(auxPoint),
                    getz(auxPoint));
            auxLink = getNextNodeLinkedList(auxLink);
        }
    }

    return;
}


/*
 *  Functions: 
 *      getAccessPoints
 *
 *  Description:
 *      returns table of access points encountered in a Map configuration 
 *      with a certain description
 *
 *  Arguments:
 *      Map *parkmap - map configuration
 *      char desc    - descriptive character of access point type
 *      int *size    - reference to external variable with table size
 *  Return value:
 *      Point ** - Point * table
 *
 *  Secondary effects:
 *      none
 */

Point **getAccessPoints(Map *parkMap, char desc, int *size){
    Point **pointTable;
    int i;

    *size = 0;

    for(i = 0; i < parkMap->S; i++)
        if( getDesc(parkMap->accessPoints[i]) == desc )
            (*size)++;

    pointTable = (Point**) malloc(sizeof(Point *));

    for(i = 0; i < *size; i++) {
        if( getDesc(parkMap->accessPoints[i]) == desc){
            pointTable[i] = newPoint(getID(parkMap->accessPoints[i]),
                                    desc, getx(parkMap->accessPoints[i]),
                                    gety(parkMap->accessPoints[i]),
                                    getz(parkMap->accessPoints[i]));
        }
    }
    
    return pointTable;
}


/*
 *  Functions: 
 *      findPath
 *
 *  Description:
 *      finds a path between two points and returns total cost
 *
 *  Arguments:
 *      Map *parkmap - map configuration
 *      Point *entrance - entrance point
 *      Point *access   - access point
 *  Return value:
 *      int
 *
 *  Secondary effects:
 *      none
 */

int findPath(Map *parkMap, char *entranceID, char accessType) {
    int origin, dest; /* origin and destiny indexed variables */
    int cost;         /* cost of total path */
    int *st, *wt;     /* path and weight tables */
    PrioQ *PQ;        /* priority queue */
    int i;     
    Point *entrance;
    int success = 0;

    /* find the entrance Point correspondant with the id given */
    for(i = 0; i < parkMap->E; i++) {
        entrance = parkMap->entrancePoints[i];
        if(strcmp(entranceID, getID(entrance)) == 0){
            success = 1;
            break;
        }
    }
    if(success == 0){
        fprintf(stderr, "There is no entrance with the name %s\n", entranceID);
        exit(1);
    }

    /* get the path table by calculating ideal path from
     * entrance to access points
     *
     * entrance node is at index: x*M +N*m + N*M*p
     * and
     * access node is at index:   x*M +N*m + N*M*p + N*M*P
     */
    origin = toIndex(getx(entrance),
                    gety(entrance),
                    getz(entrance), parkMap->N, parkMap->M, parkMap->P);
    dest = parkMap->accessTable[(int) accessType];


    /* pre-Initialize weight and path tables, posterior function requirement */
    st = (int*) malloc(sizeof(int) * Gnodes(parkMap->Graph));
    wt = (int*) malloc(sizeof(int) * Gnodes(parkMap->Graph));

    /* load desired values onto tables */
    for(i = 0; i < Gnodes(parkMap->Graph); i++) {
        st[i] = -1;
        wt[i] = NOCON;
    }

    /* initialize priority queue, posterior function requirement */
    PQ = PQinit(wt, Gnodes(parkMap->Graph));

    /* set origin definitions and update PQ */
    st[origin] = -1;
    wt[origin] = 0;
    PQupdateIndex(PQ, origin);


    /* calculate Ideal path and get total cost */
    cost = GDijkstra(parkMap->Graph, origin, dest, st, wt, PQ);

    i = dest;
    while(st[i] != -1) {
        fprintf(stdout, "%d ", st[i]);
        i = st[i];
    }
    fprintf(stdout, "\n");

    free(st);
    free(wt);
    PQdestroy(PQ);

    return cost;
}


 /*
 *  Function:
 *      loadRestrictions
 *  Description:
 *      loads Resctriction vector of Map structure with all restrictions as per
 *      indicated in the file
 *
 *  Arguments:
 *      Pointer to struct Map
 *      char *filename - name of the restriction input file
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      the pointer sent in becomes NULL
 */

void loadRestrictions(Map *parkMap, char *filename) {
    FILE *fp;
    Restriction **rcts;
    int i;             
    int floor, x, y, z, ta, tb; /* restriction parameters */
    char rc;
    char buffer[SIZE];          /* dummy string */
    int inpRead;
    int numRes = 0;

    fp = fopen(filename, "r");
    if(fp == NULL){
        fprintf(stderr, "Couldn't open restrictions file %s\n", filename);
        exit(1);
    }

    /* counting number of lines || counting number of restrictions */
    for(numRes = 0; fgets(buffer, SIZE, fp); numRes++)
        if(buffer[0] != 'R')
            break;
    rewind(fp);                          /* return to beggining of file */

    if(numRes == 0)
        return;

    rcts = (Restriction **) malloc(sizeof(Restriction) * numRes);

    for(i = 0; i < numRes; i++) {
        inpRead = fscanf(fp, "%c %d %d %d %d %d\n", &rc, &ta, &tb, &x, &y, &z);
        if(inpRead == 6){
            rcts[i] = (Restriction *) malloc(sizeof(Restriction));
            rcts[i]->floor = -1;
            rcts[i]->x = x;
            rcts[i]->y = y;
            rcts[i]->z = z;
            rcts[i]->ta = ta;
            rcts[i]->tb = tb;
        }
        else if(inpRead == 4){
            floor = x;         /* variable x holds floor number */
            rcts[i] = (Restriction *) malloc(sizeof(Restriction));
            rcts[i]->floor = floor; 
            rcts[i]->ta = ta;
            rcts[i]->tb = tb;
            /* coordinates will remain uninitialized */
        }
        else{
            fprintf(stderr, "Can't process this restriction file\n");
            exit(1);
        }
    }
    for(i = 0; i < numRes; i++){
        if(rcts[i]->floor == -1)
            fprintf(stdout, "R %d %d %d %d %d\n", rcts[i]->ta, rcts[i]->tb,
                                        rcts[i]->x, rcts[i]->y, rcts[i]->z);
        else
            fprintf(stdout, "R %d %d %d\n", rcts[i]->ta, rcts[i]->tb,
                                          rcts[i]->floor);
    }
    fclose(fp);

    parkMap->R = numRes;
    parkMap->rcts = rcts;
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
        pointDestroy((Item) parkMap->accessPoints[i]);
    free(parkMap->accessPoints);
    for(i = 0; i < parkMap->E; i++)
        pointDestroy((Item) parkMap->entrancePoints[i]);
    free(parkMap->entrancePoints);

    for(n = 0; n < parkMap->N; n++) {
        for(m = 0; m < parkMap->M; m++) 
            free(parkMap->mapRep[n][m]);
        free(parkMap->mapRep[n]);
    }
    free(parkMap->mapRep);

    if(parkMap->ramps != NULL){
        for(i = 0; i < parkMap->P; i++)
            freeLinkedList(parkMap->ramps[i], pointDestroy);
    }
    free(parkMap->ramps);

    if(parkMap->Graph)
        Gdestroy(parkMap->Graph);

    freeLinkedList(parkMap->accessTypes, free);
    free(parkMap->accessTable);

    if(parkMap->R > 0){
        for(i = 0; i < parkMap->R; i++)
            free(parkMap->rcts[i]);
        free(parkMap->rcts);
    }

    free(parkMap);
}
