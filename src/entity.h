#ifndef ENTITY_H
#define ENTITY_H

#include <genesis.h>
#define MAX_ENTITIES 20

extern s8 entities_count;

enum ENTITY_TYPE{
    ENTITY_PLAYER
};

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
    u8 anim_index;
    bool isOnGround;

    enum ENTITY_TYPE type;   // 0=player, 1=enemy, 2=bullet, usw.
} Entity;

extern Entity Entities[MAX_ENTITIES];

void init_entities();
void add_entity()

#endif