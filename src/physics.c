#include "physics.h"
#include "entity_list.h"
#include "checkCollisions.h"

u16 state;
u16 state_old;

void handle_all_entities(){
    for (int i = 0; i < MAX_ENTITIES; i++){
        if (entity_used[i] == 1 && entities[i].type == ENTITY_PLAYER){
            handle_physics(&entities[i]);

        }
    }
 }

void handle_input(Entity* e){
    state = JOY_readJoypad(JOY_1);

    // Sprung
    if ((state & BUTTON_A) && !(state_old & BUTTON_A) && e->state == P_GROUNDED) {
        e->state = P_JUMPING;
        e->vy = -500; 
    }

    if (!(state & BUTTON_A) && (state_old & BUTTON_A) && e->vy < FIX16(0) ) {
        e->vy = 40;
    }

    // Horizontaler Schub
    if (state & BUTTON_LEFT) {
        e->vx = e->vx - 100;

        } else if (state & BUTTON_RIGHT) {

        e->vx = e->vx + 100;
    
    }

    state_old = state;

        if (e->vy < 0) 
            e->state = P_FALLING;

        if (e->vy > 0) 
            e->state = P_JUMPING;

    // Gravity  in Jump / falling
    if (e->state == P_JUMPING) {
        e->vy = e->vy + FIX16(25);  
    } else if (e->state == P_FALLING){
        e->vy = e->vy + FIX16(40); 
    }
    

    // Friction
     e->vx = e->vx * 0.7; 

    // Clamps - Max Speed
    if (e->vx > 700) e->vx = 700;
    if (e->vx < -700) e->vx = -700;
    if (e->vy > 1000) e->vy = 1000; // Max. Fallgeschwindigkeit
    
    // Clamps - too slow
    if ( e->vx < 10 && e->vx > -10) {
        e->vx = 0;
    }

    if ( e->vy < 10 && e->vy > -10) {
        if (e->state == P_GROUNDED) e->vy = 0;
    }

    // Set Position
    e->x = e->x + e->vx / 100;
    e->y = e->y + e->vy / 100;
}

void handle_physics(Entity* e){


    if (e->type == ENTITY_PLAYER){
        handle_input(e);
    }

    check_collision(e);
}
