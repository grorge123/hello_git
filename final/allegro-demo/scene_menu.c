#include "scene_menu.h"
#include "game.h"
#include "utility.h"
#include "scene_start.h"
#include "scene_settings.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>

static const char* txt_title = "Space Shooter";
static const char* txt_info = "Press enter key to start";
static ALLEGRO_BITMAP* img_background;

static ALLEGRO_BITMAP* img_settings;
static ALLEGRO_BITMAP* img_settings2;
static ALLEGRO_SAMPLE* bgm;
static ALLEGRO_SAMPLE_ID bgm_id;

static void init(void);
static void draw(void);
static void destroy(void);
static void on_key_down(int keycode);

int setting_button_x;
int setting_button_y;


static void init(void) {
    img_background = load_bitmap_resized(".\\img\\main-bg.jpg", SCREEN_W, SCREEN_H);
    img_settings = load_bitmap(".\\img\\settings.png");
    img_settings2 = load_bitmap(".\\img\\settings2.png");
    setting_button_x = SCREEN_W - 2 * al_get_bitmap_width(img_settings2);
    setting_button_y = 0 + al_get_bitmap_height(img_settings2);
    bgm = load_audio(".\\img\\S31-Night Prowler.ogg");
    bgm_id = play_bgm(bgm, 1);
    game_log("Menu scene initialized");
}

static void draw(void) {
    al_draw_bitmap(img_background, 0, 0, 0);
    if (pnt_in_rect(mouse_x, mouse_y, setting_button_x, setting_button_y, al_get_bitmap_width(img_settings2), al_get_bitmap_height(img_settings2)))
        al_draw_bitmap(img_settings2, setting_button_x, setting_button_y, 0);
    else
        al_draw_bitmap(img_settings, setting_button_x, setting_button_y, 0);
    al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, txt_title);
    al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 20, SCREEN_H - 50, 0, txt_info);
}

static void destroy(void) {
    al_destroy_sample(bgm);
    al_destroy_bitmap(img_background);
    al_destroy_bitmap(img_settings);
    al_destroy_bitmap(img_settings2);
    stop_bgm(bgm_id);
    game_log("Menu scene destroyed");
}

static void on_key_down(int keycode) {
    if (keycode == ALLEGRO_KEY_ENTER)
        game_change_scene(&start_scene);
}


static void on_mouse_down(int btn, int x, int y, int dz) {
    game_log("dz %d", dz);
    if (btn == 1) {
        if (pnt_in_rect(x, y, setting_button_x, setting_button_y, al_get_bitmap_width(img_settings2), al_get_bitmap_height(img_settings2)))
            game_change_scene(&setting_scene);
    }
}


void scene_menu_create(void) {
    game_log("Menu scene start creat");
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    scene.name = "Menu";
    scene.initialize = &init;
    scene.draw = &draw;
    scene.destroy = &destroy;
    scene.on_key_down = &on_key_down;
    scene.on_mouse_down = &on_mouse_down;
     scene.init = false;
    menu_scene = scene;
    game_log("%s scene created",scene.name);
    return;
}
