#include "physics.h"
#include "entity_list.h"
#include "checkCollisions.h"
#include "player_update.h" // Header für die State Machine

// Die globalen Zustandsvariablen wurden in die Entity-Struktur verschoben.

void handle_all_entities(){
    // Durchläuft alle Entitäten und wendet die Physik nur auf den Spieler an.
    for (int i = 0; i < MAX_ENTITIES; i++){
        if (entity_used[i] == 1 && entities[i].type == ENTITY_PLAYER){
            handle_physics(&entities[i]);
        }
    }
}

void handle_physics(Entity* e){

    if (e->type == ENTITY_PLAYER){
        // SCHRITT 1: State Machine Logik (in player_update.c)
        update_player_state_and_physics(e); 
    }

    // SCHRITT 2: Kollisionsprüfung
    check_collision(e);
}