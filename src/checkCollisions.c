#include <genesis.h>
#include "checkCollisions.h"
#include "entity_list.h"
#include "level.h" 
#include "debug.h"

#define TILE_SIZE_PX 16
#define MAP_WIDTH_TILES 60
#define MAP_HEIGHT_TILES 40
#define EDGE_GRAB_FREE_HEIGHT 8
// NEU: Toleranz für die horizontale Korrektur beim Deckenstoß
#define CORNER_CORRECTION_TOLERANCE 6  

static u16 getTileIndex(s16 world_x, s16 world_y){

    s16 tile_x = world_x / TILE_SIZE_PX;
    s16 tile_y = world_y / TILE_SIZE_PX;

    if (tile_x < 0 || tile_y < 0 || tile_x >= MAP_WIDTH_TILES || tile_y >= MAP_HEIGHT_TILES) return 0;

    u16 tile_index = map_collision_data[tile_y * MAP_WIDTH_TILES + tile_x];

    return tile_index;
}

static bool isTileSolid(s16 world_x, s16 world_y){
    return getTileIndex(world_x, world_y) > 0;
}

void check_collision(Entity* entity){

    bool isOnGround = false;
    s16 desired_x = entity->x;
    s16 desired_y = entity->y;

    entity->x = entity->x_old;
    entity->y = entity->y_old;

    debug_set(0, entity->x);
    debug_set(1, entity->y);


    // --- X SWEEP: Horizontal bewegen und Wandkollision prüfen ---
    entity->x = desired_x;

    // Bounding Box Definition für den X-Check (Benutzt das alte, sichere Y!)
    s16 left_x_check = entity->x - (entity->width >> 1);
    s16 right_x_check = entity->x + (entity->width >> 1);
    s16 top_x_check = entity->y - (entity->height >> 1);
    s16 bottom_x_check = entity->y + (entity->height >> 1);

if (entity->vx != 0)
{
    // Bounding Box Definition (Die Variablen nutzen bereits die aktuelle entity->x und entity->y von oben, 
    // daher wurden die redundanten Neudefinitionen entfernt, um Warnungen zu vermeiden.)
    
    // Prüfpunkte an den Ecken der Bounding Box (vorne)
    s16 check_x = (entity->vx > 0) ? right_x_check : left_x_check;
    
    // Obere Kante (ca. 1 Pixel von oben)
    s16 check_y1 = top_x_check + 1;
    // Untere Kante (ca. 1 Pixel von unten)
    s16 check_y2 = bottom_x_check - 1;

    // Normale X-Kollision prüfen
    if (isTileSolid(check_x, check_y1) || isTileSolid(check_x, check_y2))
    {
        // Kollision gefunden! (Wand getroffen)
        entity->edge_grab_side = entity->vx;

        entity->vx = 0;

        // ----------------------------------------------------------------------
        // KANTEN-GREIF-PRÜFUNG
        // ----------------------------------------------------------------------

        // Nur prüfen, wenn der Spieler springt (neuer Grab-Versuch)
        if (entity->state == P_JUMPING)
        {
            s16 free_pixels = 0;
            
            // Loop, um die oberen EDGE_GRAB_FREE_HEIGHT Pixel zu prüfen.
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
            debug_set(2, free_pixels);

if (free_pixels == EDGE_GRAB_FREE_HEIGHT && entity->timer_edgegrab == 0)
    {
            VDP_drawText("EDGE GRAB",0,1);

                entity->state = P_EDGE_GRAB;
                entity->vy = 0; // Setze vy auf 0, um Fallbewegung zu stoppen
  
                // -------------------------------------------------------------
                // NEU: KORREKTUR FÜR EDGE GRAB
                // -------------------------------------------------------------
                s16 tile_col_x = check_x / TILE_SIZE_PX;
                
                if (entity->edge_grab_side > 0) // Nach rechts gegrabbt
                {
                    // Verschiebe Spieler nach links zur Kante des kollidierten Tiles
                    s16 tile_left = tile_col_x * TILE_SIZE_PX; 
                    // Setze den rechten Rand der Entität an die linke Kachelkante
                    entity->x = tile_left - (entity->width >> 1) - 1;
                    // Du könntest hier noch einen Offset von -1 oder +1 für einen perfekten Halt hinzufügen
                }
                else // Nach links gegrabbt
                {
                    // Verschiebe Spieler nach rechts zur Kante des kollidierten Tiles
                    s16 tile_right = (tile_col_x + 1) * TILE_SIZE_PX;
                    // Setze den linken Rand der Entität an die rechte Kachelkante
                    entity->x = tile_right + (entity->width >> 1);
                    // Du könntest hier noch einen Offset von -1 oder +1 für einen perfekten Halt hinzufügen
                }
                // -------------------------------------------------------------

                return; // Verlasse die Funktion nach dem erfolgreichen Edge Grab
            }
        }
        
        // ----------------------------------------------------------------------
        // NORMALE WAND-KORREKTUR (falls kein Kanten-Griff möglich)
        // ----------------------------------------------------------------------
        
        // Spieler an die Kachelkante verschieben (Alignment und Offset-Hack)
        if (entity->x > entity->x_old) // Nach rechts bewegt
        {
             s16 tile_left = (check_x / TILE_SIZE_PX) * TILE_SIZE_PX;
             // Behalte deinen -1 Offset, der dein Tunnelling behoben hat
             entity->x = tile_left - (entity->width >> 1) - 1; 
        }
        else // Nach links bewegt
        {
             s16 tile_right = ((check_x / TILE_SIZE_PX) + 1) * TILE_SIZE_PX;
             entity->x = tile_right + (entity->width >> 1);
        }
    } 
}
    
    // --- Y SWEEP: Vertikal bewegen und Boden-/Deckenkollision prüfen ---
    entity->y = desired_y;

    // Bounding Box Definition für den Y-Check
    s16 left_y_check = entity->x - (entity->width >> 1);
    s16 right_y_check = entity->x + (entity->width >> 1);
    s16 top_y_check = entity->y - (entity->height >> 1);
    s16 bottom_y_check = entity->y + (entity->height >> 1);

    if (entity->vy != 0)
    {
        // Prüfpunkte an den Ecken der Bounding Box (oben/unten)
        s16 check_y = (entity->vy > 0) ? bottom_y_check : top_y_check;
        
        // Linke Kante (ca. 1 Pixel von links)
        s16 check_x1 = left_y_check + 1;
        // Rechte Kante (ca. 1 Pixel von rechts)
        s16 check_x2 = right_y_check - 1;

        if (isTileSolid(check_x1, check_y) || isTileSolid(check_x2, check_y))
        {
            // Vertikale Kollision gefunden!
            
            if (entity->vy > 0) // Nach unten bewegt (Landung)
            {
                // Spieler auf die obere Kante der kollidierten Kachel verschieben
                s16 tile_top = (check_y / TILE_SIZE_PX) * TILE_SIZE_PX;
                entity->y = tile_top - (entity->height >> 1);
                entity->vy = 0;
                isOnGround = true; // Landung erfolgt
            }
            else // Nach oben bewegt (Kopf gestoßen)
            {
                bool slide_occurred = false;
                bool hit_left = isTileSolid(check_x1, check_y);
                bool hit_right = isTileSolid(check_x2, check_y);

                // Nur wenn genau EINE Seite trifft, prüfen wir den Eck-Slide
                if (hit_left != hit_right) 
                {
                    // Tile-Kanten basierend auf den Kollisionspunkten
                    s16 tile_col_x1 = check_x1 / TILE_SIZE_PX;
                    s16 tile_col_x2 = check_x2 / TILE_SIZE_PX;
                    s16 tile_right_x1 = (tile_col_x1 + 1) * TILE_SIZE_PX; // Rechte Kante des linken Kollisions-Tiles
                    s16 tile_left_x2 = tile_col_x2 * TILE_SIZE_PX;         // Linke Kante des rechten Kollisions-Tiles

                    if (hit_right) // KORREKTUR 1: Rechte Ecke stößt (Sliding nach Links)
                    {
                        // Prüfe, ob der Überlapp klein genug ist.
                        if ((right_y_check > tile_left_x2) && (right_y_check - tile_left_x2 <= CORNER_CORRECTION_TOLERANCE)) 
                        {
                            // PUSH LEFT: Verschiebe den Spieler, sodass sein rechter Rand 1 Pixel links von der Kachelkante ist.
                            entity->x = tile_left_x2 - (entity->width >> 1) - 1; 
                            slide_occurred = true;
                        }
                    }
                    else if (hit_left) // KORREKTUR 2: Linke Ecke stößt (Sliding nach Rechts)
                    {
                        // Prüfe, ob der Überlapp klein genug ist.
                        if ((left_y_check < tile_right_x1) && (tile_right_x1 - left_y_check <= CORNER_CORRECTION_TOLERANCE))
                        {
                            // PUSH RIGHT: Verschiebe den Spieler, sodass sein linker Rand 1 Pixel rechts von der Kachelkante ist.
                            entity->x = tile_right_x1 + (entity->width >> 1) + 1; 
                            slide_occurred = true;
                        }
                    }
                }
                
                // Spieler auf die untere Kante der kollidierten Kachel verschieben (Vertikale Auflösung)
                s16 tile_bottom = ((check_y / TILE_SIZE_PX) + 1) * TILE_SIZE_PX;
                entity->y = tile_bottom + (entity->height >> 1);
                
                // Geschwindigkeitskorrektur: vy = 0 nur, wenn KEIN Slide stattgefunden hat.
                // Wenn ein Slide stattfand, behält vy den (negativen) Wert, sodass der 
                // Spieler in der nächsten Frame weiter horizontal entlang der Decke "rutscht".
                if (!slide_occurred) {
                    entity->vy = 0;
                }
            }
        }
    }

/*
if (entity->state == P_EDGE_GRAB && false == true)
{
    s16 head_y = entity->y - (entity->height >> 1);

    // Prüfe zwei Punkte: 1 Pixel unter dem Kopf, und 8 Pixel unter dem Kopf
    bool near_corner = false;
    for (s16 y_offset = 1; y_offset <= EDGE_GRAB_FREE_HEIGHT; y_offset++)
    {
        s16 check_y = head_y + y_offset;

        // check_x = dort wo die Kollision ursprünglich erkannt wurde
        s16 check_x = (entity->vx > 0)
                        ? entity->x + (entity->width >> 1)
                        : entity->x - (entity->width >> 1);

        if (!isTileSolid(check_x, check_y))
        {
            near_corner = true;
            break;
        }
    }

    if (!near_corner)
    {
        entity->state = P_FALLING;
    }
}
*/








    s16 check_y_ground = entity->y + (entity->height >> 1) + 1; 
    s16 check_x1_ground = entity->x - (entity->width >> 1) + 1;
    s16 check_x2_ground = entity->x + (entity->width >> 1) - 1;

    if (isTileSolid(check_x1_ground, check_y_ground) || isTileSolid(check_x2_ground, check_y_ground)) isOnGround = true;
    if (isOnGround) entity->state = P_GROUNDED;
    else if (entity->state != P_JUMPING && entity->state != P_EDGE_GRAB) entity->state = P_FALLING;

}