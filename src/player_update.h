#ifndef PLAYER_UPDATE_H
#define PLAYER_UPDATE_H

#include "entity_list.h" // Stellt die Entity-Struktur (Daten) bereit

// Deklariert die Hauptfunktion der State Machine.
// Diese Funktion wird in player_update.c definiert und von physics.c aufgerufen.
void update_player_state_and_physics(Entity* player);

#endif // PLAYER_UPDATE_H