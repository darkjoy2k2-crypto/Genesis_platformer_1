#ifndef ENTITY_LIST_H
#define ENTITY_LIST_H

#include <genesis.h>

enum ENTITY_TYPE {
    ENTITY_PLAYER = 0
};

enum ENTITY_STATE {
    E_INACTIVE = 0,
    E_ACTIVE = 1,
    P_IDLE = 2,
    P_DEAD = 3,
    P_ALIVE = 4,
    P_GROUNDED = 5,
    P_JUMPING = 6,
    P_FALLING = 7,
    P_RUNNING = 8,
    P_EDGE_GRAB = 9,
    P_SAFE = 10
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
    u16 state_old;
    u16 state_old_joy;
    
    s16 anim_index;

    s16 timer_grace;
    s16 timer_buffer;
    s16 timer_edgegrab;

    Sprite* sprite;
    enum ENTITY_TYPE type;

    bool is_on_wall;
    s16 edge_grab_side;
} Entity;

#define MAX_ENTITIES 10
extern Entity entities[MAX_ENTITIES];
extern u8 entity_used[MAX_ENTITIES];

void init_entities();
int create_entity(s16 x, s16 y, u16 width, u16 height, u16 type);
void delete_entity(int id);

#endif
