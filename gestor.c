#include<stdio.h>
#include"parkmap.h"
#include"point.h"
#include"LinkedList.h"
#include"stdlib.h"
#include"string.h"
#include"escreve_saida.h"
#include"queue.h"

typedef struct _order{
    /* char type - describes type of order:
     * R is restrict, r is unrestrict
     * s indicates car is leaving using ID
     * S indicates car is leaving using coordinates
     * every other character indicates desired access point
     */
    char type;               
    char action;
    int x, y, z;                  /* coordinates associated with new order */
    int time;
    char *id;                   
} Order;

void OrderDestroy(Item order){
    if( order == NULL)
        return;
    if(((Order *) order)->id != NULL)
        free(((Order *)order)->id);
    free( ((Order *) order));
    return;
}
    

void Usage(){
    fprintf(stderr, "Usage:\n\t./gestor <parque.cfg> <parque.inp> [parque.res]\n");
    return;
}

/* loads instructions into simple orders time ordered list */
LinkedList *loadInstructionFile(char *inpfile){
    FILE *fp;
    char buffer[256];
    int time, x, y, z;
    char type;
    char auxChar;
    int inpRead;
    Order *order;
    LinkedList *t;

    t = initLinkedList();

    fp = fopen(inpfile, "r");
    if(fp == NULL){
        fprintf(stderr, "Couldn't open instruction file %s\n", inpfile);
        return NULL;
    }

    buffer[0] = '\0';

    auxChar = (char) fgetc(fp);
    while(auxChar != EOF){
        if(auxChar == 'V'){
            ungetc(auxChar, fp);
            inpRead = fscanf(fp, "%s %d %c %d %d %d", buffer, &time, &type,
                                                      &x, &y, &z);
            if(inpRead == 6){
                order = (Order *) malloc(sizeof(Order));
                if(type == 'S'){
                    order->action = 'S';
                    order->type = ' ';
                    order->time = time;
                    order->x = x; order->y = y; order->z = z;
                    order->id = (char *) malloc(sizeof(char) * (strlen(buffer) + 1));
                    strcpy(order->id, buffer);
                    buffer[0] = '\0';

                    t = insertUnsortedLinkedList(t, (Item) order);
                }
                else{
                    order->type = type;
                    order->action = 'E';
                    order->time = time;
                    order->x = x; order->y = y; order->z = z;
                    order->id = (char *) malloc(sizeof(char) * (strlen(buffer) + 1));
                    strcpy(order->id, buffer);
                    buffer[0] = '\0';

                    t = insertUnsortedLinkedList(t, (Item) order);
                }
            } else if(inpRead == 3){
                order = (Order *) malloc(sizeof(Order));
                order->action = 's';
                order->type = ' ';
                order->time = time;
                order->id = (char *) malloc(sizeof(char) * (strlen(buffer) + 1));
                strcpy(order->id, buffer);
                buffer[0] = '\0';
                order->x = -1; order->y = -1; order->z = -1;
                t = insertUnsortedLinkedList(t, (Item) order);
            } else{
                break;
            }
        }
        auxChar = (char) fgetc(fp);
    }
    fclose(fp);

    return t;
}

/* loads restrictions into simple orders time ordered list */
LinkedList *loadRestrictionFile(char *resfile){
    FILE *fp;
    int ta, tb, x, y, z;
    int inpRead;
    char R, auxChar;
    Order *order;
    LinkedList *t;

    t = initLinkedList();

    fp = fopen(resfile, "r");
    if(fp == NULL){
        fprintf(stderr, "Couldn't open restrictions file %s\n", resfile);
        return NULL;
    }

    /* go through restriction file and list them in reverse order */
    auxChar = (char) fgetc(fp);
    while(auxChar != EOF){
        if(auxChar == 'R'){
            ungetc(auxChar, fp);
            inpRead = fscanf(fp, "%c %d %d %d %d %d\n", &R, &ta, &tb,
                                                      &x, &y, &z);
            if(inpRead == 6){
                order = (Order *) malloc(sizeof(Order));
                order->action = 'R';
                order->type = ' ';
                order->time = ta;
                order->id = NULL;
                order->x = x; order->y = y; order->z = z;
                t = insertUnsortedLinkedList(t, (Item) order);

                if(tb >= ta){
                    order = (Order *) malloc(sizeof(Order));
                    order->action = 'r';
                    order->type = ' ';
                    order->time = tb;
                    order->id = NULL;
                    order->x = x; order->y = y; order->z = z;
                    t = insertUnsortedLinkedList(t, (Item) order);
                }
            } else if(inpRead == 4){   /* if it's a floor type restriction */
                order = (Order *) malloc(sizeof(Order));
                order->action = 'P';
                order->type = ' ';
                order->time = ta;
                order->z = x;             /* x contains floor */
                order->x = -1; order->y = -1;
                order->id = NULL;

                t = insertUnsortedLinkedList(t, (Item) order);
                if(tb >= ta){
                    order = (Order *) malloc(sizeof(Order));
                    order->action = 'p';
                    order->type = ' ';
                    order->time = tb;
                    order->z = x;        /* x contains floor */
                    order->x = -1;
                    order->y = -1;
                    order->id = NULL;
                    t = insertUnsortedLinkedList(t, (Item) order);
                }
            } else{
                break;
            }
        }
        auxChar = (char) fgetc(fp);
    }
    fclose(fp);

    return t;
}

/* function used by inpresShuffleOrder to compare times */
int compareOrderTime(Item order1, Item order2){
    if( ((Order *) order1)->time <= ((Order *) order2)->time)
        return 1;
    else
        return 0;
}

/* merges two orderedLists into one reverse ordered list */
LinkedList *inpresShuffleOrder(LinkedList *inpList, LinkedList *resList){
    return mergeOrderedLists(inpList, resList, compareOrderTime);
}


int main(int argc, char* argv[]) {
    Map *parkMap;
    LinkedList *inp, *res, *orders;
    LinkedList *t, *testT;
    int time;
    Order *o, *testO;
    int cost, *st;
    FILE *fp;
    char *ptsfilename;
    Queue *Q;
    
    if(argc < 3) {
        Usage();
        return 1;
    }

    parkMap = mapInit(argv[1]);
    if(parkMap == NULL){
        fprintf(stderr, "Error producing the park Map structure\n");
        return 2;
    }
    buildGraphs(parkMap);

    /* load instruction file into Orders reverse ordered list */
    inp = loadInstructionFile(argv[2]);
    t = inp;

   /* if restriction file is presented */
    if(argc == 4){
        /* load restriction file into Orders reverse ordered list */
        res = loadRestrictionFile(argv[3]);
        t = res;
    }
    else
        res = initLinkedList();

    /* if there is restriction file, both lists will be merged in a ordered
     * list according to time of execution
     *
     * else, if there is no restriction file this instruction will simply 
     * reverse the order in the inp list
     *
     * caution: inpresShuffleOrder frees both input lists
     */
    orders = inpresShuffleOrder(res, inp);
    t = orders;

    /* generate output file name */
    ptsfilename = (char *) malloc(sizeof(char) * (strlen(argv[1]) + 1));
    strncpy(ptsfilename, argv[1], (strlen(argv[1]) - 4));
    ptsfilename[strlen(argv[1]) - 4] = '\0';
    strcat(ptsfilename, ".ptx");

    fp = fopen(ptsfilename, "w");
    
    /* initialize car queue */
    Q = Qinit();
    t = orders;
    while(t != NULL){
        o = (Order *) getItemLinkedList(t);
        switch (o->action){
            case 'E':
                if(isQueueEmpty(Q) == 1){
                    st = findPath(parkMap, o->id, o->x, o->y, o->z, o->type, &cost);
                    if(st == NULL){
                        escreve_saida(fp, o->id, o->time, o->x, o->y, o->z, 'i');
                        Qpush(Q, (Item) o);
                    }
                    else{
                        writeOutput(fp, parkMap, st, cost, o->time, o->id, o->type);
                        free(st);
                    }
                }
                else{
                    escreve_saida(fp, o->id, o->time, o->x, o->y, o->z, 'i');
                    Qpush(Q, (Item) o);
                }
                break;
            case 'S':
                /* freeSpot */
                clearSpotCoordinates(parkMap, o->x, o->y, o->z);
                escreve_saida(fp, o->id, o->time, o->x, o->y, o->z, 's');

                /* check if there are any map restrictions/unrestrictions ahead */
                testT = getNextNodeLinkedList(t);
                if(testT != NULL){
                    testO = (Order *) getItemLinkedList(testT);
                    if( strchr("RrPpSs", (int) testO->action) != NULL){
                        /* if they are applied at the same time */
                        if( testO->time == o->time){
                            t = getNextNodeLinkedList(t);
                            /* go to next node */
                            continue;
                        }
                    }

                }
                
                /* if this is the last unrestriction for this exact time */
                if(isQueueEmpty(Q) == 0){
                    time = o->time;   /* to update order time */
                    o = (Order *) Qpop(Q);
                    st = findPath(parkMap, o->id, o->x, o->y, o->z, o->type, &cost);
                    if(st == NULL)
                        QpushFirst(Q, (Item) o);
                    else{
                        cost += time - o->time;
                        writeOutputAfterIn(fp, parkMap, st, cost, time, o->id, o->type, o->time);
                        free(st);
                    }
                }
                break;
            case 's':
                /* free spot of car with ID */
                clearSpotIDandWrite(fp, parkMap, o->id, o->time);

                /* check if there are any map restrictions/unrestrictions ahead */
                testT = getNextNodeLinkedList(t);
                if(testT != NULL){
                    testO = (Order *) getItemLinkedList(testT);
                    if( strchr("RrPpSs", (int) testO->action) != NULL){
                        /* if they are applied at the same time */
                        if( testO->time == o->time){
                            t = getNextNodeLinkedList(t);
                            /* go to next node */
                            continue;
                        }
                    }

                }

                if(isQueueEmpty(Q) == 0){
                    time = o->time;   /* to update order time */
                    o = (Order *) Qpop(Q);
                    st = findPath(parkMap, o->id, o->x, o->y, o->z, o->type, &cost);
                    if(st == NULL)
                        QpushFirst(Q, (Item) o);
                    else{
                        cost += time - o->time;
                        writeOutputAfterIn(fp, parkMap, st, cost, time, o->id, o->type, o->time);
                        free(st);
                    }
                }
                break;
            case 'R':
                restrictMapCoordinate(parkMap, o->x, o->y, o->z);
                break;
            case 'r':
                freeRestrictionMapCoordinate(parkMap, o->x, o->y, o->z);

                /* check if there are any map restrictions/unrestrictions ahead */
                testT = getNextNodeLinkedList(t);
                if(testT != NULL){
                    testO = (Order *) getItemLinkedList(testT);
                    if( strchr("RrPpSs", (int) testO->action) != NULL){
                        /* if they are applied at the same time */
                        if( testO->time == o->time){
                            t = getNextNodeLinkedList(t);
                            /* go to next node */
                            continue;
                        }
                    }

                }

                while(isQueueEmpty(Q) == 0){
                    time = o->time;   /* to update order time */
                    o = (Order *) Qpop(Q);
                    st = findPath(parkMap, o->id, o->x, o->y, o->z, o->type, &cost);
                    if(st == NULL){
                        QpushFirst(Q, (Item) o);
                        break;
                    }
                    else{
                        cost += time - o->time; /* add additional cost for waiting */
                        writeOutputAfterIn(fp, parkMap, st, cost, time, o->id, o->type, o->time);
                        free(st);
                    }
                }
                break;
            case 'P':
                restrictMapFloor(parkMap, o->z);
                break;
            case 'p':
                freeRestrictionMapFloor(parkMap, o->z);

                /* check if there are any map restrictions/unrestrictions ahead */
                testT = getNextNodeLinkedList(t);
                if(testT != NULL){
                    testO = (Order *) getItemLinkedList(testT);
                    if( strchr("RrPpSs", (int) testO->action) != NULL){
                        /* if they are applied at the same time */
                        if( testO->time == o->time){
                            t = getNextNodeLinkedList(t);
                            /* go to next node */
                            continue;
                        }
                    }

                }

                while(isQueueEmpty(Q) == 0){
                    time = o->time;   /* to update order time */
                    o = (Order *) Qpop(Q);
                    st = findPath(parkMap, o->id, o->x, o->y, o->z, o->type, &cost);
                    if(st == NULL){
                        QpushFirst(Q, (Item) o);
                        break;
                    }
                    else{
                        cost += time - o->time;
                        writeOutputAfterIn(fp, parkMap, st, cost, time, o->id, o->type, o->time);
                        free(st);
                    }
                }
                break;
            default:
                fprintf(stderr, "Unknown order action %c\n", o->action);
                break;
        }
        t = getNextNodeLinkedList(t);
    }
    
    Qdestroy(Q);
    fclose(fp);
    free(ptsfilename);
    freeLinkedList(orders, OrderDestroy);
    mapDestroy(parkMap);

    exit(0);
}
