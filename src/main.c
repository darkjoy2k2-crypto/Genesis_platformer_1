#include <genesis.h>
#include "title.h" // Stellt level_bg und player_sprite bereit
#include "level.h"
#include "entity_list.h"
#include "checkCollisions.h"
#include "physics.h"



Vect2D_s16 camera_position = {.x = 0, .y = 0}; 
char info[40]; // Erhöht auf 40, um Überlauf zu vermeiden
Map* level_1_map;
bool show_level = true;

int i = 0;
int player_id = 0;

static void handlecamera(Entity* player){
    int p_int_x = player->x;
    int p_int_y = player->y;

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

    
    SPR_setPosition(player->sprite, p_int_x - camera_position.x - 8, p_int_y - camera_position.y - 8);

}

u16 ind = TILE_USER_INDEX;



void update_animation(Entity* e){
    int dx = e->x_old - e->x;


    e->anim_index += dx ; // approx *1.2

    if (e->anim_index < 0) e->anim_index += 50;
    if (e->anim_index > 49) e->anim_index -= 50;

    SPR_setAnimAndFrame(e->sprite, 0, e->anim_index / 6);
    sprintf(info, "AF: %d STATE: %d      ",e->anim_index, e->state);
    VDP_drawText(info, 0, 0);
}

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
    player_id = create_entity(50,50,12,12,ENTITY_PLAYER);

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

    // --- GAME LOOP ---
    while(1) {
             
        handle_all_entities();

        update_animation(&entities[player_id]);

        handlecamera(&entities[player_id]);

       entities[player_id].x_old = entities[player_id].x;
       entities[player_id].y_old = entities[player_id].y;

        SPR_update(); 
        SYS_doVBlankProcess();
    }
    return (0);
}