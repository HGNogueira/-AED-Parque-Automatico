#include<stdio.h>
#include"parkmap.h"
#include"point.h"
#include"LinkedList.h"
#include"stdlib.h"
#include"string.h"

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

void Usage(){
    fprintf(stderr, "Usage:\n\t./gestor <parque.cfg> <entranceID> <accessDesc>\n");
    return;
}

/* loads instructions into simple orders time ordered list */
LinkedList *loadInstructionFile(char *inpfile){
    FILE *fp;
    char buffer[256];
    int time, x, y, z;
    char type;
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

    while(1){
        inpRead = fscanf(fp, "%s %d %c %d %d %d\n", buffer, &time, &type,
                                                  &x, &y, &z);
        if(inpRead == 6){
            order = (Order *) malloc(sizeof(Order));
            if(type == 'S'){
                order->action = 'S';
                order->type = ' ';
            }
            else{
                order->type = type;
                order->action = 'E';
            }
            order->time = time;
            order->x = x; order->y = y; order->z = z;
            order->id = (char *) malloc(sizeof(char) * (strlen(buffer) + 1));
            strcpy(order->id, buffer);
            buffer[0] = '\0';
            t = insertUnsortedLinkedList(t, (Item) order);
        } else if(inpRead == 3){
            order = (Order *) malloc(sizeof(Order));
            order->action = 's';
            order->time = time;
            order->id = (char *) malloc(sizeof(char) * (strlen(buffer) + 1));
            buffer[0] = '\0';
            order->x = -1; order->y = -1; order->z = -1;
            t = insertUnsortedLinkedList(t, (Item) order);
        } else{
            break;
        }
    }
    fclose(fp);

    return t;
}

/* loads restrictions into simple orders time ordered list */
LinkedList *loadRestrictionFile(char *resfile){
    FILE *fp;
    int ta, tb, x, y, z;
    int inpRead;
    char R;
    Order *order;
    LinkedList *t;

    t = initLinkedList();

    fp = fopen(resfile, "r");
    if(fp == NULL){
        fprintf(stderr, "Couldn't open restrictions file %s\n", resfile);
        return NULL;
    }
    while(1){
        inpRead = fscanf(fp, "%c %d %d %d %d %d\n", &R, &ta, &tb,
                                                  &x, &y, &z);
        if(inpRead == 6){
            order = (Order *) malloc(sizeof(Order));
            order->action = 'R';
            order->type = ' ';
            order->time = ta;
            order->x = x; order->y = y; order->z = z;
            t = insertUnsortedLinkedList(t, (Item) order);

            if(tb != 0){
                order = (Order *) malloc(sizeof(Order));
                order->type = 'r';
                order->time = tb;
                order->x = x; order->y = y; order->z = z;
                t = insertUnsortedLinkedList(t, (Item) order);
            }
        } else if(inpRead == 4){
            order = (Order *) malloc(sizeof(Order));
            order->action = 'P';
            order->type = ' ';
            order->time = ta;
            order->z = x;             /* x contains floor */
            order->x = -1; order->y = -1;

            t = insertUnsortedLinkedList(t, (Item) order);
            if(tb != 0){
                order = (Order *) malloc(sizeof(Order));
                order->action = 'p';
                order->type = ' ';
                order->time = tb;
                order->z = x;        /* x contains floor */
                order->x = -1;
                order->y = -1;
                t = insertUnsortedLinkedList(t, (Item) order);
            }
        } else{
            break;
        }
    }
    fclose(fp);

    return t;
}

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
    LinkedList *t;
    Order *o;
    
    if(argc < 4) {
        Usage();
        return 1;
    }

    parkMap = mapInit(argv[1]);
    if(parkMap == NULL){
        fprintf(stderr, "Error producing the park Map structure\n");
        return 2;
    }
    mapPrintStd(parkMap);
    buildGraphs(parkMap);

    inp = loadInstructionFile(argv[2]);
    res = loadRestrictionFile(argv[3]);

    t = inp;
    fprintf(stdout, "Inputs\n");
    while(t != NULL){
        o = (Order *) getItemLinkedList(t);
        fprintf(stdout, "<%s> <%c> <%c> <%d> <%d,%d,%d>\n", o->id, o->type,
                                            o->action, o->time, o->x, o->y, 
                                            o->z);
        t = getNextNodeLinkedList(t);
    }

    t = res;
    fprintf(stdout, "Restrictions\n");
    while(t != NULL){
        o = (Order *) getItemLinkedList(t);
        fprintf(stdout, "<%s> <%c> <%c> <%d> <%d,%d,%d>\n", o->id, o->type,
                                            o->action, o->time, o->x, o->y, 
                                            o->z);
        t = getNextNodeLinkedList(t);
    }

    orders = inpresShuffleOrder(res, inp);
    t = orders;
    fprintf(stdout, "Shuffle\n");
    while(t != NULL){
        o = (Order *) getItemLinkedList(t);
        fprintf(stdout, "<%s> <%c> <%c> <%d> <%d,%d,%d>\n", o->id, o->type,
                                            o->action, o->time, o->x, o->y, 
                                            o->z);
        t = getNextNodeLinkedList(t);
    }




    /*
    findPath(parkMap, argv[2], argv[3][0]);

    if(argc == 5)
        loadRestrictions(parkMap, argv[4]);
    */

    mapDestroy(parkMap);

    return 0;
}
