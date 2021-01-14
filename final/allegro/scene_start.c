#include "scene_start.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "utility.h"
#include <math.h>
#include "scene_menu.h"

#include "queue.h"

// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.

static ALLEGRO_BITMAP* img_background;
static ALLEGRO_BITMAP* img_plane;
static ALLEGRO_BITMAP* img_enemy;
// [HACKATHON 2-1]
// TODO: Declare a variable to store your bullet's image.
// Uncomment and fill in the code below.
static ALLEGRO_BITMAP* img_bullet;
static ALLEGRO_BITMAP* img_bullet_enemies;

typedef struct {
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the object’s image.
    ALLEGRO_BITMAP* img;
    int hp;
    int born_aixl;
} MovableObject;

static void init(void);
static void update(void);
static void draw_movable_object(MovableObject obj);
static void draw(void);
static void destroy(void);

#define MAX_ENEMY 5
#define MAX_ENEMY_SHOW 8
// [HACKATHON 2-2]
// TODO: Declare the max bullet count that will show on screen.
// You can try max 4 bullets here and see if you needed more.
// Uncomment and fill in the code below.
#define MAX_BULLET 50
static MovableObject plane;
static MovableObject enemies[MAX_ENEMY_SHOW][MAX_ENEMY];
bool enemies_show[MAX_ENEMY_SHOW];
// [HACKATHON 2-3]
// TODO: Declare an array to store bullets with size of max bullet count.
// Uncomment and fill in the code below.
static MovableObject bullets[MAX_BULLET];
static MovableObject bullets_enemies[MAX_ENEMY_SHOW][MAX_ENEMY];

// [HACKATHON 2-4]
// TODO: Set up bullet shooting cool-down variables.
// 1) Declare your shooting cool-down time as constant. (0.2f will be nice)
// 2) Declare your last shoot timestamp.
// Uncomment and fill in the code below.
static const float MAX_COOLDOWN = 0.2f;
static double last_shoot_timestamp;
static ALLEGRO_SAMPLE* bgm;
static ALLEGRO_SAMPLE_ID bgm_id;
static bool draw_gizmos;

static void init(void) {
    int i;
    img_background = load_bitmap_resized("start-bg.jpg", SCREEN_W, SCREEN_H);
    img_plane = plane.img = load_bitmap("plane.png");
    plane.x = 400;
    plane.y = 500;
    plane.w = al_get_bitmap_width(plane.img);
    plane.h = al_get_bitmap_height(plane.img);
    img_enemy = load_bitmap("rocket-4.png");
    enemies_show[0] = true;
    for (int q = 0; q < MAX_ENEMY_SHOW; q++) {
        for (i = 0; i < MAX_ENEMY; i++) {
            enemies[q][i].img = img_enemy;
            enemies[q][i].w = al_get_bitmap_width(img_enemy);
            enemies[q][i].h = al_get_bitmap_height(img_enemy);
            if (i == 0) {
                enemies[q][i].y = 0;
                enemies[q][i].x = rand() % SCREEN_W;
                enemies[q][0].born_aixl = enemies[0][i].x;
            }
            else {
                enemies[q][i].y = enemies[0][i - 1].y - 100;
                enemies[q][i].x = enemies[0][i - 1].x;
            }
            enemies[q][i].hp = 2;
            enemies[q][i].hidden = false;
        }
    }
    // [HACKATHON 2-5]
    // TODO: Initialize bullets.
    // 1) Search for a bullet image online and put it in your project.
    //    You can use the image we provided.
    // 2) Load it in by 'load_bitmap' or 'load_bitmap_resized'.
    // 3) For each bullets in array, set their w and h to the size of
    //    the image, and set their img to bullet image, hidden to true,
    //    (vx, vy) to (0, -3).
    // Uncomment and fill in the code below.
    img_bullet = al_load_bitmap("image12.png");
    img_bullet_enemies = al_load_bitmap("rocket-1.png");
    for (int i = 0; i < MAX_BULLET; i++) {
        bullets[i].img = img_bullet;
        bullets[i].w = al_get_bitmap_width(img_bullet);
        bullets[i].h = al_get_bitmap_height(img_bullet);
        bullets[i].vx = 0;
        bullets[i].vy = -3;
        bullets[i].hidden = true;
    }
    /*for (int i = 0; i < MAx_enemy; i++) {
        bullets_enemies[i].img = img_bullet_enemies;
        bullets_enemies[i].w = al_get_bitmap_width(img_bullet);
        bullets_enemies[i].h = al_get_bitmap_height(img_bullet);
        bullets_enemies[i].vx = 1;
        bullets_enemies[i].vy = 3;
        bullets_enemies[i].hidden = true;
    }*/
    // Can be moved to shared_init to decrease loading time.
    bgm = load_audio("mythica.ogg");
    game_log("Start scene initialized");
    bgm_id = play_bgm(bgm, 1);
}
bool collision(MovableObject a, MovableObject b) {
    int flag1 = 0, flag2 = 0;
    if (a.x - a.w / 2 >= b.x - b.w / 2 && a.x - a.w / 2 <= b.x + b.w / 2)flag1 = 1;
    if (a.x + a.w / 2 >= b.x - b.w / 2 && a.x + a.w / 2 <= b.x + b.w / 2)flag1 = 1;
    if (a.y - a.h / 2 >= b.y - b.h / 2 && a.y - a.h / 2 <= b.y + b.h / 2)flag2 = 1;
    if (a.y + a.h / 2 >= b.y - b.h / 2 && a.y + a.h / 2 <= b.y + b.h / 2)flag2 = 1;
    return flag1 && flag2;
}
void born(MovableObject enemies[],int num) {
    enemies_show[num] = 1;
    for (int i = 0; i < MAX_ENEMY; i++) {
        if (i == 0) {
            enemies[i].y = 0;
            enemies[i].x = rand() % SCREEN_W;
            enemies[0].born_aixl = enemies[i].x;
        }
        else {
            enemies[i].y = enemies[i - 1].y - 100;
            enemies[i].x = enemies[i - 1].x;
        }
        enemies[i].hidden = false;
        enemies[i].hp = 2;
    }
}
bool check_enemis(MovableObject enemies[], int num) {
    for (int i = 0; i < MAX_ENEMY; i++) {
        enemies[i].y += 1;
        enemies[i].x = enemies[0].born_aixl + cos(enemies[i].y / 10) * 15;
        if (enemies[MAX_ENEMY - 1].y > SCREEN_H) {
            enemies_show[num] = 0;
        }
    }
    for (int i = 0; i < MAX_BULLET; i++) {
        for (int q = 0; q < MAX_ENEMY; q++) {
            if (bullets[i].hidden || enemies[q].hidden)continue;
            if (collision(bullets[i], enemies[q])) {
                bullets[i].hidden = true;
                enemies[q].hp--;
                if (enemies[q].hp <= 0)enemies[q].hidden = true;
            }
        }
    }
    for (int i = 0; i < MAX_ENEMY; i++) {
        if (!enemies[i].hidden &&collision(plane, enemies[i])) {
            printf("OUT\n");
            game_change_scene(scene_menu_create());
            return;
        }
    }
}
static void update(void) {
    double now = al_get_time();
    plane.vx = plane.vy = 0;
    if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
        plane.vy -= 1;
    if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
        plane.vy += 1;
    if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
        plane.vx -= 1;
    if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
        plane.vx += 1;
    // 0.71 is (1/sqrt(2)).
    plane.y += plane.vy * 4 * (plane.vx ? 0.71f : 1);
    plane.x += plane.vx * 4 * (plane.vy ? 0.71f : 1);
    // [HACKATHON 1-1]
    // TODO: Limit the plane's collision box inside the frame.
    //       (x, y axes can be separated.)
    // Uncomment and fill in the code below.
    if (plane.x - plane.w / 2 < 0)
        plane.x = 0 + plane.w / 2;
    else if (plane.x + plane.w / 2 > SCREEN_W)
        plane.x = SCREEN_W - plane.w / 2;
    if (plane.y - plane.h / 2 < 0)
        plane.y = plane.h / 2;
    else if (plane.y + plane.h / 2 > SCREEN_H)
        plane.y = SCREEN_H - plane.w / 2;
    // [HACKATHON 2-6]
    // TODO: Update bullet coordinates.
    // 1) For each bullets, if it's not hidden, update x, y
    // according to vx, vy.
    // 2) If the bullet is out of the screen, hide it.
    // Uncomment and fill in the code below.
    int i;
    for (i = 0; i < MAX_BULLET; i++) {
        if (bullets[i].hidden)
            continue;
        bullets[i].x += bullets[i].vx;
        bullets[i].y += bullets[i].vy;
        if (bullets[i].y <= 0)
            bullets[i].hidden = true;
    }
    for (int i = 0; i < MAX_ENEMY_SHOW; i++) {
        int ra = rand();
        if (enemies_show[i])check_enemis(enemies[i], i);
        else if (ra % 500 == 0)born(enemies[i], i);
    }
    // [HACKATHON 2-7]
    // TODO: Shoot if key is down and cool-down is over.
    // 1) Get the time now using 'al_get_time'.
    // 2) If Space key is down in 'key_state' and the time
    //    between now and last shoot is not less that cool
    //    down time.
    // 3) Loop through the bullet array and find one that is hidden.
    //    (This part can be optimized.)
    // 4) The bullet will be found if your array is large enough.
    // 5) Set the last shoot time to now.
    // 6) Set hidden to false (recycle the bullet) and set its x, y to the
    //    front part of your plane.
    // Uncomment and fill in the code below.
    if ((key_state[ALLEGRO_KEY_SPACE] || key_state[175])&& now - last_shoot_timestamp >= MAX_COOLDOWN) {
        for (i = 0; i < MAX_BULLET; i++) {
            if (bullets[i].hidden) {
                last_shoot_timestamp = now;
                bullets[i].hidden = 0;
                bullets[i].x = plane.x;
                bullets[i].y = plane.y - plane.h / 2;
                break;
            }
        }
    }

}

static void draw_movable_object(MovableObject obj) {
    if (obj.hidden)
        return;
    al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
    if (draw_gizmos) {
        al_draw_rectangle(round(obj.x - obj.w / 2), round(obj.y - obj.h / 2),
            round(obj.x + obj.w / 2) + 1, round(obj.y + obj.h / 2) + 1, al_map_rgb(255, 0, 0), 0);
    }
}

static void draw(void) {
    int i;
    al_draw_bitmap(img_background, 0, 0, 0);
    // [HACKATHON 2-8]
    // TODO: Draw all bullets in your bullet array.
    // Uncomment and fill in the code below.
    for (int i = 0 ; i < MAX_BULLET ; i++)
        if(!bullets[i].hidden)
            al_draw_bitmap(bullets[i].img, bullets[i].x, bullets[i].y, 0);
    draw_movable_object(plane);
    for(int q = 0 ; q < MAX_ENEMY_SHOW ; q++)
        if(enemies_show[q])
            for (i = 0; i < MAX_ENEMY; i++)
                draw_movable_object(enemies[q][i]);
}

static void destroy(void) {
    al_destroy_bitmap(img_background);
    al_destroy_bitmap(img_plane);
    al_destroy_bitmap(img_enemy);
    al_destroy_sample(bgm);
    // [HACKATHON 2-9]
    // TODO: Destroy your bullet image.
    // Uncomment and fill in the code below.
    al_destroy_bitmap(img_bullet);
    stop_bgm(bgm_id);
    game_log("Start scene destroyed");
}

static void on_key_down(int keycode) {
    if (keycode == ALLEGRO_KEY_TAB)
        draw_gizmos = !draw_gizmos;
}

// TODO: Add more event callback functions such as keyboard, mouse, ...

// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_start_create(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    scene.name = "Start";
    scene.initialize = &init;
    scene.update = &update;
    scene.draw = &draw;
    scene.destroy = &destroy;
    scene.on_key_down = &on_key_down;
    // TODO: Register more event callback functions such as keyboard, mouse, ...
    game_log("Start scene created");
    return scene;
}
