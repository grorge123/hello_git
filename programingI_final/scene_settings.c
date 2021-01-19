// [HACKATHON 3-9]
// TODO: Create scene_settings.h and scene_settings.c.
// No need to do anything for this part. We've already done it for
// you, so this 2 files is like the default scene template.
#include "scene_settings.h"
#include "scene_menu.h"
#include "scene_start.h"
#include "game.h"
#include "utility.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>

// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.

static ALLEGRO_BITMAP* img_background;

static ALLEGRO_BITMAP* img_single;
static ALLEGRO_BITMAP* img_double;
static ALLEGRO_BITMAP* img_player1;
static ALLEGRO_BITMAP* img_player2;
static ALLEGRO_BITMAP* img_player3;

static int BUTTON_W = 100, BUTTON_H = 100;

static int choose = 0;

static void init(void) {
    game_log("setting scene start initializ");
    img_background = load_bitmap_resized(".\\img\\setting-bg.jpg", SCREEN_W, SCREEN_H);
    img_single = load_bitmap_resized(".\\img\\img_single.png", BUTTON_W, BUTTON_H);
    img_double = load_bitmap_resized(".\\img\\img_double.png", BUTTON_W * 2, BUTTON_H);
    img_player1 = load_bitmap_resized(".\\img\\img_player1.png", BUTTON_W, BUTTON_H);
    img_player2 = load_bitmap_resized(".\\img\\img_player2.png", BUTTON_W, BUTTON_H);
    img_player3 = load_bitmap_resized(".\\img\\img_player3.png", BUTTON_W, BUTTON_H);
    game_log("setting scene initialized");
}

static int line1 = 150, line2 = 300;
static int left_space = 150;

static void draw(void) {
    al_draw_bitmap(img_background, 0, 0, 0);

    al_draw_bitmap(img_single, left_space, line1, 0);
    al_draw_bitmap(img_double, left_space * 2 + BUTTON_W, line1, 0);
    al_draw_bitmap(img_player1, left_space, line2, 0);
    al_draw_bitmap(img_player2, left_space * 2 + BUTTON_W, line2, 0);
    al_draw_bitmap(img_player3, left_space * 3 + BUTTON_W * 2, line2, 0);
    int thinkness = 10;
    if(choose == 0)al_draw_rectangle(left_space, line1,
    left_space + al_get_bitmap_width(img_single), line1 + al_get_bitmap_height(img_single), al_map_rgb(255, 0, 0), thinkness);
    if(choose == 1)al_draw_rectangle(left_space * 2 + BUTTON_W, line1,
    left_space * 2 + BUTTON_W + al_get_bitmap_width(img_double), line1 + al_get_bitmap_height(img_double), al_map_rgb(255, 0, 0), thinkness);
    if(choose == 2)al_draw_rectangle(left_space , line2,
    left_space + al_get_bitmap_width(img_player1), line2 + al_get_bitmap_height(img_player1), al_map_rgb(255, 0, 0), thinkness);
    if(choose == 3)al_draw_rectangle(left_space * 2 + BUTTON_W, line2,
    left_space * 2 + BUTTON_W + al_get_bitmap_width(img_player2), line2 + al_get_bitmap_height(img_player2), al_map_rgb(255, 0, 0), thinkness);
    if(choose == 4)al_draw_rectangle(left_space * 3 + BUTTON_W * 2, line2,
    left_space * 3 + BUTTON_W * 2 + al_get_bitmap_width(img_player3), line2 + al_get_bitmap_height(img_player3), al_map_rgb(255, 0, 0), thinkness);

}
static void update(void){

}
static void on_key_down(int keycode) {
    // [HACKATHON 3-10]
    // TODO: If active_scene is SCENE_SETTINGS and Backspace is pressed,
    // return to SCENE_MENU.
    if (keycode == ALLEGRO_KEY_ESCAPE){
        game_change_scene(&menu_scene);
    }
    if (choose == 0){
        if(keycode == ALLEGRO_KEY_DOWN)choose = 2;
        if(keycode == ALLEGRO_KEY_RIGHT)choose = 1;
    }else if(choose == 1){
        if(keycode == ALLEGRO_KEY_DOWN)choose = 3;
        if(keycode == ALLEGRO_KEY_RIGHT)choose = 2;
        if(keycode == ALLEGRO_KEY_LEFT)choose = 0;
    }else if(choose == 2){
        if(keycode == ALLEGRO_KEY_UP)choose = 0;
        if(keycode == ALLEGRO_KEY_RIGHT)choose = 3;
        if(keycode == ALLEGRO_KEY_LEFT)choose = 1;
    }else if(choose == 3){
        if(keycode == ALLEGRO_KEY_UP)choose = 1;
        if(keycode == ALLEGRO_KEY_RIGHT)choose = 4;
        if(keycode == ALLEGRO_KEY_LEFT)choose = 2;
    }else if(choose == 4){
        if(keycode == ALLEGRO_KEY_UP)choose = 1;
        if(keycode == ALLEGRO_KEY_LEFT)choose = 3;
    }
    if(keycode == ALLEGRO_KEY_ENTER){
        if(choose == 0)player_double = false;
        if(choose == 1)player_double = true;
        if(choose == 2)strcpy(plane_img[0], ".\\img\\img_player1.png"),strcpy(plane_img[1], ".\\img\\img_player1.png");
        if(choose == 3)strcpy(plane_img[0], ".\\img\\img_player2.png"),strcpy(plane_img[1], ".\\img\\img_player2.png");
        if(choose == 4)strcpy(plane_img[0], ".\\img\\img_player3.png"),strcpy(plane_img[1], ".\\img\\img_player3.png");

    }
}

// The only function that is shared across files.
void scene_settings_create(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    scene.initialize = &init;
    scene.name = "setting";
    scene.draw = &draw;
    scene.on_key_down = &on_key_down;
    scene.update = &update;
    scene.init = false;
    // TODO: Register more event callback functions such as keyboard, mouse, ...
    setting_scene = scene;
    game_log("Settings scene created");
    return;
}
