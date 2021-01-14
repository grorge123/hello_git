#include "scene_gameover.h"
#include "scene_menu.h"
#include "game.h"
#include "utility.h"
#include "scene_start.h"
#include "scene_settings.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

static ALLEGRO_BITMAP* img_background;

static ALLEGRO_SAMPLE* bgm;
static ALLEGRO_SAMPLE_ID bgm_id;

static void init(void);
static void draw(void);
static void destroy(void);
static void on_key_down(int keycode);

static void init(void) {
    img_background = load_bitmap_resized(".\\img\\gameover-bg.jpg", SCREEN_W, SCREEN_H);
//    bgm = load_audio(".\\img\\S31-Night Prowler.ogg");
//    bgm_id = play_bgm(bgm, 1);
    game_log("gameover scene initialized");
}

static void draw(void) {
    al_draw_bitmap(img_background, 0, 0, 0);
    char txt_info[] = "Press enter key to restart";
    al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 20, SCREEN_H - 50, 0, txt_info);
}

static void destroy(void) {
    al_destroy_sample(bgm);
    al_destroy_bitmap(img_background);
    stop_bgm(bgm_id);
    game_log("gameover scene destroyed");
}

static void on_key_down(int keycode) {
    if (keycode == ALLEGRO_KEY_ENTER)
        game_change_scene(&menu_scene);
}


// static void on_mouse_down(int btn, int x, int y, int dz) {
//
// }


void scene_gameover_create(void) {
    game_log("Game over scene start creat");
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    scene.name = "Game over";
    scene.initialize = &init;
    scene.draw = &draw;
    scene.destroy = &destroy;
    scene.on_key_down = &on_key_down;
    // scene.on_mouse_down = &on_mouse_down;
    scene.initialize();
    scene.init = true;
    gameover_scene = scene;
    game_log("%s scene created",scene.name);
    return;
}
