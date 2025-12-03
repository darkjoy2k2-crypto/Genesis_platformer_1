#ifndef ENTITY_LIST_H
#define ENTITY_LIST_H

#include <genesis.h>

enum ENTITY_TYPE {
    ENTITY_PLAYER = 0
};

enum ENTITY_STATE {
    E_INACTIVE = 0,
    E_ACTIVE = 1,

    P_DEAD = 2,
    P_ALIVE = 3,
    P_GROUNDED = 4,
    P_JUMPING = 5,
    P_FALLING = 6,
    P_SAFE = 7
};

typedef struct {
    s16 x;
    s16 y;
    s16 x_old;
    s16 y_old;

    s16 vx;
    s16 vy;

    u16 width;
    u16 height;

    u16 state;
    u16 anim_index;

    Sprite* sprite;
    enum ENTITY_TYPE type;
} Entity;

#define MAX_ENTITIES 10
extern Entity entities[MAX_ENTITIES];
extern u8 entity_used[MAX_ENTITIES];

void init_entities();
int create_entity(s16 x, s16 y, u16 width, u16 height, u16 type);
void delete_entity(int id);

#endif
