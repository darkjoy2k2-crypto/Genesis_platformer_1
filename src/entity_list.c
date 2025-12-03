#include "entity_list.h"
#include "genesis.h"
#include "title.h"


#define MAX_ENTITIES 10

Entity entities[MAX_ENTITIES];
u8 entity_used[MAX_ENTITIES];

void init_entities() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        entity_used[i] = 0;
    }
}

int create_entity(s16 x, s16 y, u16 width, u16 height, u16 type) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!entity_used[i]) {
            
            entity_used[i] = 1;
            Entity* e = &entities[i];
            e->type = type;
            e->x = e->x_old = x;
            e->y = e->y_old = y;
            e->anim_index = e->vx = e->vy = 0;
            e->width = width;
            e->height = height;
            e->state = E_INACTIVE;
            e->sprite = SPR_addSprite(&player_sprite, x, y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
            return i;
        }
    }
    return -1; // kein Platz mehr
}

void delete_entity(int id) {
    if (id < 0 || id >= MAX_ENTITIES) return;
    entity_used[id] = 0;
}