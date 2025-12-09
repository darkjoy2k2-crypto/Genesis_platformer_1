#include <genesis.h>
#include "entity_list.h" // KORREKTUR: Alle Entity-Definitionen kommen von hier
#include "player_update.h" 
#include "debug.h"

// ====================================================================
// 1. PHYSIK-KONSTANTEN (ANGEPASST AN IHRE VORGABEN)
// ====================================================================

#define JUMP_VELOCITY_START 480
#define JUMP_VELOCITY_START_GC 680
#define EDGE_JUMP_START     250
#define HORIZONTAL_ACCEL    90 // REDUZIERT
#define RUN_SPEED_MAX       650 // REDUZIERT
#define FALL_SPEED_MAX      1000
#define GRAVITY_JUMP        20
#define GRAVITY_FALL        40
#define GROUND_FRICTION     0.7 
#define GRACE_PERIOD_FRAMES 5  


// ====================================================================
// 2. FORWARD DECLARATIONS
// ====================================================================

static void buffered_jump(Entity* player);
static void grace_jump(Entity* player);
static void joy_check_directions(Entity* player, u16 joy_state);
static void joy_check_falling(Entity* player, u16 joy_state);
static void joy_check_grab_directions(Entity* player, u16 joy_state);
static void check_grace_jump(Entity* player);
static void check_normal_jump(Entity* player, u16 joy_state);
static void edge_jump(Entity* player, u16 joy_state);
static void jump(Entity* player);
static void check_is_on_wall(Entity* player);
static void exec_wall_jump(Entity* player, u16 joy_state);

/**
 * @brief Die Haupt-Update-Funktion des Players (State Machine).
 * Definiert die gesamte Steuerlogik.
 *
 * @param player Pointer auf die Player-Entity.
 */
static void buffered_jump(Entity* player){
    if (player->state == P_GROUNDED && player->state_old == P_FALLING && player->timer_buffer > 0)
    {
        player->state = P_JUMPING;
        player->vy = -JUMP_VELOCITY_START; 
        player->timer_buffer = 0;
        VDP_drawText("BUFFERED JUMP", 0, 1);
    }
}

static void grace_jump(Entity* player){
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
    player->timer_edgegrab = 10;
    player->state = P_JUMPING;
    player->timer_buffer = 0;
    player->vy = -JUMP_VELOCITY_START; 
}

void check_is_on_wall(Entity* player){
    if (player->timer_edgegrab == 0 
    && player->is_on_wall
    && player->timer_wall_exec == 0 
    && player->state_old == P_JUMPING )
    {
        player->state = P_WALL_JUMP;
                VDP_drawText("WALL JUMP",0,1);

        player->timer_wall = 20;
    }        
}

void exec_wall_jump(Entity* player, u16 joy_state)
{  
    VDP_drawText("WAIT EXEC WALL JUMP",0,1);

    if ((joy_state & BUTTON_UP))
        player->y -=1;
    if ((joy_state & BUTTON_DOWN) )
        player->y +=1;


    if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A)
    && player->timer_edgegrab == 0)
    {


    if ((joy_state & BUTTON_LEFT))
                player->vx = -200;
    if ((joy_state & BUTTON_RIGHT))
                player->vx = 200;
        
            player->vy = -JUMP_VELOCITY_START;
                VDP_drawText("EXEC WALL JUMP",0,1);

            player->is_on_wall = false;
            player->timer_wall_exec++;
            

        //player->timer_wall = 20;

    }
}

void check_for_shot(Entity* player, u16 joy_state){

    if ((joy_state & BUTTON_B) && !(player->state_old_joy & BUTTON_B)
    && player->timer_shot_jump == 0)
    
    {
            player->vx = 0;
            player->vy = 0;
            player->timer_shot_jump = 40;
            player->state = P_SHOT_JUMP;
    }

    if (player->timer_shot_jump < 25 && player->vx == 0 && player->vy == 0){

        if ((joy_state & BUTTON_UP) && (joy_state & BUTTON_LEFT) ){
            player->vx = -282;
            player->vy = -282;
        } else

        if ((joy_state & BUTTON_UP) && (joy_state & BUTTON_RIGHT) ){
            player->vx = 282;
            player->vy = -282;
        }else
        if (!(joy_state & BUTTON_UP) && (joy_state & BUTTON_RIGHT) ){
            player->vx = 400;
            player->vy = 0;
        }else

        if (!(joy_state & BUTTON_UP) && (joy_state & BUTTON_LEFT) ){
            player->vx = -400;
            player->vy = 0;


            
        }else 
        if ((joy_state & BUTTON_DOWN) && (joy_state & BUTTON_LEFT) ){
            player->vx = -282;
            player->vy = 282;
        } else

        if ((joy_state & BUTTON_DOWN) && (joy_state & BUTTON_RIGHT) ){
            player->vx = 282;
            player->vy = 282;
        }else


        if ((joy_state & BUTTON_UP)){
            player->vx = 0;
            player->vy = -400;
        }else{
            player->state = P_FALLING;
            player->timer_shot_jump = 0;
        }


    }

}


void update_player_state_and_physics(Entity* player) {

    u16 joy_state = JOY_readJoypad(JOY_1);
    
    // --- 1. GLOBALE INPUT- UND TIMER-VERARBEITUNG (Coyote Time, Jump Buffer) ---
    player->x_old = player->x;
    player->y_old = player->y;
    
    // Timer-Updates
    if (player->timer_grace > 0) player->timer_grace--;
    if (player->timer_buffer > 0) player->timer_buffer--;
    if (player->timer_edgegrab > 0) player->timer_edgegrab--;
    if (player->timer_wall > 0) player->timer_wall--;
    if (player->timer_shot_jump > 0) player->timer_shot_jump--;
    

    debug_set(3, player->vx);
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
            check_for_shot(player, joy_state);

            check_is_on_wall(player);
            break;

        case P_FALLING:
                    check_is_on_wall(player);

            joy_check_directions(player, joy_state);
            joy_check_falling(player, joy_state);
            grace_jump(player);
            player->vy += GRAVITY_FALL;
            player->vx *= GROUND_FRICTION;
            //VDP_drawText("FALLING           ",0,1);
            check_for_shot(player, joy_state);
            break;
        case P_WALL_JUMP:
            if (player->timer_wall_exec == 0){
            player->vy = 60;
            player->vx = 0;
            } 

            exec_wall_jump(player, joy_state);
            player->vy += GRAVITY_JUMP;
            //player->vx *= GROUND_FRICTION;

            if (player->timer_wall == 0) 
                player->state = P_JUMPING;;
            break;    
        case P_EDGE_GRAB:

            joy_check_grab_directions(player, joy_state);
            player->state = P_FALLING;
            break;
        case P_SHOT_JUMP:
            check_for_shot(player, joy_state);
            if (player->timer_shot_jump == 0){
                player->state = P_FALLING;
            }
            break;
        default:
            break;
    }

    if (player->vx > RUN_SPEED_MAX) player->vx = RUN_SPEED_MAX;
    if (player->vx < -RUN_SPEED_MAX) player->vx = -RUN_SPEED_MAX;
    if (player->vy > FALL_SPEED_MAX) player->vy = FALL_SPEED_MAX;
   
    player->state_old_joy = joy_state;
    
    player->state_old = player->state;

    player->x += player->vx / 100;
    player->y += player->vy / 100;
}

