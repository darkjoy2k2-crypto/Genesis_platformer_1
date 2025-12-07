#ifndef DEBUG_H
#define DEBUG_H

#include <genesis.h>

extern char info[40];
extern int debug_value[10];

void debug_draw();
void debug_set(int place, int value);

#endif

