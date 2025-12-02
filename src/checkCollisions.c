#include <genesis.h>
#include "checkCollisions.h"
#include "entity.h"
#include "level.h" // Muss für die Map-Struktur und Definitionen enthalten sein

// Wichtige Annahme: Die Kachelgröße ist 16x16 Pixel.
// Um Konflikte mit der SGDK-Definition zu vermeiden (Warning "TILE_SIZE" redefined), wurde TILE_SIZE in TILE_SIZE_PX umbenannt.
#define TILE_SIZE_PX 16
// Map-Dimensionen. Stellen Sie sicher, dass diese mit Ihren definierten MAP_W/MAP_H Konstanten übereinstimmen.
#define MAP_WIDTH_TILES 60
#define MAP_HEIGHT_TILES 40

// KORREKTUR: Wir verwenden nun das rohe Daten-Array für die Kollision,
// um den Fehler 'Map' has no member named 'data' zu vermeiden.
// Diese Variable MUSS in einer anderen .c-Datei (z.B. main.c oder level.c) als:
// const u16 map_collision_data[MAP_WIDTH_TILES * MAP_HEIGHT_TILES] = { ... };
// definiert werden. Die Werte 0 bedeuten leer (keine Kollision), > 0 bedeuten solide.
extern const u16 map_collision_data[MAP_WIDTH_TILES * MAP_HEIGHT_TILES];

// Gibt den Kachel-Index an der gegebenen Weltposition (Pixel) auf Plane A zurück.
// Kacheln mit Index 1 (oder höher) werden als solide angenommen.
static u16 getTileIndex(s16 world_x, s16 world_y)
{
    // Weltkoordinaten in Kachelkoordinaten umrechnen
    s16 tile_x = world_x / TILE_SIZE_PX;
    s16 tile_y = world_y / TILE_SIZE_PX;

    // Prüfung auf Map-Grenzen
    if (tile_x < 0 || tile_y < 0 || tile_x >= MAP_WIDTH_TILES || tile_y >= MAP_HEIGHT_TILES)
    {
        // Außerhalb der Map -> Als nicht-solide behandeln
        return 0;
    }

    // Zugriff auf das rohe Kollisionsdaten-Array.
    // Index = y * map_width + x
    u16 tile_index = map_collision_data[tile_y * MAP_WIDTH_TILES + tile_x];

    // Wir nehmen an, dass Kacheln mit Index 1 oder höher solide sind.
    // Daher ist die Bedingung `tile_index > 0` ausreichend.
    return tile_index;
}

// Prüft, ob eine Kachel an der gegebenen Weltposition solide ist.
static bool isTileSolid(s16 world_x, s16 world_y)
{
    // Fürs Erste: Ist der Kachel-Index > 0?
    return getTileIndex(world_x, world_y) > 0;
}

// Haupt-Kollisionslogik-Funktion
void check_collision(Entity* entity)
{
    // 1. Speichere die gewünschte (aber möglicherweise illegale) Position und setze auf alte, sichere Position zurück.
    // main.c hat die Integration bereits durchgeführt. Wir setzen zurück und machen die Sweeps nacheinander.
    fix32 desired_x = entity->x;
    fix32 desired_y = entity->y;

    entity->x = entity->x_old;
    entity->y = entity->y_old;

    // --- X SWEEP: Horizontal bewegen und Wandkollision prüfen ---
    entity->x = desired_x;
    entity->x_s16 = F32_toInt(entity->x);
    entity->y_s16 = F32_toInt(entity->y); // y_s16 bleibt bei y_old (sichere Position)

    // Bounding Box Definition für den X-Check (Benutzt das alte, sichere Y!)
    s16 left_x_check = entity->x_s16 - (entity->width / 2);
    s16 right_x_check = entity->x_s16 + (entity->width / 2);
    s16 top_x_check = entity->y_s16 - (entity->height / 2);
    s16 bottom_x_check = entity->y_s16 + (entity->height / 2);

    if (entity->vx != FIX32(0))
    {
        // Prüfpunkte an den Ecken der Bounding Box (vorne)
        s16 check_x = (entity->vx > FIX32(0)) ? right_x_check : left_x_check;
        
        // Obere Kante (ca. 1 Pixel von oben)
        s16 check_y1 = top_x_check + 1;
        // Untere Kante (ca. 1 Pixel von unten)
        s16 check_y2 = bottom_x_check - 1;

        if (isTileSolid(check_x, check_y1) || isTileSolid(check_x, check_y2))
        {
            // Kollision gefunden! (Wand getroffen)
            entity->vx = FIX32(0);
            
            // Spieler an die Kachelkante verschieben (Alignment)
            if (entity->x > entity->x_old) // Nach rechts bewegt
            {
                s16 tile_left = (check_x / TILE_SIZE_PX) * TILE_SIZE_PX;
                entity->x = FIX32(tile_left - (entity->width / 2));
            }
            else // Nach links bewegt
            {
                s16 tile_right = ((check_x / TILE_SIZE_PX) + 1) * TILE_SIZE_PX;
                entity->x = FIX32(tile_right + (entity->width / 2));
            }
        }
    }
    
    // --- Y SWEEP: Vertikal bewegen und Boden-/Deckenkollision prüfen ---
    entity->y = desired_y;
    entity->x_s16 = F32_toInt(entity->x); // Nutzt den korrigierten X-Wert aus dem X-Sweep
    entity->y_s16 = F32_toInt(entity->y); // Neuer Y-Screen-Wert

    // Bounding Box Definition für den Y-Check
    s16 left_y_check = entity->x_s16 - (entity->width / 2);
    s16 right_y_check = entity->x_s16 + (entity->width / 2);
    s16 top_y_check = entity->y_s16 - (entity->height / 2);
    s16 bottom_y_check = entity->y_s16 + (entity->height / 2);

    entity->isOnGround = FALSE; // Voraussichtlich nicht am Boden (wird durch Korrektur überschrieben)

    if (entity->vy != FIX32(0))
    {
        // Prüfpunkte an den Ecken der Bounding Box (oben/unten)
        s16 check_y = (entity->vy > FIX32(0)) ? bottom_y_check : top_y_check;
        
        // Linke Kante (ca. 1 Pixel von links)
        s16 check_x1 = left_y_check + 1;
        // Rechte Kante (ca. 1 Pixel von rechts)
        s16 check_x2 = right_y_check - 1;

        if (isTileSolid(check_x1, check_y) || isTileSolid(check_x2, check_y))
        {
            // Vertikale Kollision gefunden!
            
            if (entity->vy > FIX32(0)) // Nach unten bewegt (Landung)
            {
                // Spieler auf die obere Kante der kollidierten Kachel verschieben
                s16 tile_top = (check_y / TILE_SIZE_PX) * TILE_SIZE_PX;
                entity->y = FIX32(tile_top - (entity->height / 2));
                
                entity->isOnGround = TRUE; // Landung erfolgt
            }
            else // Nach oben bewegt (Kopf gestoßen)
            {
                // Spieler auf die untere Kante der kollidierten Kachel verschieben
                s16 tile_bottom = ((check_y / TILE_SIZE_PX) + 1) * TILE_SIZE_PX;
                entity->y = FIX32(tile_bottom + (entity->height / 2));
            }

            // Vertikale Geschwindigkeit auf Null setzen (Hard Stop in Y-Richtung)
            entity->vy = FIX32(0);
        }
    }
    
    // 3. Abschließende Positionen und statische Bodenprüfung
    entity->x_s16 = F32_toInt(entity->x);
    entity->y_s16 = F32_toInt(entity->y);

    
    // --- STATISCHE BODEN-PRÜFUNG ---
    // Diese Prüfung stellt sicher, dass isOnGround auch bei vy=0 TRUE bleibt.
    
    // Prüfen, ob eine solide Kachel 1 Pixel unterhalb des Players liegt
    s16 check_y_ground = entity->y_s16 + (entity->height / 2) + 1; 
    s16 check_x1_ground = entity->x_s16 - (entity->width / 2) + 1;
    s16 check_x2_ground = entity->x_s16 + (entity->width / 2) - 1;

    if (isTileSolid(check_x1_ground, check_y_ground) || isTileSolid(check_x2_ground, check_y_ground))
    {
        // Wir stehen auf dem Boden.
        entity->isOnGround = TRUE;
    }
    // WICHTIG: Wenn der Spieler bereits durch den Y-Sweep auf TRUE gesetzt wurde, 
    // muss die statische Prüfung dies bestätigen, oder der nächste Frame korrigiert es.
}