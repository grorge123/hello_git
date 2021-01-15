#include "game.h"
#include "scene_start.h"
#include "utility.h"
#include "shared.h"
#include "scene_menu.h"
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

static ALLEGRO_BITMAP* img_background;
static ALLEGRO_BITMAP* img_plane[2];
static ALLEGRO_BITMAP* img_enemy[2];

bool player_double = false;
char plane_img[2][50] = {".\\img\\plane.png", ".\\img\\plane.png"};

static ALLEGRO_BITMAP* img_bullet;
static ALLEGRO_BITMAP* img_bullet_enemies;

typedef struct MovableObject MovableObject;
typedef struct LinkListMovableObject LinkListMovableObject;
typedef int(*change_v)(MovableObject now, int data[]);

struct MovableObject{
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
    int max_hp;
    int type;
    int data[10];
    change_v cvx;
    change_v cvy;
};


struct LinkListMovableObject{
    LinkListMovableObject *last, *next;
    MovableObject val;
};

static void init(void);
static void update(void);
static void draw_movable_object(MovableObject obj);
static void draw(void);
static void destroy(void);

#define MAX_enemy_1_size 8
#define MAX_BULLET 4

static MovableObject plane[2];
static MovableObject bullets[2][MAX_BULLET];

static const float MAX_COOLDOWN = 0.2f;
static double last_shoot_timestamp[2];

static ALLEGRO_SAMPLE* bgm;
static ALLEGRO_SAMPLE_ID bgm_id;
static bool draw_gizmos;
static int score;

LinkListMovableObject *enemies;

void to_string(char *s, int num){
    char tmp[64] = {};
    int now = 0, start = strlen(s);
    while (num > 0){
        tmp[now] = (num%10) + '0';
        num /= 10;
        now++;
    }
    if(now == 0){
        tmp[now] = '0';
        now++;
    }
    for(int i = 1; i <= now ; i++){
        s[start + i - 1] = tmp[now - i];
    }
    return;
}

void clear_link_list(LinkListMovableObject *now){
    if(now == NULL)return;
    clear_link_list(now->next);
    free(now);
    return;
}
static LinkListMovableObject *tail = NULL;
LinkListMovableObject* find_tail(LinkListMovableObject *now){
    if(tail == NULL){
        tail = now;
    }
    while(tail->next != NULL){
        tail = tail->next;
    }
    return tail;
}

static void init(void) {
    score = 0;
    if(!img_background)
        img_background = load_bitmap_resized(".\\img\\start-bg.jpg", SCREEN_W, SCREEN_H);
    clear_link_list(enemies);
    img_plane[0] = plane[0].img = load_bitmap(plane_img[0]);
    img_plane[1] = plane[1].img = load_bitmap(plane_img[1]);
    plane[0].x = 200;
    plane[0].y = 500;
    plane[1].x = 600;
    plane[1].y = 500;
    for(int i = 0 ; i < 2 ; i++){
        plane[i].w = al_get_bitmap_width(plane[i].img);
        plane[i].h = al_get_bitmap_height(plane[i].img);
        plane[i].hp = plane[i].max_hp = 5;
        plane[i].hidden = false;
    }
    if(!img_enemy[0])
        img_enemy[0] = load_bitmap(".\\img\\rocket-4.png");
    if(!img_bullet)
        img_bullet = load_bitmap(".\\img\\image12.png");
    if(!img_bullet_enemies)
        img_bullet_enemies = load_bitmap(".\\img\\rocket-1.png");
    for(int q = 0 ; q < 2 ; q++){
        for (int i = 0; i < MAX_BULLET; i++) {
            bullets[q][i].img = img_bullet;
            bullets[q][i].w = al_get_bitmap_width(img_bullet);
            bullets[q][i].h = al_get_bitmap_height(img_bullet);
            bullets[q][i].vx = 0;
            bullets[q][i].vy = -3;
            bullets[q][i].hidden = true;
        }
    }
    enemies = NULL;
    if(!player_double)plane[1].hidden = true;
    // Can be moved to shared_init to decrease loading time.
//    if(!bgm)
//        bgm = load_audio(".\\img\\mythica.ogg");
    game_log("Start scene initialized");
//    bgm_id = play_bgm(bgm, 1);
}
bool collision(MovableObject a, MovableObject b) {
    if(abs(a.x - b.x) * abs(a.x - b.x) + abs(a.y-b.y) * abs(a.y - b.y) <= max(max(a.w, a.h), max(b.w, b.h)) * max(max(a.w, a.h), max(b.w, b.h)))
        return true;
    return false;
}


int type_1_change_v_x(MovableObject now, int data[]){
    return (int)(data[0] + cos(now.y / 10) * 15) - now.x;
}
void born(int type) {
    if(type == 0){
        int aixl_y = 0, aixl_x = 15 + (rand() % (SCREEN_W - 30));
        for(int i = 0 ; i < MAX_enemy_1_size ; i++, aixl_y -= 100){
            LinkListMovableObject *new_enemy_p = malloc(sizeof(LinkListMovableObject));
            MovableObject *enemy = malloc(sizeof(MovableObject));
            enemy->img = img_enemy[0];
            enemy->hp = 2;
            enemy->hidden = false;
            enemy->x = aixl_x;
            enemy->y = aixl_y;
            enemy->w = al_get_bitmap_width(img_enemy[0]);
            enemy->h = al_get_bitmap_height(img_enemy[0]);
            enemy->vx = 0;
            enemy->vy = 1;
            enemy->data[0] = aixl_x;
            enemy->cvx = &type_1_change_v_x;
            enemy->cvy = NULL;
            enemy->type = 0;
            new_enemy_p->val = *enemy;
            new_enemy_p->last = new_enemy_p->next = NULL;
            if(enemies == NULL){
                enemies = new_enemy_p;
            }else{
                tail = find_tail(enemies);
                tail->next = new_enemy_p;
                new_enemy_p->last = tail;
            }
        }
    }
}

bool change_state(MovableObject *now){
    if(now->cvx)
        now->vx = now->cvx(*now, now->data);
    if(now->cvy)
        now->vy = now->cvy(*now, now->data);
    now->x += now->vx;
    now->y += now->vy;
    for(int i = 0 ; i < 2 ; i++){
        for(int q = 0 ; q < MAX_BULLET ; q++ ){
            if(bullets[i][q].hidden == false && collision(*now, bullets[i][q])){
                bullets[i][q].hidden = true;
                now->hp--;
                if(now->hp <= 0)return true;
            }
        }
    }
    for(int i = 0 ; i < 2 ; i++){
        if(plane[i].hidden == false && collision(plane[i], *now)){
            plane[i].hp -= now->hp;
            now->hp = 0;
            return true;
        }
    }
    return false;
}

static void update(void) {
    double now = al_get_time();
//    if(!((int)now % 5)){
//        if(rand()%50==0)born(0);
//    }
    LinkListMovableObject* now_enemy = enemies;
    plane[0].vx = plane[0].vy = plane[1].vx = plane[1].vy = 0;
    if (key_state[ALLEGRO_KEY_UP])
        plane[0].vy -= 1;
    if (key_state[ALLEGRO_KEY_DOWN])
        plane[0].vy += 1;
    if (key_state[ALLEGRO_KEY_LEFT])
        plane[0].vx -= 1;
    if (key_state[ALLEGRO_KEY_RIGHT])
        plane[0].vx += 1;
    if (key_state[ALLEGRO_KEY_W])
        plane[1].vy -= 1;
    if (key_state[ALLEGRO_KEY_S])
        plane[1].vy += 1;
    if (key_state[ALLEGRO_KEY_A])
        plane[1].vx -= 1;
    if (key_state[ALLEGRO_KEY_D])
        plane[1].vx += 1;
    for(int i = 0 ; i < 2 ; i++){
        plane[i].y += plane[i].vy * 4 * (plane[i].vx ? 0.71f : 1);
        plane[i].x += plane[i].vx * 4 * (plane[i].vy ? 0.71f : 1);
        if (plane[i].x - plane[i].w / 2 < 0)
            plane[i].x = 0 + plane[i].w / 2;
        else if (plane[i].x + plane[i].w / 2 > SCREEN_W)
            plane[i].x = SCREEN_W - plane[i].w / 2;
        if (plane[i].y - plane[i].h / 2 < 0)
            plane[i].y = plane[i].h / 2;
        else if (plane[i].y + plane[i].h / 2 > SCREEN_H)
            plane[i].y = SCREEN_H - plane[i].w / 2;
    }
    return;
    while(now_enemy != NULL){
        if(change_state(&(now_enemy->val))){
            if(now_enemy == tail){
                tail = now_enemy->last;
            }
            if(now_enemy->val.type == 0)score += 10;
            if(now_enemy->last != NULL && now_enemy->next != NULL){
                now_enemy->next->last = now_enemy->last;
                now_enemy->last->next = now_enemy->next;
            }else if(now_enemy->last == NULL && now_enemy->next != NULL){
                enemies = now_enemy->next;
                now_enemy->next->last = NULL;
            }else if(now_enemy->last != NULL && now_enemy->next == NULL){
                now_enemy->last->next = NULL;
            }else{
                enemies = NULL;
            }
            LinkListMovableObject *tmp = now_enemy->next;
            free(now_enemy);
            now_enemy = tmp;
        }else{
            now_enemy = now_enemy->next;
        }
    }
    for(int q = 0 ; q < 2 ; q++){
        for (int i = 0; i < MAX_BULLET; i++) {
            if (bullets[q][i].hidden)
                continue;
            bullets[q][i].x += bullets[q][i].vx;
            bullets[q][i].y += bullets[q][i].vy;
            if (bullets[q][i].y <= 0)
                bullets[q][i].hidden = true;
        }
    }
    if (((key_state[ALLEGRO_KEY_K]) && now - last_shoot_timestamp[0] >= MAX_COOLDOWN) && (plane[0].hp > 0 && !plane[0].hidden)) {
        for (int i = 0; i < MAX_BULLET; i++) {
            if (bullets[0][i].hidden) {
                last_shoot_timestamp[0] = now;
                bullets[0][i].hidden = 0;
                bullets[0][i].x = plane[0].x;
                bullets[0][i].y = plane[0].y - plane[0].h / 2;
                break;
            }
        }
    }
    if (((key_state[ALLEGRO_KEY_SPACE] || key_state[175])&& now - last_shoot_timestamp[1] >= MAX_COOLDOWN) && (plane[1].hp > 0 && !plane[1].hidden)) {
        for (int i = 0; i < MAX_BULLET; i++) {
            if (bullets[1][i].hidden) {
                last_shoot_timestamp[1] = now;
                bullets[1][i].hidden = 0;
                bullets[1][i].x = plane[1].x;
                bullets[1][i].y = plane[1].y - plane[1].h / 2;
                break;
            }
        }
    }
    for(int i = 0 ; i < 2 ; i++){
        if(plane[i].hp <= 0){
            plane[i].hidden = true;
        }
    }
    if(plane[0].hidden && plane[1].hidden){
        start_scene.init = false;
        game_change_scene(&gameover_scene);
    }else if(score >= 100){
        start_scene.init = false;
        game_change_scene(&gamewin_scene);
    }
}

static void draw_movable_object(MovableObject obj) {
    if (obj.hidden)
        return;
    al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
    if (draw_gizmos) {
        // al_draw_rectangle(round(obj.x - obj.w / 2), round(obj.y - obj.h / 2),
        //     round(obj.x + obj.w / 2) + 1, round(obj.y + obj.h / 2) + 1, al_map_rgb(255, 0, 0), 0);
        al_draw_circle(obj.x, obj.y, max(obj.w, obj.h)/2, al_map_rgb(255, 0, 0), 0);
    }
}
static void draw(void) {
    al_draw_bitmap(img_background, 0, 0, 0);
    char score_char[10] = {"Score:"};
//    to_string(score_char, score);
    for(int i = 0 ; i < 2 ; i++){
        if(plane[i].hp <= 0 || plane[i].hidden)continue;
//        char output[50] = {};
//        to_string(output, plane[i].hp);
//        output[strlen(output)] = '/';
//        to_string(output, plane[i].max_hp);
//        al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), plane[i].x, plane[i].y - 65, ALLEGRO_ALIGN_CENTER, output);
        draw_movable_object(plane[i]);
    }
    return;
    al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), 130, 10, ALLEGRO_ALIGN_CENTER, score_char);
    for(int q = 0 ; q < 2 ; q++)
        for (int i = 0 ; i < MAX_BULLET ; i++)
            if(!bullets[q][i].hidden)
                draw_movable_object(bullets[q][i]);
    LinkListMovableObject *now_enemy = enemies;
    while(now_enemy != NULL){
        draw_movable_object(now_enemy->val);
        now_enemy = now_enemy->next;
    }
}

static void destroy(void) {
    al_destroy_bitmap(img_background);
    al_destroy_bitmap(img_plane[0]);
    al_destroy_bitmap(img_plane[1]);
    al_destroy_bitmap(img_enemy[0]);
    al_destroy_bitmap(img_enemy[1]);
    al_destroy_sample(bgm);
    al_destroy_bitmap(img_bullet);
//    stop_bgm(bgm_id);
    game_log("Start scene destroyed");
}

static void on_key_down(int keycode) {
    if (keycode == ALLEGRO_KEY_TAB)
        draw_gizmos = !draw_gizmos;
}


void scene_start_create(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    scene.name = "Start";
    scene.initialize = &init;
    scene.update = &update;
    scene.draw = &draw;
    scene.destroy = &destroy;
    scene.on_key_down = &on_key_down;
    scene.initialize();
    scene.init = false;
    start_scene = scene;
    game_log("Start scene created");
    return;
}
