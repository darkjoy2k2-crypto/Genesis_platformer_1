#include "physics.h"
#include "entity.h"

u16 state;
u16 state_old;

void prepareEntitys(Entity* e){

    for (int i = 0; i < 1; i++){
        e->x_old = e->x;
        e->y_old = e->y;
    }
}

void handle_all_entities(){
    for (int i = 1; i < entities_count + 1; i++){
    handle_physics(&Entities[i]);
    }
 }

void handle_input(Entity* e){
    state = JOY_readJoypad(JOY_1);

    // Sprung
    if ((state & BUTTON_A) && !(state_old & BUTTON_A) && e->isOnGround) {
        e->isOnGround = FALSE;
        e->vy = FIX16(-5.0); 
    }

    if (!(state & BUTTON_A) && (state_old & BUTTON_A) && e->vy < FIX16(0) ) {
        e->vy = FIX16(0.4);
    }

    // Horizontaler Schub
    if (state & BUTTON_LEFT) {
        e->vx = e->vx - FIX16(1);

        } else if (state & BUTTON_RIGHT) {

        e->vx = e->vx + FIX16(1);
    
    }

    state_old = state;

    // Gravity  in Jump / falling
    if (!e->isOnGround) {
        if (e->vy < FIX16(0)) {
            e->vy = e->vy + FIX16(0.25);  
        } else {
            e->vy = e->vy + FIX16(0.4); 
        }
    }

    // Friction
     e->vx = F16_mul(e->vx, FIX16(0.7)); 

    // Clamps - Max Speed
    if (e->vx > FIX16(7.0)) e->vx = FIX16(7.0);
    if (e->vx < FIX16(-7.0)) e->vx = FIX16(-7.0);
    if (e->vy > FIX16(10.0)) e->vy = FIX16(10.0); // Max. Fallgeschwindigkeit
    
    // Clamps - too slow
    if (F32_toRoundedInt(e->vx) == 0 && (e->vx < FIX16(0.1) && e->vx > FIX16(-0.1))) {
        e->vx = FIX16(0);
    }

    if (F16_toRoundedInt(e->vy) == 0 && (e->vy < FIX16(0.1) && e->vy > FIX16(-0.1))) {
        if (e->isOnGround) e->vy = FIX16(0);
    }

    // Set Position
    e->x = e->x + F16_toFix32(e->vx);
    e->y = e->y + F16_toFix32(e->vy);
}

void handle_physics(Entity* e){
    prepareEntitys(e);

    if (e->type == ENTITY_PLAYER){
        handle_input(e);
    }

    check_collision(e);

    e->x_s16 = F32_toRoundedInt(e->x);
    e->y_s16 = F32_toRoundedInt(e->y);

}
