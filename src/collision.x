#ifndef COLLISION_H
#define COLLISION_H

#include <genesis.h>
#include "entity.h"

// Debug: store tile indices of the four corners checked
extern u16 debug_corner_tiles[4];

bool collideTop(Entity* e, u16 row, u16 y_offset);
bool collideBottom(Entity* e, u16 row, u16 y_offset);
bool collideLeft(Entity* e, u16 col, u16 row);
bool collideRight(Entity* e, u16 col, u16 row);

bool collideSlopeTop(Entity* e, u16 col, u16 row, fix32 slope, u16 y_offset);

void handleCurve(Entity* e, u16 col, u16 row);
void resolveTile(Entity* e, u16 tileType, u16 col, u16 row);
void checkCollisionAt(int corner, Entity* e, u16 x, u16 y);
void handleCollisions(Entity* e);

#endif