#include "entity.h"
#include "title.h"

u8 entities_count = 0;
Entity Entities[MAX_ENTITIES];


void add_entity(){
    if (entities_count >= MAX_ENTITIES) return;


    entities_count++;

        // --- SPIELER INIT ---
    Entities[entities_count].type = ENTITY_PLAYER;
    Entities[entities_count].width = 12; // Größe anpassen
    Entities[entities_count].height = 12;
    Entities[entities_count].x = FIX32(300); // Startposition
    Entities[entities_count].y = FIX32(100);
    Entities[entities_count].x_old = Entities[entities_count].x;
    Entities[entities_count].y_old = Entities[entities_count].y;
    Entities[entities_count].vx = FIX16(0); // Verwende FIX32 für Geschw.
    Entities[entities_count].vy = FIX16(0); // Verwende FIX32 für Geschw.
    Entities[entities_count].jumping = TRUE;
    Entities[entities_count].anim_index = 10; // FIX16 für Animationsindex beibehalten
    Entities[entities_count].x = 0;
    Entities[entities_count].sprite = SPR_addSprite(&player_sprite, F32_toInt(Entities[entities_count].x), F32_toInt(Entities[entities_count].y), TILE_ATTR(PAL1, 0, FALSE, FALSE));
    SPR_setAnimAndFrame(Entities[entities_count].sprite, 0, 0);

}