#include<stdio.h>
#include"parkmap.h"
#include"point.h"

int main(int argc, char* argv[]) {
    Map *parkMap;
    Point *entrance, *access;
    
    if(argc == 1) {
        fprintf(stderr, "Usage ...\n");
        return 1;
    }
    parkMap = mapInit(argv[1]);
    
    mapPrintStd(parkMap);

    buildGraphs(parkMap);
    printGraph(stdout, parkMap);

    entrance = newPoint("E2", '-', 10, 0, 0);
    access = newPoint("A2", 'C', 2, 14, 0);

    findPath(parkMap, entrance, access);

    mapDestroy(parkMap);
    pointDestroy(entrance);
    pointDestroy(access);

    return 0;
}
