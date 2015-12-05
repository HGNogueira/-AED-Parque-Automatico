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
#include"queue.h"
#include"htable.h"
#include"escreve_saida.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define SIZE 256
#define CHARSIZE 256
#define HASHCONSTANT 17
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
#define toCoordinateX(n,X,Y,Z) ((n)%(X))
#define toCoordinateY(n,X,Y,Z) ((n % ((X)*(Y)*(Z))) % ((X)*(Y))) / (X)
#define toCoordinateZ(n,X,Y,Z) ( (n % ((X)*(Y)*(Z))) / ((X)*(Y)) )



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
    int N, M;             /* floor dimensions NxM */
    int P;                /* num of floors */
    int E, S;             /* num of entrances (E) and peon access points (S) */
    int difS;             /* num of different type of peon access points */
    int n_spots, n_av;    /* total number of spots, number of available */

    char ***mapRep; /* table of matrices to represent multiple floor map */
    Point **accessPoints; /* table of map access points */
    Point **entrancePoints; /* table of map entrance points */

    int *accessTable; /* lookup table for accesspoints descriptors */
    LinkedList *accessTypes;  /* lookup table with acccess Type descriptors
                                 to save some sort of order */

    LinkedList **ramps; /* table to save ramp Points, index corresponds
                           to the floor */

    HashTable *pCars;            /* Hash table with information about parked
                                    cars*/

    GraphL *Graph;               /* car and pedestrian graph */
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
    int it;
    Map *parkMap; 
    int n, m, p, i;                               /* iteration variables */
    char auxChar, desc, *auxPChar;             /* auxiliary chars */
    char ID[SIZE]; /* auxiliary strings */
    int x, y, z;                               /* point coordinate values */
    int atE = 0, atA = 0;                      /* control variables for access 
                                                and entrance tables */

    parkMap = (Map*) malloc(sizeof(Map));
    check_mem(parkMap);      /* memory check debug macro */

    fp = fopen( filename, "r");
    check_file(fp);          /* file check debug macro */                   

    it = fscanf(fp, "%d %d %d %d %d\n", &parkMap->N, &parkMap->M, &parkMap->P,
                &parkMap->E, &parkMap->S);
    if(it != 5){
        fprintf(stderr, "First line of map config has wrong format\n");
        exit(1);
    }

    parkMap->n_spots = 0;
    parkMap->n_av = 0;

    /* initialize number of diferent access types to zero */
    parkMap->difS = 0;

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
            while( '\n' != fgetc(fp));  /* skip to next line in file */
        }
        
        /* read access and entrance point info between floors */
        auxChar = (char) fgetc(fp);
        while(auxChar != '+') {
            switch(auxChar) {
                case 'E':
                    ungetc(auxChar, fp);
                    it = fscanf(fp, "%s %d %d %d %c", ID, &x, &y, &z, &desc);
                    if(it != 5){
                        fprintf(stderr, "Entrance line has wrong format\n");
                        exit(1);
                    }
                    parkMap->entrancePoints[atE] = newPoint(ID, desc, x, y, z);
                    ID[0] = '\0';
                    atE++;
                    break;
                case 'A':
                    ungetc(auxChar, fp);   /* back to start of description */
                    it = fscanf(fp, "%s %d %d %d %c", ID, &x, &y, &z, &desc);
                    if(it != 5){
                        fprintf(stderr, "Access line has wrong format\n");
                        exit(1);
                    }
                    parkMap->accessPoints[atA] = newPoint(ID, desc, x, y, z);
                    ID[0] = '\0';

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
                default:
                    break;
            }
            auxChar = (char) fgetc(fp);    /* read next character from file */
        }
        while( '\n' != fgetc(fp)); /* skip to next line in file */
    }

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
        t = getNextNodeLinkedList(t);
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
        for(m = 0; m < M; m++){
            for(n = 0; n < N; n++){
                switch(parkMap->mapRep[n][m][p]){
                    case '@': break;
                    case 'u':
                        /* insert upper ramp in appropriate floor ramps list */
                        auxRamp = newPoint("Ramp", 'u', n, m, p);
                        parkMap->ramps[p] = insertUnsortedLinkedList(
                                                parkMap->ramps[p], 
                                                (Item) auxRamp);

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
                        if(n > 0){
                            if( strchr("@exa.", (int) LEFT) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n-1,m,p,N,M,P), 1);
                            } 
                            if( strchr("@xe.", (int) BOTTOM) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n-1,m,p,N,M,P) + N*M*P, 3);
                            } 
                        }
                        /* TOP */
                        if(m < M - 1){
                            if( strchr("@exa.", (int) TOP) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                        toIndex(n,m+1,p,N,M,P), 1);
                            } 
                            if( strchr("@xe.", (int) BOTTOM) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m+1,p,N,M,P) + N*M*P, 3);
                            } 
                        }
                        /* RIGHT */
                        if(n < N - 1){
                            if( strchr("@exa.", (int) RIGHT) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                        toIndex(n+1,m,p,N,M,P), 1);
                            } 
                            if( strchr("@xe.", (int) BOTTOM) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n+1,m,p,N,M,P) + N*M*P, 3);
                            } 
                        }
                        /* BOTTOM */
                        if(m > 0){
                            if( strchr("@exa.", (int) BOTTOM) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                        toIndex(n,m-1,p,N,M,P), 1);
                            } 
                            if( strchr("@xe.", (int) BOTTOM) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m-1,p,N,M,P) + N*M*P, 3);
                            } 
                        }
                          
                        /* connect car path with upper floor */
                        GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p + 1,N,M,P), 2);
                        /* connect peon path with upper floor, weight = 3 * 2 */
                        GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m,p + 1,N,M,P) + N*M*P, 6);

                        break;
                    case 'd':
                        /* insert upper ramp in appropriate floor ramps list */
                        auxRamp = newPoint("Ramp", 'd', n, m, p);
                        parkMap->ramps[p] = insertUnsortedLinkedList(
                                                parkMap->ramps[p], 
                                                (Item) auxRamp);

                        
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
                        if(n > 0){
                            if( strchr("@exa.", (int) LEFT) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                        toIndex(n-1,m,p,N,M,P), 1);
                            } 
                            if( strchr("@xe.", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n-1,m,p,N,M,P) + N*M*P, 3);
                            }
                        }
                        /* TOP */
                        if(m < M -1){
                            if( strchr("@exa.", (int) TOP) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                        toIndex(n,m+1,p,N,M,P), 1);
                            }
                            if( strchr("@xe.", (int) BOTTOM) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m+1,p,N,M,P) + N*M*P, 3);
                            } 

                        }
                        /* RIGHT */
                        if(n < N - 1){
                            if( strchr("@exa.", (int) RIGHT) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                        toIndex(n+1,m,p,N,M,P), 1);
                            }
                            if( strchr("@xe.", (int) BOTTOM) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n+1,m,p,N,M,P) + N*M*P, 3);
                            }  
                        }
                        /* BOTTOM */
                        if(m > 0){
                            if( strchr("@exa.", (int) BOTTOM) == NULL){
                                GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                        toIndex(n,m-1,p,N,M,P), 1);
                            } 
                            if( strchr("@xe.", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m-1,p,N,M,P) + N*M*P, 3);
                            }
                        }


                        /* connect car path with lower floor */
                        GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m,p - 1,N,M,P), 2);
                        /* connect peon path with lower floor, weight = 3 * 2 */
                        GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m,p - 1,N,M,P) + N*M*P, 6);

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
                        if( strchr("@ea", (int) LEFT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n-1,m,p,N,M,P), 1);
                        }
                        /*insert in peon path by adding N*M*P to index */
                        if( strchr("@xe.", (int) LEFT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n-1,m,p,N,M,P) + N*M*P, 3);
                        }
                        /* TOP */
                        if( strchr("@ea", (int) TOP) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m+1,p,N,M,P), 1);
                        } 
                        /*insert in peon path by adding N*M*P to index */
                        if( strchr("@xe.", (int) TOP) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m+1,p,N,M,P) + N*M*P, 3);
                        }
                        /* RIGHT */
                        if( strchr("@ea", (int) RIGHT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n+1,m,p,N,M,P), 1);
                        }
                        /*insert in peon path by adding N*M*P to index */
                        if( strchr("@xe.", (int) RIGHT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n+1,m,p,N,M,P) + N*M*P , 3);
                        }
                        /* BOTTOM */
                        if( strchr("@ea", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                    toIndex(n,m-1,p,N,M,P), 1);
                        } 
                        /*insert in peon path by adding N*M*P to index */
                        if( strchr("@xe.", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m-1,p,N,M,P) + N*M*P, 3);
                        }
                                               
                        break;
                    case 'x':
                        parkMap->n_av--;/* to counter-act next increment '.' */
                        GdeactivateNode(Graph, toIndex(n,m,p,N,M,P));

                        /* don't break, continue through to case '.' to add
                         * all connections. Since spot is deactivated in graph
                         * it wont be usable until activated
                         */
                    
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
                        if( strchr("@xe.", (int) LEFT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n-1,m,p,N,M,P) + N*M*P, 3);
                        }
                        /* TOP */
                        if( strchr("@xe.", (int) TOP) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m+1,p,N,M,P) + N*M*P, 3);
                        }
                        /* RIGHT */
                        if( strchr("@xe.", (int) RIGHT) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n+1,m,p,N,M,P) + N*M*P, 3);
                        }
                        /* BOTTOM */
                        if( strchr("@xe.", (int) BOTTOM) == NULL){
                            GinsertEdge(Graph, toIndex(n,m,p,N,M,P) + N*M*P,
                                    toIndex(n,m-1,p,N,M,P) + N*M*P, 3);
                        }

                        /* finally we can link the car paths and pedestrian
                         * paths together
                         *
                         * these edges have the value 0, we can think of them
                         * as leaving the car
                         */

                        GinsertEdge(Graph, toIndex(n,m,p,N,M,P),
                                toIndex(n,m,p,N,M,P) + N*M*P, 0);
                        break;

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
    /* initializing hastable with m = n_spots and p = 17 (prime number) */
    parkMap->pCars = HTinit(parkMap->n_spots, 17);  

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

int getAccessTypeNode(Map *parkMap, char desc){
    return parkMap->accessTable[ (int) desc];
}

void writeOutput(FILE *fp, Map *parkMap, int *st, int cost, int time, char *ID, char accessType){
    int *path;
    int i, pathSize, dest, j;
    int N, M, P;
    int TIME[3];             /* array to save important times */
    int test = 0;            /* to test if at least 1 m or p is printed */

    TIME[0] = time;

    N = parkMap->N;
    M = parkMap->M;
    P = parkMap->P;

    dest = parkMap->accessTable[ (int) accessType];

    /* find size of path vector */
    for(i = st[ dest ], pathSize = 0; st[i] != -1; i = st[i], pathSize++);
    pathSize++;

    /* fill path vector with passby nodes
     * starts at the access point node and goes back until entrace is reached
     */
    path = (int *) malloc(sizeof(int) * (pathSize));
    for(j = pathSize - 1, i = st[dest]; j >= 0; j--, i = st[i]){
        path[j] = i; 
    }

    /* write entering output */
    escreve_saida(fp, ID, time, toCoordinateX(path[0], N, M, P),
                                toCoordinateY(path[0], N, M, P),
                                toCoordinateZ(path[0], N, M, P),
                                'i');
    /* go through all nodes between the first and parking node */
    for(j = 1; path[j + 1] - path[j] != N*M*P; j++){
        time++;
        /* check if car is going through a ramp and add time if so */
        if((path[j] - path[j - 1]) == N*M || (path[j - 1] - path[j]) == N*M)
            time++;
        /* check if node in index j corresponds to a turn in the path
         * if so this is an important point to consider and we must 
         * write it out
         */
        if(path[j] - path[j - 1] != path[j + 1] - path[j]){
            test = 1;
            escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                        toCoordinateY(path[j], N, M, P),
                                        toCoordinateZ(path[j], N, M, P), 'm');
        }
    }
    if(test == 0){
        escreve_saida(fp, ID, TIME[0] + 1, toCoordinateX(path[1], N, M, P),
                                toCoordinateY(path[1], N, M, P),
                                toCoordinateZ(path[1], N, M, P), 'm');
    }
    time++;
    TIME[1] = time;
    /* car has just parked */
    escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                toCoordinateY(path[j], N, M, P),
                                toCoordinateZ(path[j], N, M, P), 'e');

    test = 0;
    /* now start from first peon node and go until you reach the access */
    for(j += 2 ; j < pathSize - 1; j++){
        time++;
        /* check if peon is going through a ramp and add time if so */
        if(  (path[j + 1] - path[j] || path[j] - path[j + 1]) == N*M)
            time++;
        /* check if node in index j corresponds to a turn in the path
         * if so this is an important point to consider and we must 
         * write it out
         */
        if(path[j] - path[j - 1] != path[j + 1] - path[j]){
            test = 1;
            escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                        toCoordinateY(path[j], N, M, P),
                                        toCoordinateZ(path[j], N, M, P), 'p');
        }
    }
    if(test == 0){
        j = pathSize - 2;
        escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                toCoordinateY(path[j], N, M, P),
                                toCoordinateZ(path[j], N, M, P), 'p');
    }
    /* peon has reached the access point
     * we may write it to the output file
     */
    time++; /* add one extra tick to arrive to the access */
    TIME[2] = time;
    j = pathSize - 1;/* go to the last index of the path list */
    escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                toCoordinateY(path[j], N, M, P),
                                toCoordinateZ(path[j], N, M, P), 'a');
    /* write terminating line */
    escreve_saida(fp, ID, TIME[0], TIME[1], TIME[2], cost, 'x');

    free(path);
    return;
}

void writeOutputAfterIn(FILE *fp, Map *parkMap, int *st, int cost, int time, 
        char *ID, char accessType, int origTime){
    int *path;
    int i, pathSize, dest, j;
    int N, M, P;
    int TIME[3];             /* array to save important times */
    int test = 0;            /* to test if at least 1 m or p is printed */

    TIME[0] = origTime;

    N = parkMap->N;
    M = parkMap->M;
    P = parkMap->P;

    dest = parkMap->accessTable[ (int) accessType];

    /* find size of path vector */
    for(i = st[ dest ], pathSize = 0; st[i] != -1; i = st[i], pathSize++);
    pathSize++;

    /* fill path vector with passby nodes
     * starts at the access point node and goes back until entrace is reached
     */
    path = (int *) malloc(sizeof(int) * (pathSize));
    for(j = pathSize - 1, i = st[dest]; j >= 0; j--, i = st[i]){
        path[j] = i; 
    }

    /* go through all nodes between the first and parking node */
    for(j = 1; path[j + 1] - path[j] != N*M*P; j++){
        time++;
        /* check if car is going through a ramp and add time if so */
        if((path[j] - path[j - 1]) == N*M || (path[j - 1] - path[j]) == N*M)
            time++;
        /* check if node in index j corresponds to a turn in the path
         * if so this is an important point to consider and we must 
         * write it out
         */
        if(path[j] - path[j - 1] != path[j + 1] - path[j]){
            test = 1;
            escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                        toCoordinateY(path[j], N, M, P),
                                        toCoordinateZ(path[j], N, M, P), 'm');
        }
    }
    if(test == 0){
        escreve_saida(fp, ID, TIME[0] + 1, toCoordinateX(path[1], N, M, P),
                                toCoordinateY(path[1], N, M, P),
                                toCoordinateZ(path[1], N, M, P), 'm');
    }
    time++;
    TIME[1] = time;
    /* car has just parked */
    escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                toCoordinateY(path[j], N, M, P),
                                toCoordinateZ(path[j], N, M, P), 'e');

    test = 0;
    /* now start from first peon node and go until you reach the access */
    for(j += 2 ; j < pathSize - 1; j++){
        time++;
        /* check if peon is going through a ramp and add time if so */
        if(  (path[j + 1] - path[j] || path[j] - path[j + 1]) == N*M)
            time++;
        /* check if node in index j corresponds to a turn in the path
         * if so this is an important point to consider and we must 
         * write it out
         */
        if(path[j] - path[j - 1] != path[j + 1] - path[j]){
            test = 1;
            escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                        toCoordinateY(path[j], N, M, P),
                                        toCoordinateZ(path[j], N, M, P), 'p');
        }
    }
    if(test == 0){
        j = pathSize - 2;
        escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                toCoordinateY(path[j], N, M, P),
                                toCoordinateZ(path[j], N, M, P), 'p');
    }
    /* peon has reached the access point
     * we may write it to the output file
     */
    time++; /* add one extra tick to arrive to the access */
    TIME[2] = time;
    j = pathSize - 1;/* go to the last index of the path list */
    escreve_saida(fp, ID, time, toCoordinateX(path[j], N, M, P),
                                toCoordinateY(path[j], N, M, P),
                                toCoordinateZ(path[j], N, M, P), 'a');
    /* write terminating line */
    escreve_saida(fp, ID, TIME[0], TIME[1], TIME[2], cost, 'x');

    free(path);
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
 *      clearSpotCoordinates
 *      clearSpotIDandWrite
 *
 *  Description:
 *      Frees parking spot coordinates
 *      using the spot coordinates (clearSpotCoordinates)
 *      using the car ID (clearSpotIdandWrite)
 *
 *      In the 2nd case, the program also writes to the output
 *
 *  Arguments:
 *      Map *parkmap - map configuration
 *      int x, y, z - parking spot's coordinates (clearSpotCoordinates)
 *      char *ID - car identifier (clearSpotID)
 *      
 *  Return value:
 *      void
 *
 *  Secondary effects:
 *      parking spot in graph becomes active again
 */

void clearSpotCoordinates(Map *parkMap, int x, int y, int z){
    GactivateNode(parkMap->Graph, toIndex(x, y, z, parkMap->N, parkMap->M
                                                 , parkMap->P));
    parkMap->n_av++;
    return;
}

void clearSpotIDandWrite(FILE *fp, Map *parkMap, char *ID, int time){
    int node = HTget(parkMap->pCars, ID);
    int N, M, P;
    N = parkMap->N;
    M = parkMap->M;
    P = parkMap->P;

    GactivateNode(parkMap->Graph, node);
    escreve_saida(fp, ID, time, toCoordinateX(node, N, M, P),
                                toCoordinateY(node, N, M, P),
                                toCoordinateZ(node, N, M, P),
                                's');
    return;
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
 *      char *ID - car ID 
 *      int ex, ey, ez - entrance coordinates
 *      char accessType - descriptor character of access type
 *      int *cost - reference integer to save cost of path
 *  Return value:
 *      int *
 *
 *  Secondary effects:
 *      none
 */

int *findPath(Map *parkMap, char *ID, int ex, int ey, int ez, char accessType, int *cost) {
    int origin, dest; /* origin and destiny indexed variables */
    int *st, *wt;     /* path and weight tables */
    PrioQ *PQ;        /* priority queue */
    int i;     

    /* get the path table by calculating ideal path from
     * entrance to access points
     *
     * entrance node is at index: x*M +N*m + N*M*p
     * and
     * access node is at index:   x*M +N*m + N*M*p + N*M*P
     */
    origin = toIndex(ex, ey, ez, parkMap->N, parkMap->M, parkMap->P);
    dest = parkMap->accessTable[(int) accessType];
    if(dest == -1) {
        fprintf(stderr, "There's no access with that type\n");
        exit(1);
    }


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
    *cost = GDijkstra(parkMap->Graph, origin, dest, st, wt, PQ);

    /* if no path is encountered, return NULL pointer */
    if(st[dest] == -1){
        free(wt);
        PQdestroy(PQ);
        free(st);
        return NULL;
    }

    /* occupy parking spot */
    for(i = st[dest]; i != -1; i = st[i]){
        if( i - st[i] == parkMap->N * parkMap->M * parkMap->P){
            i = st[i];
            GdeactivateNode(parkMap->Graph, i);
            HTinsert(parkMap->pCars, i, ID);
            parkMap->n_av--;
            break;
        }
    }

    free(wt);
    PQdestroy(PQ);

    return st;
}


 /*
 *  Function:
 *      restrictMapCoordinate
 *  Description:
 *      breaks edges from a certain location in a map thus becoming unusable 
 *  when calculatin ideal path
 *
 *  Arguments:
 *      Pointer to struct Map
 *      int x, y, z - location coordinates
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      changes the internal graph of Map structure sent as argument by
 *  deactivating respective coordinate node
 */

void restrictMapCoordinate(Map *parkMap, int x, int y, int z){
    int N, M, P;

    if(parkMap->Graph == NULL){
        fprintf(stderr, "Graph hasn't been built yet\n");
        return;
    }

    N = parkMap->N;
    M = parkMap->M;
    P = parkMap->P;

    /* if restricting a parking spot, decrease num of available spots */
    if(parkMap->mapRep[x][y][z] == '.')
        parkMap->n_av--;

    /* deactive car path node */
    GdeactivateNode(parkMap->Graph, toIndex(x, y, z, N, M, P));

    /* and deactivate peon path node */
    GdeactivateNode(parkMap->Graph, toIndex(x, y, z, N, M, P) + N*M*P);

    return;
}


 /*
 *  Function:
 *      freeRestrictionMapCoordinate
 *  Description:
 *      activates the respective node in internal graph making it usable in 
 *  the calculation of the ideal path again
 *
 *  Arguments:
 *      Pointer to struct Map
 *      int x, y, z - location coordinates
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      changes the internal graph of Map structure sent as argument by
 *  activating the respective coordinate node
 */

void freeRestrictionMapCoordinate(Map *parkMap, int x, int y, int z){
    int N, M, P;

    if(parkMap->Graph == NULL){
        fprintf(stderr, "Graph hasn't been built yet\n");
        return;
    }

    N = parkMap->N;
    M = parkMap->M;
    P = parkMap->P;

    /* if freeing a parking spot, increase num of available spots */
    if(parkMap->mapRep[x][y][z] == '.')
        parkMap->n_av++;

    /* activate car path node */
    GactivateNode(parkMap->Graph, toIndex(x, y, z, N, M, P));

    /* and activate peon path node */
    GactivateNode(parkMap->Graph, toIndex(x, y, z, N, M, P) + N*M*P);
    return;
}


/*
 *  Function:
 *      restrictMapFloor
 *  Description:
 *      restricts a whole floor in map configuration for usage
 *
 *  Arguments:
 *      Pointer to struct Map
 *      int floor - index of the floor to free
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      changes the internal graph of Map structure sent as argument by 
 *  deactivating fundamental floor nodes
 */

void restrictMapFloor(Map *parkMap, int floor){
    Point *t;
    LinkedList *floorRamps;
    int i;
    int x, y, z;
    int N, M;

    if(parkMap->Graph == NULL){
        fprintf(stderr, "Graph hasn't been built yet\n");
        return;
    }

    N = parkMap->N;
    M = parkMap->M;

    /* close all entrances to the floor */
    for(i = 0; i < parkMap->E; i++){
        if(getz(parkMap->entrancePoints[i]) == floor){
            x = getx(parkMap->entrancePoints[i]);
            y = gety(parkMap->entrancePoints[i]);
            z = floor;
            GdeactivateNode(parkMap->Graph, toIndex(x, y, z, N, M, P));
        }
    }

    /* go through all the ramps located in that floor and close them */
    floorRamps = parkMap->ramps[floor];
    while( (t = (Point *) getItemLinkedList(floorRamps)) != NULL){
        x = getx(t);
        y = gety(t);
        z = floor;
        GdeactivateNode(parkMap->Graph, toIndex(x, y, z, N, M, P));
    }
    return;
}


 /*
 *  Function:
 *      freeRestrictionMapFloor
 *  Description:
 *      frees previously applied restricion on floor
 *
 *  Arguments:
 *      Pointer to struct Map
 *      int floor - index of the floor to free
 *  Return value:
 *      none
 *
 *  Secondary effects:
 *      changes the internal graph of Map structure sent as argument through
 *  activation of fundamental floor nodes
 */

void freeRestrictionMapFloor(Map *parkMap, int floor){
    Point *t;
    LinkedList *floorRamps;
    int i;
    int x, y, z;
    int N, M;

    if(parkMap->Graph == NULL){
        fprintf(stderr, "Graph hasn't been built yet\n");
        return;
    }

    N = parkMap->N;
    M = parkMap->M;

    /* reopen all entrances to the floor */
    for(i = 0; i < parkMap->E; i++){
        if(getz(parkMap->entrancePoints[i]) == floor){
            x = getx(parkMap->entrancePoints[i]);
            y = gety(parkMap->entrancePoints[i]);
            z = floor;
            GactivateNode(parkMap->Graph, toIndex(x, y, z, N, M, P));
        }
    }

    /* go through all the ramps located in that floor and reopen them */
    floorRamps = parkMap->ramps[floor];
    while( (t = (Point *) getItemLinkedList(floorRamps)) != NULL){
        x = getx(t);
        y = gety(t);
        z = floor;
        GactivateNode(parkMap->Graph, toIndex(x, y, z, N, M, P));
    }
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
        pointDestroy((Item) parkMap->accessPoints[i]);
    free(parkMap->accessPoints);
    for(i = 0; i < parkMap->E; i++)
        pointDestroy((Item) parkMap->entrancePoints[i]);
    free(parkMap->entrancePoints);

    /* deallocate map representation matrices */
    if(parkMap->mapRep != NULL){
        for(n = 0; n < parkMap->N; n++) {
            for(m = 0; m < parkMap->M; m++) 
                free(parkMap->mapRep[n][m]);
            free(parkMap->mapRep[n]);
        }
        free(parkMap->mapRep);
    }

    if(parkMap->ramps != NULL){
        for(i = 0; i < parkMap->P; i++)
            freeLinkedList(parkMap->ramps[i], pointDestroy);
        free(parkMap->ramps);
    }

    if(parkMap->Graph != NULL)
        Gdestroy(parkMap->Graph);

    freeLinkedList(parkMap->accessTypes, free);
    free(parkMap->accessTable);

    if(parkMap->pCars != NULL)
        HTdestroy(parkMap->pCars);

    free(parkMap);
}
