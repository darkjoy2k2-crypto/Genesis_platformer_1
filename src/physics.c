#include "physics.h"
#include "entity.h"

void prepareEntitys(Entity* e){

    for (int i = 0; i < 1; i++){
        e->x_old = e->x;
        e->y_old = e->y;
    }
}

void handle_physics(){
    for (int i = 0; i < entities_count; i++)
    prepareEntitys(&Entities[i]);
}
