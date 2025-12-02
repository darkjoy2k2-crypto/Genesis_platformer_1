#ifndef LEVEL_H
#define LEVEL_H

#include <genesis.h>

// Globale Level-Konstanten
// --- KONSTANTEN ---
#define TILE_SIZE 16
#define MAP_W 60
#define MAP_H 40  
#define cam_min_x 102
#define cam_max_x 203
#define cam_min_y 65
#define cam_max_y 176

// Die Map selbst (extern, damit nur eine Instanz existiert)
extern const u16 map_data[MAP_W * MAP_H];
extern const fix32 OFFSET;
#endif