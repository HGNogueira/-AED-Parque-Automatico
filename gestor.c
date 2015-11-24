#include<stdio.h>
#include"parkmap.h"
#include"point.h"

void Usage(){
    fprintf(stderr, "Usage:\n\t./gestor <parque.cfg> <entranceID> <accessDesc>\n");
    return;
}

int main(int argc, char* argv[]) {
    Map *parkMap;
    
    if(argc < 4) {
        Usage();
        return 1;
    }
    parkMap = mapInit(argv[1]);
    
    mapPrintStd(parkMap);

    buildGraphs(parkMap);

    findPath(parkMap, argv[2], argv[3][0]);

    mapDestroy(parkMap);

    return 0;
}
