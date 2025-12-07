#include "debug.h"
#include "genesis.h"

char info[40];
int debug_value[10];

void debug_set(int place, int value){
    debug_value[place] = value;
}

void debug_draw(){
    sprintf(info, "%d %d %d %d %d ",
        debug_value[0], 
        debug_value[1], 
        debug_value[2],
        debug_value[3],
        debug_value[4]);

    VDP_drawText(info, 0, 0);
}