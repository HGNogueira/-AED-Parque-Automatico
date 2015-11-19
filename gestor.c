#include<stdio.h>
#include"parkmap.h"

int main(int argc, char* argv[]) {
    Map *parkMap;
    if(argc == 1) {
        fprintf(stderr, "Usage ...\n");
        return 1;
    }
    parkMap = mapInit(argv[1]);
    
    mapPrintStd(parkMap);

    mapDestroy(parkMap);

    return 0;
}
