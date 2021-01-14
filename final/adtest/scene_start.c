#include "scene_start.h"
#include "scene_menu.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "utility.h"
#include "enemy_spawn.h"
#include <math.h>

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
static ALLEGRO_BITMAP* img_enemy_P2;
// [HACKATHON 2-1]
// TODO: Declare a variable to store your bullet's image.
static ALLEGRO_BITMAP* img_bullet;
static ALLEGRO_BITMAP* img_enemy_bullet;

static void init(void);
static void update(void);
static void draw_movable_object(MovableObject obj);
static void draw(void);
static void destroy(void);

#define MAX_ENEMY 4
#define MAX_ENEMY_BULLET 8
#define MAX_ENEMY_TYPE 2
// [HACKATHON 2-2]
// TODO: Declare the max bullet count that will show on screen.
#define MAX_BULLET 10
static MovableObject plane;
static MovableObject enemies[MAX_ENEMY];
static MovableObject enemies_Horizontal[MAX_ENEMY];
static MovableObject Ghost_enemy1; //support enemies
static MovableObject Ghost_enemy2; //support enemies_Horizontal
// [HACKATHON 2-3]
// TODO: Declare an array to store bullets with size of max bullet count.
static MovableObject bullets[MAX_BULLET];
static MovableObject enemyBullets[MAX_ENEMY_BULLET];
// [HACKATHON 2-4]
// TODO: Set up bullet shooting cool-down variables.
static const float MAX_COOLDOWN = 0.2f;
static const float MAX_ENEMY_COOLDOWN = 2.5f;
static double last_shoot_timestamp;
static double last_enemyshoot_timestamp;
//bgm
static ALLEGRO_SAMPLE* bgm;
static ALLEGRO_SAMPLE_ID bgm_id;
//debug support
static bool draw_gizmos;
//enemy move support
static double initX_0;
static double initY_0;
//static int enemies_Destroyed[MAX_ENEMY_TYPE];
static int enemies_Destroyed;
static int nowE;

static void init(void) {
    int i;
    //background init
    img_background = load_bitmap_resized("./Assets/start-bg.jpg", SCREEN_W, SCREEN_H);
    //plane init
    img_plane = plane.img = load_bitmap("./Assets/plane.png");
    plane.x = 400;
    plane.y = 500;
    plane.w = al_get_bitmap_width(plane.img);
    plane.h = al_get_bitmap_height(plane.img);
    plane.hp = 3;
    //enemy type init
    nowE = rand() % MAX_ENEMY_TYPE;
    //enemy init
    for(i = 0; i < MAX_ENEMY_TYPE; i++) enemies_Destroyed = 0;

    img_enemy = load_bitmap("./Assets/smallfighter0006.png");
    for (i = 0; i < MAX_ENEMY; i++) {
        enemies[i].img = img_enemy;
        enemies[i].w = al_get_bitmap_width(img_enemy);
        enemies[i].h = al_get_bitmap_height(img_enemy);
        if(i == 0){
            enemies[i].y = -enemies[i].h/2;
            enemies[i].x = (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
            initX_0 = enemies[i].x;
        }else{
            enemies[i].y = enemies[i-1].y - 80;
            enemies[i].x = enemies[i-1].x;
        }
        enemies[i].vx = 0;
        enemies[i].vy = 2.5;
        enemies[i].hp = 3;
        enemies[i].hidden = false;
    }
    //ghost 1 init
    Ghost_enemy1.x = initX_0;
    Ghost_enemy1.y = enemies[0].y;
    Ghost_enemy1.vy = 2.5;
    Ghost_enemy1.hidden = true;
    //enemy 2 init
    img_enemy_P2 = load_bitmap("./Assets/rocket-4.png");
    for (i = 0; i < MAX_ENEMY; i++) {
        enemies_Horizontal[i].img = img_enemy_P2;
        enemies_Horizontal[i].w = al_get_bitmap_width(img_enemy_P2);
        enemies_Horizontal[i].h = al_get_bitmap_height(img_enemy_P2);
        if(i == 0){
                enemies_Horizontal[i].x = -enemies_Horizontal[i].w/2;
                enemies_Horizontal[i].y = (rand() % (int)(SCREEN_H/2 - 2 * enemies_Horizontal[i].h)) + enemies_Horizontal[i].h;
                initY_0 = enemies_Horizontal[i].y;
            }else{
                enemies_Horizontal[i].x = enemies_Horizontal[i-1].x - 100;
                enemies_Horizontal[i].y = enemies_Horizontal[i-1].y;
            }
        enemies_Horizontal[i].vx = 1.5;
        enemies_Horizontal[i].vy = 0;
        enemies_Horizontal[i].hp = 3;
        enemies_Horizontal[i].hidden = false;
    }
    //ghost 2 init
    Ghost_enemy2.x = enemies_Horizontal[0].x;
    Ghost_enemy2.y = initY_0;
    Ghost_enemy2.vx = enemies_Horizontal[0].vx;
    Ghost_enemy2.hidden = true;
    // [HACKATHON 2-5]
    // TODO: Initialize bullets.
    img_bullet = load_bitmap("./Assets/image12.png");
    for (i = 0; i < MAX_BULLET; i++) {
        bullets[i].w = al_get_bitmap_width(img_bullet);
        bullets[i].h = al_get_bitmap_height(img_bullet);
        bullets[i].img = img_bullet;
        bullets[i].vx = 0;
        bullets[i].vy = -10;
        bullets[i].hidden = true;
        bullets[i].hp = 1;
    }
    img_enemy_bullet = load_bitmap("./Assets/rocket-1.png");
    for (i = 0; i < MAX_ENEMY_BULLET; i++) {
            enemyBullets[i].w = al_get_bitmap_width(img_enemy_bullet);
            enemyBullets[i].h = al_get_bitmap_height(img_enemy_bullet);
            enemyBullets[i].img = img_enemy_bullet;
            enemyBullets[i].vx = 0;
            enemyBullets[i].vy = 1;
            enemyBullets[i].hidden = true;
            enemyBullets[i].hp = 1;
    }
    // Can be moved to shared_init to decrease loading time.
    bgm = load_audio("./Assets/mythica.ogg");
    game_log("Start scene initialized");
    bgm_id = play_bgm(bgm, 1);
}

static void update(void) {
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
    if (plane.x - plane.w/2< 0)
        plane.x = plane.w/2;
    else if (plane.x + plane.w/2> SCREEN_W)
        plane.x = SCREEN_W - plane.w/2;
    if (plane.y - plane.h/2 < 0)
        plane.y = plane.h/2;
    else if (plane.y + plane.h/2 > SCREEN_H)
        plane.y = SCREEN_H - plane.h/2;
    // [HACKATHON 2-6]
    //bullets move
    int i;
    for (i = 0; i < MAX_BULLET; i++) {
        if (bullets[i].hidden)
            continue;
        bullets[i].x += bullets[i].vx;
        bullets[i].y += bullets[i].vy;
        if(bullets[i].y + bullets[i].h/2 < 0 || bullets[i].hp == 0)
            bullets[i].hidden = true;
    }
    //enemyBullets move
    for (i = 0; i < MAX_ENEMY_BULLET; i++) {
        if (enemyBullets[i].hidden)
            continue;
        enemyBullets[i].x += enemyBullets[i].vx;
        enemyBullets[i].y += enemyBullets[i].vy;
        if (enemyBullets[i].y - enemyBullets[i].h/2 > SCREEN_H || enemyBullets[i].hp == 0)
            enemyBullets[i].hidden = true;
    }
    // [HACKATHON 2-7]
    //plane shoot bullets
    double now = al_get_time();
    if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN) {
        for (i = 0; i < MAX_BULLET; i++) {
            if (bullets[i].hidden) {
                last_shoot_timestamp = now;
                bullets[i].hidden = false;
                bullets[i].x = plane.x;
                bullets[i].y = plane.y - plane.h/2;
                bullets[i].hp = 1;
                break;
            }
        }
    }
    //now enemy
    if(nowE == 0){
        //ghost 1 move
        Ghost_enemy1.y += Ghost_enemy1.vy;
        //Enemy move
        for(i = 0; i < MAX_ENEMY; i++){
            if((enemies[i].hp == 0 && !enemies[i].hidden) || enemies[i].y - enemies[i].h/2 > SCREEN_H){
                enemies[i].hidden = true;
                enemies_Destroyed++;
                game_log("Hey");
            }
            if(enemies[0].y < 300){
                enemies[i].y += enemies[i].vy;
                enemies[i].x = initX_0 + 15*cos(enemies[i].y/25);
            }else{
                if(i == 0) enemies[i].x = initX_0 + 15*cos(Ghost_enemy1.y/25);
                else enemies[i].x = initX_0 + 15*cos((Ghost_enemy1.y-80 * i)/25);
            }

        }
    }else if(nowE == 1){
        //ghost 2 move
        Ghost_enemy2.x += Ghost_enemy2.vx;
        //enemy 2 move
        for(i = 0; i < MAX_ENEMY; i++){
            if((enemies_Horizontal[i].hp == 0 && !enemies_Horizontal[i].hidden) || enemies_Horizontal[i].x - enemies_Horizontal[i].w/2 > SCREEN_W){
                enemies_Horizontal[i].hidden = true;
                enemies_Destroyed++;
                game_log("HeyH");
            }
            if(enemies_Horizontal[0].x < 600){
                //game_log("OA");
                enemies_Horizontal[i].x += enemies_Horizontal[i].vx;
                enemies_Horizontal[i].y = initY_0 + 15*sin(enemies_Horizontal[i].x/25);
            }else{
                if(i == 0) enemies_Horizontal[i].y = initY_0 + 15*sin(Ghost_enemy2.x/25);
                else enemies_Horizontal[i].y = initY_0 + 15*sin((Ghost_enemy2.x - 100 * i)/25);
            }


        }
    }
    //enemyShoot
    if (now - last_enemyshoot_timestamp >= MAX_ENEMY_COOLDOWN) {
        /*for(int j = 0; j < MAX_ENEMY; j++){
            for (i = 0; i < MAX_ENEMY_BULLET; i++) {
                if (enemyBullets[i].hidden && !enemies[j].hidden && enemies[j].y > 0) {
                    last_enemyshoot_timestamp = now;
                    enemyBullets[i].hidden = false;
                    enemyBullets[i].x = enemies[j].x;
                    enemyBullets[i].y = enemies[j].y + enemies[j].h/2;
                    enemyBullets[i].hp = 1;
                    break;
                }
            }
        }*/
        last_enemyshoot_timestamp = now;
        enemy_shoot_bullets(enemies_Horizontal, enemyBullets, MAX_ENEMY, MAX_ENEMY_BULLET);
        enemy_shoot_bullets(enemies, enemyBullets, MAX_ENEMY, MAX_ENEMY_BULLET);
    }

    //if enemy 1 all destroyed
    //if enemy 2 all destroyed
    if(enemies[MAX_ENEMY-1].y - enemies[MAX_ENEMY-1].h/2 > SCREEN_H || \
    enemies_Horizontal[MAX_ENEMY-1].y - enemies_Horizontal[MAX_ENEMY-1].h/2 > SCREEN_W || \
    enemies_Destroyed == MAX_ENEMY ){
        enemies_Destroyed = 0;
        nowE = rand() % MAX_ENEMY_TYPE;
        if(nowE == 0){
            enemy_spawn(enemies, 1, MAX_ENEMY);
            Ghost_enemy1.y = enemies[0].y;
            Ghost_enemy1.x = initX_0 = enemies[0].x;
        }else if(nowE == 1){
            enemy_spawn(enemies_Horizontal, 2, MAX_ENEMY);
            Ghost_enemy2.y = initY_0 = enemies_Horizontal[0].y;
            Ghost_enemy2.x = enemies_Horizontal[0].x;
        }

    }

    //let bullet destroy enemybullet
    enemy_detect_bullet(bullets, enemyBullets, MAX_BULLET, MAX_ENEMY_BULLET);
    //let bullet destroy enemy
    enemy_detect(bullets, enemies, MAX_BULLET, MAX_ENEMY);
    enemy_detect(bullets, enemies_Horizontal, MAX_BULLET, MAX_ENEMY);
    //let enemyBullet destroy player
    enemy_detect_player(&plane, enemies, MAX_ENEMY);
    enemy_detect_player(&plane, enemyBullets, MAX_ENEMY_BULLET);
    enemy_detect_player(&plane, enemies_Horizontal, MAX_ENEMY);

    if(plane.hp == 0){
        game_change_scene(scene_menu_create());
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
    for (i = 0; i < MAX_BULLET; i++)
        draw_movable_object(bullets[i]);
    draw_movable_object(plane);
    for (i = 0; i < MAX_ENEMY; i++)
        draw_movable_object(enemies[i]);
    for (i = 0; i < MAX_ENEMY_BULLET; i++)
        draw_movable_object(enemyBullets[i]);
    for (i = 0; i < MAX_ENEMY; i++){
        draw_movable_object(enemies_Horizontal[i]);
    }
}

static void destroy(void) {
    al_destroy_bitmap(img_background);
    al_destroy_bitmap(img_plane);
    al_destroy_bitmap(img_enemy);
    al_destroy_sample(bgm);
    // [HACKATHON 2-9]
    // TODO: Destroy your bullet image.
    al_destroy_bitmap(img_bullet);
    al_destroy_bitmap(img_enemy_bullet);
    al_destroy_bitmap(img_enemy_P2);
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
