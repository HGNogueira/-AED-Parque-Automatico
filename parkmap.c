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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define IDSIZE 64



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
    int n, m, p; /* iteration variables */
    char auxChar;
    char ID[256], desc, *auxString;
    int x, y, z; /* point coordinate values */
    int atE = 0, atA = 0; /* control variables for access and entrance tables */


    parkMap = (Map*) malloc(sizeof(Map));
    check_mem(parkMap);      /* memory check debug macro */

    fp = fopen( filename, "r");
    check_file(fp);          /* file check debug macro */                   

    fscanf(fp, "%d %d %d %d %d\n", &parkMap->N, &parkMap->M, &parkMap->P,
                &parkMap->E, &parkMap->S);

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
                auxChar = (char) fgetc(fp);
                while(auxChar == '\n' || auxChar == '\r')
                    auxChar = (char) fgetc(fp);
                parkMap->mapRep[n][m][p] = auxChar;
            }
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
                    fscanf(fp, "%s %d %d %d %c\n", ID, &x, &y, &z, &desc);
                    auxString = strdup(ID);
                    ID[0] = '\0';
                    parkMap->accessPoints[atA] = newPoint(auxString, desc, x, y, z);
                    atA++;
                    break;
            }
            
            auxChar = (char) fgetc(fp);

        }
        
    }

    return parkMap;
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
    int n, m, p; /* iteration variables */
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

    for(n = 0; n < parkMap->E; n++) {
        ap = parkMap->entrancePoints[n];
        pointPrintStd(ap);
    }
    for(n = 0; n < parkMap->S; n++) {
        ap = parkMap->accessPoints[n];
        pointPrintStd(ap);
    }


    return;
}
