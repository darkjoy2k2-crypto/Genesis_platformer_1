#include <genesis.h>
#include "checkCollisions.h"
#include "entity_list.h" // Für Entity und P_ Zustände
#include "level.h" 
#include "debug.h"

// --- Konstanten (Annahme: Für 8x8 Tiles angepasst) ---
#define TILE_SIZE_PX 8 
#define MAP_WIDTH_TILES MAP_W // 60 * 2
#define MAP_HEIGHT_TILES MAP_H // 40 * 2
#define EDGE_GRAB_FREE_HEIGHT 4 // z.B. 4 Pixel für 8x8 Tiles (Hälfte der Kachelhöhe)
#define CORNER_CORRECTION_TOLERANCE 6 
#define MAX_COLLISION_STEP (TILE_SIZE_PX / 2) // Maximaler Schritt: 4 Pixel

// ====================================================================
// HILFSFUNKTIONEN FÜR DIE TILE-PRÜFUNG
// ====================================================================

// Annahme: map_collision_data ist in level.h definiert und global zugänglich

static u16 getTileIndex(s16 world_x, s16 world_y){

    s16 tile_x = world_x / TILE_SIZE_PX;
    s16 tile_y = world_y / TILE_SIZE_PX;

    if (tile_x < 0 || tile_y < 0 || tile_x >= MAP_WIDTH_TILES || tile_y >= MAP_HEIGHT_TILES) return 1;

    // Annahme: map_collision_data ist ein eindimensionales Array der Größe MAP_WIDTH_TILES * MAP_HEIGHT_TILES
    u16 tile_index = map_collision_data[tile_y * MAP_WIDTH_TILES + tile_x];

    return tile_index;
}

static bool isTileSolid(s16 world_x, s16 world_y){
    return getTileIndex(world_x, world_y) > 0;
}

// ====================================================================
// HAUPT-KOLLISIONSFUNKTION
// ====================================================================

void check_collision(Entity* entity){

    entity->is_on_wall = false; 
    bool isOnGround = false;
    s16 desired_x = entity->x;
    s16 desired_y = entity->y;

    // Vor dem Sweep: Setze Position auf alten Wert zurück
    entity->x = entity->x_old;
    entity->y = entity->y_old;


    // ====================================================================
    // X SWEEP: Iterative Horizontalbewegung (Verhindert Tunnelling)
    // ====================================================================
    
    s16 dx = desired_x - entity->x;
    if (dx != 0)
    {
        s16 steps = (abs(dx) + MAX_COLLISION_STEP - 1) / MAX_COLLISION_STEP;
        s16 step_x = dx / steps;
        
        for (int i = 0; i < steps; i++) 
        {
            entity->x += step_x;
            
            s16 left_x_check = entity->x - (entity->width >> 1);
            s16 right_x_check = entity->x + (entity->width >> 1);
            s16 top_x_check = entity->y - (entity->height >> 1);
            s16 bottom_x_check = entity->y + (entity->height >> 1);

            // ⚠️ KORREKTUR FÜR DIE RECHTE SEITE: 1 Pixel weiter prüfen, um sicher den Tile zu treffen.
            s16 check_x;
            if (entity->vx > 0) {
                check_x = right_x_check + 1; // +1 für die Prüfung des *nächsten* Tiles
            } else {
                check_x = left_x_check; 
            }
            
            s16 check_y1 = top_x_check + 1; // Oben
            s16 check_y2 = bottom_x_check - 1; // Unten

            bool top_hit = isTileSolid(check_x, check_y1);
            bool bottom_hit = isTileSolid(check_x, check_y2);

            if (top_hit || bottom_hit)
            {
                // Kollision gefunden! Zuerst zurück zum letzten sicheren Punkt.
                entity->x -= step_x;
                
                // Strenge Wand-Definition (Deine Anforderung)
                if (top_hit && bottom_hit){
                    entity->is_on_wall = true; 
                    entity->timer_wall_exec = 0; 
                } else {
                    entity->is_on_wall = false;
                }
                
                entity->edge_grab_side = entity->vx;
                
                // KANTEN-GREIF-PRÜFUNG
                if (entity->state == P_JUMPING)
                {
                    s16 free_pixels = 0;
                    
                    // Loop, um die EDGE_GRAB_FREE_HEIGHT Pixel über dem Kopf zu prüfen.
                    for (s16 y_offset = 1; y_offset <= EDGE_GRAB_FREE_HEIGHT; y_offset++)
                    {
                        s16 check_y_grab = top_x_check + y_offset;
                        
                        if (!isTileSolid(check_x, check_y_grab))
                        {
                            free_pixels++;
                        } else {
                            break;
                        }
                    }

                    if (bottom_hit && !top_hit && free_pixels == EDGE_GRAB_FREE_HEIGHT && entity->timer_edgegrab == 0)
                    {
                        // Edge Grab erfolgreich!
                        entity->state = P_EDGE_GRAB;
                        entity->vy = 0; 
                        entity->vx = 0;
                        
                        // X-Korrektur für Edge Grab (perfekte Positionierung)
                        s16 tile_col_x = check_x / TILE_SIZE_PX;
                        if (entity->edge_grab_side > 0) {
                            // Wenn nach rechts bewegt, korrigiere an die linke Kante des Tiles (tile_col_x)
                            entity->x = (tile_col_x * TILE_SIZE_PX) - (entity->width >> 1) - 1;
                        } else {
                            // Wenn nach links bewegt, korrigiere an die rechte Kante des Tiles (tile_col_x + 1)
                            entity->x = ((tile_col_x + 1) * TILE_SIZE_PX) + (entity->width >> 1);
                        }
                        
                        return; // Verlasse die gesamte Funktion
                    }
                }
                
                // Normale Wand-Korrektur (wenn kein Edge Grab)
                if (entity->state != P_WALL_JUMP){
                    entity->vx = 0;
                }

                if (dx > 0) { // Nach rechts
                    // Korrigiere mit dem nun korrekten check_x, der den kollidierten Tile identifiziert
                    s16 tile_left = (check_x / TILE_SIZE_PX) * TILE_SIZE_PX;
                    entity->x = tile_left - (entity->width >> 1) - 1; 
                } else { // Nach links
                    s16 tile_right = ((check_x / TILE_SIZE_PX) + 1) * TILE_SIZE_PX;
                    entity->x = tile_right + (entity->width >> 1);
                }
                
                break; // Iterativen Sweep beenden
            }
        }
    } 
    
    // ====================================================================
    // Y SWEEP: Iterative Vertikalbewegung (Verhindert Tunnelling)
    // ====================================================================

    s16 dy = desired_y - entity->y; // dy basiert auf der durch X korrigierten Position
    if (dy == 0) dy = desired_y - entity->y_old; // Fallback, falls X-Sweep keinen großen Schritt machte

    if (dy != 0)
    {
        s16 steps = (abs(dy) + MAX_COLLISION_STEP - 1) / MAX_COLLISION_STEP;
        s16 step_y = dy / steps;

        for (int i = 0; i < steps; i++)
        {
            entity->y += step_y;

            s16 left_y_check = entity->x - (entity->width >> 1);
            s16 right_y_check = entity->x + (entity->width >> 1);
            s16 top_y_check = entity->y - (entity->height >> 1);
            s16 bottom_y_check = entity->y + (entity->height >> 1);

            s16 check_y = (entity->vy > 0) ? bottom_y_check : top_y_check;
            s16 check_x1 = left_y_check + 1;
            s16 check_x2 = right_y_check - 1;

            if (isTileSolid(check_x1, check_y) || isTileSolid(check_x2, check_y))
            {
                // Vertikale Kollision gefunden!
                entity->y -= step_y; // ZURÜCK zum letzten sicheren y-Punkt

                if (entity->vy > 0) // Landung
                {
                    s16 tile_top = (check_y / TILE_SIZE_PX) * TILE_SIZE_PX;
                    entity->y = tile_top - (entity->height >> 1);
                    entity->vy = 0;
                    isOnGround = true; 
                }
                else // Kopf gestoßen (Decke & Sliding)
                {
                    bool slide_occurred = false;
                    bool hit_left = isTileSolid(check_x1, check_y);
                    bool hit_right = isTileSolid(check_x2, check_y);

                    // Decken-Slide Logik (bei Kantenstoß)
                    if (hit_left != hit_right) {
                        s16 tile_col_x1 = check_x1 / TILE_SIZE_PX;
                        s16 tile_col_x2 = check_x2 / TILE_SIZE_PX;
                        s16 tile_right_x1 = (tile_col_x1 + 1) * TILE_SIZE_PX;
                        s16 tile_left_x2 = tile_col_x2 * TILE_SIZE_PX;

                        if (hit_right) { // Schiebe nach links
                            if ((right_y_check - tile_left_x2 <= CORNER_CORRECTION_TOLERANCE)) {
                                entity->x = tile_left_x2 - (entity->width >> 1) - 1; 
                                slide_occurred = true;
                            }
                        } else if (hit_left) { // Schiebe nach rechts
                            if ((tile_right_x1 - left_y_check <= CORNER_CORRECTION_TOLERANCE)) {
                                entity->x = tile_right_x1 + (entity->width >> 1) + 1; 
                                slide_occurred = true;
                            }
                        }
                    }
                    
                    // Vertikale Auflösung
                    s16 tile_bottom = ((check_y / TILE_SIZE_PX) + 1) * TILE_SIZE_PX;
                    entity->y = tile_bottom + (entity->height >> 1);
                    
                    if (!slide_occurred) {
                        entity->vy = 0;
                    }
                }
                break; // Iterativen Sweep beenden
            }
        }
    }

    // ====================================================================
    // ZUSTANDS-AKTUALISIERUNG (Bodenprüfung für nächsten Frame)
    // ====================================================================

    // Prüft 1 Pixel unter der Entität
    s16 check_y_ground = entity->y + (entity->height >> 1) + 1; 
    s16 check_x1_ground = entity->x - (entity->width >> 1) + 1;
    s16 check_x2_ground = entity->x + (entity->width >> 1) - 1;

    if (isTileSolid(check_x1_ground, check_y_ground) || isTileSolid(check_x2_ground, check_y_ground)) isOnGround = true;
    
    if (isOnGround) entity->state = P_GROUNDED;
    else if (entity->state != P_WALL_JUMP && entity->state != P_SHOT_JUMP && entity->state != P_JUMPING && entity->state != P_EDGE_GRAB) entity->state = P_FALLING;
}