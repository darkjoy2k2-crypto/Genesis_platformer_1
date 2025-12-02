#ifndef ENTITY_H
#define ENTITY_H

#include <genesis.h>

typedef struct {
    fix32 x;
    s16 x_s16;

    fix32 y;
    s16 y_s16;

    fix32 x_old;
    s16 x_old_s16;

    fix32 y_old;
    s16 y_old_s16;

    fix16 vx;
    fix16 vy;

    u16 width;
    u16 height;

    bool jumping;

    Sprite* sprite;
    f16 anim_index;
    bool isOnGround;

    u8 type;   // 0=player, 1=enemy, 2=bullet, usw.
} Entity;

#endif