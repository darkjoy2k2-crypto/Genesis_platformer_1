#include <genesis.h>
#include "entity_list.h" // KORREKTUR: Alle Entity-Definitionen kommen von hier
#include "player_update.h" 
#include "debug.h"

// ====================================================================
// 1. PHYSIK-KONSTANTEN (ANGEPASST AN IHRE VORGABEN)
// ====================================================================

#define JUMP_VELOCITY_START 480
#define JUMP_VELOCITY_START_GC 480
#define EDGE_JUMP_START     250
#define HORIZONTAL_ACCEL    90 // REDUZIERT
#define RUN_SPEED_MAX       650 // REDUZIERT
#define FALL_SPEED_MAX      1000
#define GRAVITY_JUMP        20
#define GRAVITY_FALL        40
#define GROUND_FRICTION     0.7 
#define GRACE_PERIOD_FRAMES 5  


// ====================================================================
// 2. STATE HANDLER
// ====================================================================

static void handle_idle_running_logic(Entity* player, u16 joy_state);
static void handle_jumping_falling_logic(Entity* player, u16 joy_state);
static void handle_edge_grab_logic(Entity* player, u16 joy_state);


/**
 * @brief Die Haupt-Update-Funktion des Players (State Machine).
 * Definiert die gesamte Steuerlogik.
 *
 * @param player Pointer auf die Player-Entity.
 */
void update_player_state_and_physics(Entity* player) {

    u16 joy_state = JOY_readJoypad(JOY_1);
    
    // --- 1. GLOBALE INPUT- UND TIMER-VERARBEITUNG (Coyote Time, Jump Buffer) ---
    player->x_old = player->x;
    player->y_old = player->y;
    
    // Timer-Updates
    if (player->timer_grace > 0) player->timer_grace--;
    if (player->timer_buffer > 0) player->timer_buffer--;
    if (player->timer_edgegrab > 0) player->timer_edgegrab--;
    
    debug_set(3, player->vy);
    debug_set(4, player->state);

    switch (player->state) {
        case P_IDLE:
        case P_RUNNING:
        case P_GROUNDED:
            buffered_jump(player);
            joy_check_directions(player, joy_state);
            check_normal_jump(player, joy_state);
            check_grace_jump(player);

            player->vx *= GROUND_FRICTION;
            if (player->vy > 0) player->state = P_FALLING;
            break;

        case P_JUMPING:

            joy_check_directions(player, joy_state);
            player->vy += GRAVITY_JUMP; 
            if (!(joy_state & BUTTON_A)){
                player->vy = 40; 
            }
            if (player->vy > 0) player->state = P_FALLING;
            player->vx *= GROUND_FRICTION;
            break;

        case P_FALLING:
            joy_check_directions(player, joy_state);
            joy_check_falling(player, joy_state);
            grace_jump(player);
            player->vy += GRAVITY_FALL;
            player->vx *= GROUND_FRICTION;

            break;
            
        case P_EDGE_GRAB:

            joy_check_grab_directions(player, joy_state);
            player->state = P_FALLING;
            break;
            
        default:
            break;
    }

    if (player->vx > RUN_SPEED_MAX) player->vx = RUN_SPEED_MAX;
    if (player->vx < -RUN_SPEED_MAX) player->vx = -RUN_SPEED_MAX;
    if (player->vy > FALL_SPEED_MAX) player->vy = FALL_SPEED_MAX;
   
    player->state_old_joy = (joy_state & BUTTON_A); 
    player->state_old = player->state;

    player->x += player->vx / 100;
    player->y += player->vy / 100;
}

// ... Implementierung der static handle_*-Funktionen ...
static void handle_idle_running_logic(Entity* player, u16 joy_state) {
    if (player->state == P_GROUNDED) {
        if (joy_state & (BUTTON_LEFT | BUTTON_RIGHT)) {
            player->state = P_RUNNING;
        } else {
            player->state = P_IDLE;
        }
        return;
    }
    if (player->state == P_IDLE && (joy_state & (BUTTON_LEFT | BUTTON_RIGHT))) {
        player->state = P_RUNNING;
    }
    if (player->state == P_RUNNING && !(joy_state & (BUTTON_LEFT | BUTTON_RIGHT))) {
        player->state = P_IDLE;
    }
}


static void handle_jumping_falling_logic(Entity* player, u16 joy_state) {
    if (player->state == P_JUMPING && player->vy >= 0) {
        player->state = P_FALLING;
    }
}

void buffered_jump(Entity* player){
    if (player->state == P_GROUNDED && player->state_old == P_FALLING && player->timer_buffer > 0)
    {
        player->state = P_JUMPING;
        player->vy = -JUMP_VELOCITY_START; 
        player->timer_buffer = 0;
        VDP_drawText("BUFFERED JUMP", 0, 1);
    }
}

void grace_jump(Entity* player){
    if (player->state_old == P_GROUNDED && player->state == P_FALLING)
    {
        player->timer_grace = GRACE_PERIOD_FRAMES;
    }
}

void joy_check_directions(Entity* player, u16 joy_state){
    if (joy_state & BUTTON_LEFT) {
        player->vx -= HORIZONTAL_ACCEL;
    } else if (joy_state & BUTTON_RIGHT) {
        player->vx += HORIZONTAL_ACCEL;
    }
}

void joy_check_falling(Entity* player, u16 joy_state){
    if (joy_state & BUTTON_LEFT) {
        player->vx -= HORIZONTAL_ACCEL;
    } else if (joy_state & BUTTON_RIGHT) {
        player->vx += HORIZONTAL_ACCEL;
    }
    if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A)){
        if (player->timer_grace > 0){
            VDP_drawText("GRACE JUMP", 0, 1);
            jump(player);
        } 
        player->timer_buffer = GRACE_PERIOD_FRAMES;
    }
 

}

void joy_check_grab_directions(Entity* player, u16 joy_state){
        edge_jump(player, joy_state);

}

void check_grace_jump(Entity* player){if (player->timer_grace > 0) jump(player); }

void check_normal_jump(Entity* player, u16 joy_state){
    if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A))
        jump(player);
}
    

void edge_jump(Entity* player, u16 joy_state){

    player->vy -= EDGE_JUMP_START;
    player->state = P_FALLING;
}

void jump(Entity* player){
    player->timer_edgegrab = 5;
    player->state = P_JUMPING;
    player->timer_buffer = 0;
    player->vy = -JUMP_VELOCITY_START; 
}