#include <genesis.h>
#include "entity_list.h" // KORREKTUR: Alle Entity-Definitionen kommen von hier
#include "player_update.h" 

// ====================================================================
// 1. PHYSIK-KONSTANTEN (ANGEPASST AN IHRE VORGABEN)
// ====================================================================

#define JUMP_VELOCITY_START 500
#define HORIZONTAL_ACCEL    90 // REDUZIERT
#define RUN_SPEED_MAX       650 // REDUZIERT
#define FALL_SPEED_MAX      1000
#define GRAVITY_JUMP        25
#define GRAVITY_FALL        40
#define GROUND_FRICTION     0.7 
#define GRACE_PERIOD_FRAMES 7  


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
    
    // Sprung-Buffer-Prüfung bei Landung 
    if (player->state == P_GROUNDED && player->state_old == P_FALLING && player->timer_buffer > 0)
    {
        player->state = P_JUMPING;
        player->vy = -JUMP_VELOCITY_START; 
        player->timer_buffer = 0;
        VDP_drawText("BUFFERED JUMP", 0, 1);
    }
    
    // Grace-Period-Aktivierung bei Kantenfall
    if (player->state_old == P_GROUNDED && player->state == P_FALLING)
    {
        player->timer_grace = GRACE_PERIOD_FRAMES;
    }
    
    // --- 2. JUMP-INPUT ---
    if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A))
    {
        if (player->state == P_GROUNDED || player->timer_grace > 0) 
        {
            player->state = P_JUMPING;
            player->timer_buffer = 0;
            player->vy = -JUMP_VELOCITY_START; 
        } else {
            player->timer_buffer = GRACE_PERIOD_FRAMES; 
        }
    }

    // --- 3. ZUSTANDSVERARBEITUNG (State Machine Switch) ---
    switch (player->state) {
        case P_IDLE:
        case P_RUNNING:
        case P_GROUNDED: 
            handle_idle_running_logic(player, joy_state);
            break;

        case P_JUMPING:
        case P_FALLING:
            handle_jumping_falling_logic(player, joy_state);
            break;
            
        case P_EDGE_GRAB:
            handle_edge_grab_logic(player, joy_state);
            break;
            
        default:
            if (player->state == P_GROUNDED) {
                player->state = P_IDLE;
            }
            break;
    }

    // --- 4. ALLGEMEINE PHYSIK ANWENDEN ---

    // A. Horizontaler Schub
    if (player->state != P_EDGE_GRAB)
    {
        if (joy_state & BUTTON_LEFT) {
            player->vx -= HORIZONTAL_ACCEL;
        } else if (joy_state & BUTTON_RIGHT) {
            player->vx += HORIZONTAL_ACCEL;
        }
    }
    

            player->vx *= GROUND_FRICTION;



    // C. Gravity 
    if (player->state == P_JUMPING) {
        if (!(joy_state & BUTTON_A) && (player->state_old_joy & BUTTON_A) && player->vy < 0) {
            player->vy = 40; 
        }
        player->vy += GRAVITY_JUMP; 
        
    } else if (player->state == P_FALLING) { // EDGE_GRAB HIER ENTFERNT!
        player->vy += GRAVITY_FALL; 
    }
    
    // D. Geschwindigkeitsbegrenzungen (Clamps)
    if (player->vx > RUN_SPEED_MAX) player->vx = RUN_SPEED_MAX;
    if (player->vx < -RUN_SPEED_MAX) player->vx = -RUN_SPEED_MAX;
    if (player->vy > FALL_SPEED_MAX) player->vy = FALL_SPEED_MAX;

    // --- 5. POSITION AKTUALISIEREN & STATE FÜR NÄCHSTEN FRAME SPEICHERN ---
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
static void handle_edge_grab_logic(Entity* player, u16 joy_state) {
    if (joy_state & BUTTON_B) {
        player->state = P_FALLING;
        if (joy_state & BUTTON_LEFT) player->vx = -100; 
        else if (joy_state & BUTTON_RIGHT) player->vx = 100;
        return;
    }
    player->vy = 0;
}