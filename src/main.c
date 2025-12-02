#include <genesis.h>
#include "title.h" // Stellt level_bg und player_sprite bereit
#include "level.h"
#include "entity.h"
//#include "collision.h"
#include "checkCollisions.h"

// Konstanten für die Kameragrenzen (müssen in Ihrem 'level.h' oder 'level.c' definiert sein)
// Für die Kompilierbarkeit füge ich hier temporäre Platzhalter hinzu, falls diese fehlen.
#ifndef cam_max_x
#define cam_max_x 160
#define cam_min_x 100
#define cam_max_y 140
#define cam_min_y 60
#endif

Entity player;
Vect2D_s16 camera_position = {.x = 0, .y = 0}; 
char info[40]; // Erhöht auf 40, um Überlauf zu vermeiden
Map* level_1_map;
bool show_level = true;

int i = 0;

static void handlecamera(){
    // Achtung: player.x_s16 und player.y_s16 werden von check_collision aktualisiert
    int p_int_x = player.x_s16;
    int p_int_y = player.y_s16;

    int p_screen_x = p_int_x - camera_position.x;
    int p_screen_y = p_int_y - camera_position.y;

    int new_cam_x;
    int new_cam_y;

    if (p_screen_x > cam_max_x){
        new_cam_x = p_int_x - cam_max_x;
    } else if (p_screen_x < cam_min_x){
        new_cam_x = p_int_x - cam_min_x;
    } else {
        new_cam_x = camera_position.x;
    }

    if (p_screen_y > cam_max_y){
        new_cam_y = p_int_y - cam_max_y;
    } else if (p_screen_y < cam_min_y){
        new_cam_y = p_int_y - cam_min_y;
    } else {
        new_cam_y = camera_position.y;
    }

    if (new_cam_x != camera_position.x || new_cam_y != camera_position.y){
        camera_position.x = new_cam_x;
        camera_position.y = new_cam_y;
    }

    // Level-Grenzen der Kamera (basierend auf der Annahme MAP_W=60, MAP_H=40)
    // Map-Größe: 960x640 Pixel. Viewport: 320x224 Pixel.
    // Minimale X/Y Position
    if (camera_position.x < 0){
        camera_position.x = 0;
    }
    if (camera_position.y < 0){
        camera_position.y = 0;
    }

    // Maximale X/Y Position
    if (camera_position.x > 960 - 320){ // 960 - 320 = 640
        camera_position.x = 960 - 320;
    }
    if (camera_position.y > 640 - 224){ // 640 - 224 = 416
        camera_position.y = 640 - 224;
    }

    if (show_level){
    MAP_scrollTo(level_1_map, camera_position.x, camera_position.y);
    }

    VDP_setHorizontalScroll(BG_B, -camera_position.x >> 2);
    // VDP_setVerticalScroll(BG_B, camera_position.y >> 3); // Geringere Scroll-Geschw.
    // Ihre Scroll-Logik für Plane B beibehalten:
    int scroll_index_y = 128;
    VDP_setVerticalScroll(BG_B, scroll_index_y);

    
    SPR_setPosition(player.sprite, p_int_x - camera_position.x - 8, p_int_y - camera_position.y - 8);

}

u16 ind = TILE_USER_INDEX;
        u16 state;
        u16 state_old;

int main() {
    // Initialisierung
    SPR_init(); // Sprite Engine starten
    VDP_setScreenWidth320();
    
    // --- GRAFIK SETUP ---
    PAL_setPalette(PAL0, bg_palette.data, DMA);
    PAL_setPalette(PAL1, player_sprite.palette->data, DMA);
    PAL_setPalette(PAL3, layer_1_palette.data,DMA);

    VDP_setTextPlane(WINDOW); 
    VDP_setWindowHPos(0, 0);
    VDP_setWindowVPos(0, 34);

    VDP_drawImageEx(BG_B,&layer_bg, TILE_ATTR_FULL(PAL0,false,false,false,ind),0,5, false, true);

    ind += layer_bg.tileset->numTile;
    VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
        
    init_entities();
    add_entity();

    if (show_level){
        // Annahme: our_tileset und our_level_map sind definiert
        // Falls Sie Fehler bekommen, liegt es an fehlenden Includes oder Variablen.
        VDP_loadTileSet(&our_tileset, ind, DMA);

        level_1_map = MAP_create(&our_level_map, BG_A, TILE_ATTR_FULL(PAL3,FALSE,FALSE,FALSE,ind));
        ind += our_tileset.numTile;
    }



    // Spieler Sprite erstellen
    // player.x_s16 wird später in der Loop durch pos_f32_s16 gesetzt
    // Frame 0 setzen
    SPR_setAnimAndFrame(player.sprite, 0, 0);

    // --- GAME LOOP ---
    while(1) {
        // Debug-Informationen
        // Korrektur: %d -> %ld für s32-Werte (F32_toInt)
        sprintf(info, "X:%d Y:%d VX:%ld VY:%ld        ", player.x_s16, player.y_s16, player.vx, player.vy);
        VDP_drawText(info, 0, 0);
        sprintf(info, "GND:%d         ", player.isOnGround);
        VDP_drawText(info, 0, 1);
                
        // 1. Physik vorbereiten (Alte Position für Deltatime/Kamera-Berechnung)
        player.x_old = player.x;
        player.y_old = player.y;

        // 2. Input lesen & Geschwindigkeit anpassen
        state = JOY_readJoypad(JOY_1);

        // Sprung
        if ((state & BUTTON_A) && !(state_old & BUTTON_A) && player.isOnGround) {
            player.isOnGround = FALSE;
            player.vy = FIX16(-5.0); // Starker Sprungimpuls
        }

        if (!(state & BUTTON_A) && (state_old & BUTTON_A) && player.vy < FIX16(0) ) {
            player.vy = FIX16(0.4);
        }

        // Horizontaler Schub
        if (state & BUTTON_LEFT) {
            player.vx = player.vx - FIX16(1);

            } else if (state & BUTTON_RIGHT) {

            player.vx = player.vx + FIX16(1);
      
        }

        state_old = state;

        // --- 3. Physik-Update ---

if (!player.isOnGround) {
    if (player.vy < FIX16(0)) {
        // Ball steigt (vy ist negativ). Wir wollen langsam verlangsamen.
        player.vy = player.vy + FIX16(0.25);  // Setzen Sie hier den KLEINEREN Wert (z.B. 1)
    } else {
        // Ball fällt (vy ist Null oder positiv). Wir wollen schnell beschleunigen.
        player.vy = player.vy + FIX16(0.4); // Setzen Sie hier den GRÖSSEREN Wert (z.B. 3)
    }
}

             // Stärkere horizontale Reibung auf dem Boden
            // Korrektur: F32_mul -> fix32Mul
            player.vx = F16_mul(player.vx, FIX16(0.7)); 

        // Geschwindigkeit kappen (Clamping)
        if (player.vx > FIX16(7.0)) player.vx = FIX16(7.0);
        if (player.vx < FIX16(-7.0)) player.vx = FIX16(-7.0);
        if (player.vy > FIX16(10.0)) player.vy = FIX16(10.0); // Max. Fallgeschwindigkeit
        
        // Zu feine Bewegungen auf null setzen (stoppt das "Schlängeln" bei langsamen Geschw.)
        if (F32_toRoundedInt(player.vx) == 0 && (player.vx < FIX16(0.1) && player.vx > FIX16(-0.1))) {
            player.vx = FIX16(0);
        }
        if (F16_toRoundedInt(player.vy) == 0 && (player.vy < FIX16(0.1) && player.vy > FIX16(-0.1))) {
            // Nur nullen, wenn am Boden, sonst kann es den Fall stoppen
            if (player.isOnGround) player.vy = FIX16(0);
        }

        // 4. POSITION INTEGRIEREN (Hier wird die neue Position berechnet!)
        player.x = player.x + F16_toFix32(player.vx);
        player.y = player.y + F16_toFix32(player.vy);

        // 5. Kollision prüfen (Korrigiert die Position und setzt vy/vx auf 0 bei Treffer)
        check_collision(&player);

        // --- 6. VISUALS & KAMERA ---

        // Animation (Roll-Effekt)
        // Korrektur: FIX16Add -> fix16Add, F32ToFIX16 -> F32_toFix16
        player.anim_index += F32_toInt(F32_mul(player.x_old - player.x, FIX32(1.2))); 

        if (player.anim_index < 10) player.anim_index += 100;
        if (player.anim_index > 100) player.anim_index -= 100;
        SPR_setAnimAndFrame(player.sprite, 0, player.anim_index / 10);

        handlecamera();

        SPR_update(); 
        SYS_doVBlankProcess();
    }
    return (0);
}