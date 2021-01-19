#include "game.h"
#include "scene_start.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"
#include "move_function.h"
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
static ALLEGRO_BITMAP* img_background2;
static ALLEGRO_BITMAP* img_plane[2];
static ALLEGRO_BITMAP* img_enemy[5];
static ALLEGRO_BITMAP* img_knife;
static ALLEGRO_BITMAP* img_skill_bullet;
static ALLEGRO_BITMAP* img_bullet;
static ALLEGRO_BITMAP* img_bullet_enemies;
static ALLEGRO_BITMAP* img_boss;

bool player_double = false;
char plane_img[2][50] = {".\\img\\plane.png", ".\\img\\plane.png"};


int now_status = 1;

static void init(void);
static void update(void);
static void draw_movable_object(MovableObject obj);
static void draw(void);
static void destroy(void);

#define MAX_enemy_1_size 8
#define MAX_enemy_2_size 8
#define MAX_enemy_3_size 8
#define MAX_enemy_4_size 8
static int enemy_4_size = 0;
#define MAX_BULLET 10

static MovableObject plane[2];
static MovableObject bullets[2][MAX_BULLET];
static MovableObject boss;

static const float MAX_COOLDOWN = 0.2f;
static double last_shoot_timestamp[2];
static const float KNIFE_FINISH = 0.5;
static const float KNIFE_COOLDOWN = 0.8;
static float knife_v = (2 * ALLEGRO_PI) / (KNIFE_FINISH * 144);
static float knife_ange[2] = {};
static float knife_last[2] = {};
static bool knife_hidden[2] = {1,1};
static int skill_COOLDOWN = 3;
static int skill_start = -10;
static int skill_end = -10;
static int skill_prepare = 0;
static double last_skill_shot = 0;
#define skill_bullet_size 100
static MovableObject skill_bullets[skill_bullet_size];
static double boss_invincible = 0;
static int boss_bullets = 10;

static ALLEGRO_SAMPLE* voice_magic;
static ALLEGRO_SAMPLE* voice_knife;
static ALLEGRO_SAMPLE* voice_bomb;
static ALLEGRO_SAMPLE* voice_bigbomb;
static ALLEGRO_SAMPLE* voice_shot;
static ALLEGRO_SAMPLE_ID magic_id;


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
    s[ start + now ] = '\0';
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
    now_status = 1;
    if(!voice_shot)
        voice_shot = load_audio(".\\img\\shot.ogg");
    if(!voice_magic)
        voice_magic = load_audio(".\\img\\magic.ogg");
    if(!voice_bomb)
        voice_bomb = load_audio(".\\img\\bomb.ogg");
    if(!voice_bigbomb)
        voice_bigbomb = load_audio(".\\img\\bigbomb.ogg");
    if(!voice_knife)
        voice_knife = load_audio(".\\img\\knife.ogg");
    if(!img_enemy[0])
        img_enemy[0] = load_bitmap(".\\img\\rocket-4.png");
    if(!img_enemy[1])
        img_enemy[1] = load_bitmap(".\\img\\rocket-3.png");
    if(!img_enemy[2])
        img_enemy[2] = load_bitmap(".\\img\\rocket-1.png");
    if(!img_enemy[3])
        img_enemy[3] = load_bitmap(".\\img\\boss_bullet.png");
    if(!img_bullet)
        img_bullet = load_bitmap(".\\img\\image12.png");
    if(!img_knife)
        img_knife = load_bitmap(".\\img\\knif.png");
    if(!img_bullet_enemies)
        img_bullet_enemies = load_bitmap(".\\img\\rocket-1.png");
    if(!img_skill_bullet)
        img_skill_bullet = load_bitmap(".\\img\\skill_bullet.png");
    if(!img_boss)
        img_boss = load_bitmap(".\\img\\rocket-2.png");
    if(!img_background2)
        img_background2 = load_bitmap_resized(".\\img\\star-bg.jpg", SCREEN_W, SCREEN_H);
    img_background = load_bitmap_resized(".\\img\\start-bg.jpg", SCREEN_W, SCREEN_H);
    clear_link_list(enemies);
    img_plane[0] = plane[0].img = load_bitmap(plane_img[0]);
    img_plane[1] = plane[1].img = load_bitmap(plane_img[1]);
    plane[0].x = 200;
    plane[0].y = 500;
    plane[1].x = 600;
    plane[1].y = 500;
    boss.hidden = true;
    boss.hp = boss.max_hp = 800;
    boss.x = SCREEN_W / 2;
    boss.y = SCREEN_H / 2;
    boss.img = img_boss;
    boss.w = al_get_bitmap_width(boss.img);
    boss.h = al_get_bitmap_height(boss.img);

    for(int i = 0 ; i < 2 ; i++){
        plane[i].w = al_get_bitmap_width(plane[i].img);
        plane[i].h = al_get_bitmap_height(plane[i].img);
        plane[i].hp = plane[i].max_hp = 600;
        plane[i].hidden = false;
    }
    skill_prepare = 0;
    for(int i = 0 ; i < skill_bullet_size ; i++){
        skill_bullets[i].hidden = true;
        skill_bullets[i].y = SCREEN_H;
        skill_bullets[i].x = (SCREEN_W / skill_bullet_size) * i;
        skill_bullets[i].vx = skill_bullets[i].vy = 0;
        skill_bullets[i].img = img_skill_bullet;
        skill_bullets[i].w = al_get_bitmap_width(skill_bullets[i].img);
        skill_bullets[i].h = al_get_bitmap_height(skill_bullets[i].img);
        skill_bullets[i].type = -1;
    }
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
    tail = NULL;
    if(!player_double)plane[1].hidden = true;
    game_log("Start scene initialized");
}
bool collision(MovableObject a, MovableObject b) {
    if(abs(a.x - b.x) * abs(a.x - b.x) + abs(a.y-b.y) * abs(a.y - b.y) <= max(max(a.w, a.h), max(b.w, b.h)) * max(max(a.w, a.h), max(b.w, b.h)))
        return true;
    return false;
}

static void born(int type) {
    if(type == 1){
        int aixl_y = 0, aixl_x = 30 + (rand() % (SCREEN_W - 60));
        for(int i = 0 ; i < MAX_enemy_1_size ; i++, aixl_y -= 100){
            LinkListMovableObject *new_enemy_p = malloc(sizeof(LinkListMovableObject));
            MovableObject *enemy = malloc(sizeof(MovableObject));
            enemy->img = img_enemy[0];
            enemy->hp = 2;
            enemy->val = 10;
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
            enemy->type = 1;
            new_enemy_p->val = *enemy;
            new_enemy_p->last = new_enemy_p->next = NULL;
            if(enemies == NULL){
                enemies = new_enemy_p;
            }else{
                if(tail == NULL)
                    tail = find_tail(enemies);
                tail->next = new_enemy_p;
                new_enemy_p->last = tail;
                tail = new_enemy_p;
            }
        }
    }else if(type == 2){
        int aixl_y = 30 + (rand() % (SCREEN_H - 60)), aixl_x = 0;
        for(int i = 0 ; i < MAX_enemy_2_size ; i++, aixl_x -= 100){
            LinkListMovableObject *new_enemy_p = malloc(sizeof(LinkListMovableObject));
            MovableObject *enemy = malloc(sizeof(MovableObject));
            enemy->img = img_enemy[0];
            enemy->hp = 2;
            enemy->val = 10;
            enemy->hidden = false;
            enemy->x = aixl_x;
            enemy->y = aixl_y;
            enemy->w = al_get_bitmap_width(img_enemy[0]);
            enemy->h = al_get_bitmap_height(img_enemy[0]);
            enemy->vx = 1;
            enemy->vy = 0;
            enemy->data[0] = aixl_y;
            enemy->cvx = NULL;
            enemy->cvy = &type_2_change_v_y;
            enemy->type = 2;
            new_enemy_p->val = *enemy;
            new_enemy_p->last = new_enemy_p->next = NULL;
            if(enemies == NULL){
                enemies = new_enemy_p;
            }else{
                if(tail == NULL)
                    tail = find_tail(enemies);
                tail->next = new_enemy_p;
                new_enemy_p->last = tail;
                tail = new_enemy_p;
            }
        }
    }else if(type == 3){
        int aixl_y = 30 + (rand() % (SCREEN_H - 60)), aixl_x = SCREEN_W;
        for(int i = 0 ; i < MAX_enemy_3_size ; i++, aixl_x += 100){
            LinkListMovableObject *new_enemy_p = malloc(sizeof(LinkListMovableObject));
            MovableObject *enemy = malloc(sizeof(MovableObject));
            enemy->img = img_enemy[0];
            enemy->hp = 2;
            enemy->val = 10;
            enemy->hidden = false;
            enemy->x = aixl_x;
            enemy->y = aixl_y;
            enemy->w = al_get_bitmap_width(img_enemy[0]);
            enemy->h = al_get_bitmap_height(img_enemy[0]);
            enemy->vx = -1;
            enemy->vy = 0;
            enemy->data[0] = aixl_y;
            enemy->cvx = NULL;
            enemy->cvy = &type_3_change_v_y;
            enemy->type = 3;
            new_enemy_p->val = *enemy;
            new_enemy_p->last = new_enemy_p->next = NULL;
            if(enemies == NULL){
                enemies = new_enemy_p;
            }else{
                if(tail == NULL)
                    tail = find_tail(enemies);
                tail->next = new_enemy_p;
                new_enemy_p->last = tail;
                tail = new_enemy_p;
            }
        }
    }
    if(type == 4){
        LinkListMovableObject *new_enemy_p = malloc(sizeof(LinkListMovableObject));
        MovableObject *enemy = malloc(sizeof(MovableObject));
        enemy->img = img_enemy[1];
        enemy->hp = 4;
        enemy->val = 20;
        enemy->hidden = false;
        enemy->x = 30 + (rand() % (SCREEN_W - 60));
        enemy->y = 100;
        enemy->w = al_get_bitmap_width(enemy->img);
        enemy->h = al_get_bitmap_height(enemy->img);
        enemy->vx = 0;
        enemy->vy = 0;
        enemy->data[0] = 1;
        enemy->data[1] = 0;
        enemy->data[2] = rand() % 3 + 1;
        enemy->cvx = &type_4_change_v_x;
        enemy->cvy = NULL;
        enemy->type = 4;
        new_enemy_p->val = *enemy;
        new_enemy_p->last = new_enemy_p->next = NULL;
        if(enemies == NULL){
            enemies = new_enemy_p;
        }else{
            if(tail == NULL)
                tail = find_tail(enemies);
            tail->next = new_enemy_p;
            new_enemy_p->last = tail;
            tail = new_enemy_p;
        }
    }
}
static bool change_state(MovableObject *now, double now_time){
    if(now_status == 2){
        for(int i = 0 ; i < skill_bullet_size ; i++){
            if(!skill_bullets[i].hidden && collision(*now, skill_bullets[i])){
                skill_bullets[i].hidden = true;
                now->hp--;
                if(now->hp <= 0){
                    score += now->val;
                    return true;
                }
            }
        }
        return false;
    }
    if(now->cvx)
        now->vx = now->cvx(*now, now->data);
    if(now->cvy)
        now->vy = now->cvy(*now, now->data);
    now->x += now->vx;
    now->y += now->vy;
//    printf("%lf %lf %lf %lf\n", now->x, now->y, now->vx, now->vy);
    if(now->type == 4 && (int)now_time % (int)now->data[2] == 0 && (int)now_time - now->data[1] >= 1){
        now->data[1] = now_time;
        int aixl_y = now->y, aixl_x = now->x;
        int bullet_size = 10;
        double l = -1, r = 1;
        double k = l;
        for(int i = 0 ; i < bullet_size ; i++, k += (r-l) / bullet_size){
            LinkListMovableObject *new_enemy_p = malloc(sizeof(LinkListMovableObject));
            MovableObject *enemy = malloc(sizeof(MovableObject));
            enemy->img = img_enemy[2];
            enemy->hp = 1;
            enemy->val = 0;
            enemy->hidden = false;
            enemy->x = aixl_x;
            enemy->y = aixl_y;
            enemy->w = al_get_bitmap_width(enemy->img);
            enemy->h = al_get_bitmap_height(enemy->img);
            enemy->vx = k;
            enemy->vy = 1;
            enemy->cvx = NULL;
            enemy->cvy = NULL;
            enemy->type = 5;
            new_enemy_p->val = *enemy;
            new_enemy_p->last = new_enemy_p->next = NULL;
            if(enemies == NULL){
                enemies = new_enemy_p;
            }else{
                if(tail == NULL)
                    tail = find_tail(enemies);
                tail->next = new_enemy_p;
                new_enemy_p->last = tail;
                tail = new_enemy_p;
            }
        }
    }
    for(int i = 0 ; i < 2 ; i++){
        MovableObject tmp = plane[i];
        tmp.w *= 3;
        tmp.h *= 3;
        if(!knife_hidden[i] && collision(*now, tmp)){
            score += now->val;
            return true;
        }
    }
    for(int i = 0 ; i < 2 ; i++){
        for(int q = 0 ; q < MAX_BULLET ; q++ ){
            if(bullets[i][q].hidden == false && collision(*now, bullets[i][q])){
                bullets[i][q].hidden = true;
                now->hp--;
                if(now->hp <= 0){
                    score += now->val;
                    return true;
                }
            }
        }
    }
    for(int i = 0 ; i < 2 ; i++){
        if(plane[i].hidden == false && collision(plane[i], *now)){
            if(now->type == 6 && rand() % 10 == 0){
                plane[i].x = rand() % SCREEN_W;
                plane[i].y = rand() % SCREEN_H;
            }
            plane[i].hp -= now->hp;
            now->hp = 0;
            score += now->val;
            return true;
        }
    }
    if(now->type == 1 && now->y > SCREEN_H)return true;
    else if(now->type == 2 && now->x > SCREEN_W)return true;
    else if(now->type == 3 && now->x < 0)return true;
    else if(now->type == 5 && (now->x < 0 || now->x > SCREEN_W || now->y < 0 ||now->y > SCREEN_H))return true;
    else if(now->type == 6 && (now->data[2] <= 0 || now->data[3] <= 0))return true;
    return false;
}
static void update_move(double now){
    //plane operate
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
    LinkListMovableObject* now_enemy = enemies;
    while(now_enemy != NULL){
        if(change_state(&(now_enemy->val), now)){
            if(now_enemy->val.type == 4)enemy_4_size--;
            if(now_enemy == tail){
                tail = now_enemy->last;
            }
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
    //move bullet
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
    //move knife
    for(int i = 0 ; i < 2 ; i++){
        if(!knife_hidden[i]){
            knife_ange[i] += knife_v;
            if(knife_ange[i] > 2 * ALLEGRO_PI){
                knife_hidden[i] = true;
                knife_last[i] = now;
            }
        }
    }
    //player0 shot
    if (((key_state[ALLEGRO_KEY_K]) && now - last_shoot_timestamp[0] >= MAX_COOLDOWN) && (plane[0].hp > 0 && !plane[0].hidden)) {
        for (int i = 0; i < MAX_BULLET; i++) {
            if (bullets[0][i].hidden) {
                last_shoot_timestamp[0] = now;
                bullets[0][i].hidden = 0;
                bullets[0][i].x = plane[0].x;
                bullets[0][i].y = plane[0].y - plane[0].h / 2;
                play_audio(voice_shot, 1);
                break;
            }
        }
    }
    //player1 shot
    if (((key_state[ALLEGRO_KEY_SPACE] || key_state[175])&& now - last_shoot_timestamp[1] >= MAX_COOLDOWN) && (plane[1].hp > 0 && !plane[1].hidden)) {
        for (int i = 0; i < MAX_BULLET; i++) {
            if (bullets[1][i].hidden) {
                last_shoot_timestamp[1] = now;
                bullets[1][i].hidden = 0;
                bullets[1][i].x = plane[1].x;
                bullets[1][i].y = plane[1].y - plane[1].h / 2;
                play_audio(voice_shot, 1);
                break;
            }
        }
    }
    //player0 knife
    if((key_state[ALLEGRO_KEY_RSHIFT] && now - knife_last[0] > KNIFE_COOLDOWN) && knife_hidden[0] && (plane[0].hp > 0 && !plane[0].hidden)){
        knife_hidden[0] = false;
        knife_ange[0] = 0;
        play_audio(voice_knife, 1);
    }
    //player1 knife
    if((key_state[ALLEGRO_KEY_LSHIFT] && now - knife_last[1] > KNIFE_COOLDOWN) && knife_hidden[1] && (plane[1].hp > 0 && !plane[1].hidden)){
        knife_hidden[1] = false;
        knife_ange[1] = 0;
        play_audio(voice_knife, 1);
    }
    //skill launch
    if(key_state[ALLEGRO_KEY_H] && now - skill_end > skill_COOLDOWN && ((boss.hidden && score >= 300) || (!boss.hidden && score >= 800))){
        skill_start = now;
        skill_end = now + 10;
        now_status = 2;
        score -= (boss.hidden?300:800);
        magic_id = play_bgm(voice_magic, 1);
    }
}
static void check_state(void){
    //move enemy
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
    if(score >= 1000){
        if(boss.hidden){
            boss.hidden = false;
            img_background = img_background2;
            stop_bgm(bgm_id);
            bgm_id = play_audio(bgm_boss, 1);
        }
    }
    for(int i = 0 ; i < 2 ; i++){
        if(plane[i].hp <= 0){
            plane[i].hidden = true;
        }
    }
    if(plane[0].hidden && plane[1].hidden){
        start_scene.init = false;
        stop_bgm(bgm_id);
        bgm_id = play_bgm(bgm_menu, 1);
        game_change_scene(&gameover_scene);
    }else if(boss.hp <= 0){
        start_scene.init = false;
        stop_bgm(bgm_id);
        bgm_id = play_bgm(bgm_menu, 1);
        game_change_scene(&gamewin_scene);
    }
}
static void boss_attack(void){
    boss.x = rand() % SCREEN_W;
    boss.y = rand() % SCREEN_H;
    int aixl_y = boss.y, aixl_x = boss.x;
    double k = 0, v = 1 + rand() % 10;
    for(int i = 0 ; i < boss_bullets ; i++, k += 360 / boss_bullets){
        LinkListMovableObject *new_enemy_p = malloc(sizeof(LinkListMovableObject));
        MovableObject *enemy = malloc(sizeof(MovableObject));
        enemy->img = img_enemy[3];
        enemy->hp = 1;
        enemy->val = 0;
        enemy->hidden = false;
        enemy->x = aixl_x;
        enemy->y = aixl_y;
        enemy->w = al_get_bitmap_width(enemy->img);
        enemy->h = al_get_bitmap_height(enemy->img);
        enemy->vx = cos(k / 180 * ALLEGRO_PI) * v;
        enemy->vy = sin(k / 180 * ALLEGRO_PI) * v;
        enemy->data[0] = enemy->vx;
        enemy->data[1] = enemy->vy;
        enemy->data[2] = 2;
        enemy->data[3] = 2;
        enemy->cvx = &type_6_change_v_x;
        enemy->cvy = &type_6_change_v_y;
        enemy->type = 6;
        new_enemy_p->val = *enemy;
        new_enemy_p->last = new_enemy_p->next = NULL;
        if(enemies == NULL){
            enemies = new_enemy_p;
        }else{
            if(tail == NULL)
                tail = find_tail(enemies);
            tail->next = new_enemy_p;
            new_enemy_p->last = tail;
            tail = new_enemy_p;
        }
    }
}
static void boss_update(double now){
    if(!now_status || boss.hidden || boss.hp < 0)return;
    if((int)now % 3 == 0){
        boss.x = rand() % SCREEN_W;
        boss.y = rand() % SCREEN_H;
    }
    if(now_status == 2){
        for(int i = 0 ; i < skill_bullet_size ; i++){
            if(!skill_bullets[i].hidden && collision(boss, skill_bullets[i])){
                boss.hp--;
                boss_attack();
                skill_bullets[i].hidden = true;
            }
        }
        return;
    }
    for(int i = 0 ; i < 2 ; i++){
        MovableObject tmp = plane[i];
        tmp.w *= 3;
        tmp.h *= 3;
        if(!knife_hidden[i] && collision(boss, tmp)){
            if(now - boss_invincible >= 0.2){
                boss_invincible = now;
                boss.hp--;
                play_audio(voice_bomb, 1);
                boss_attack();
            }
        }
    }
    for(int i = 0 ; i < 2 ; i++){
        for(int q = 0 ; q < MAX_BULLET ; q++ ){
            if(bullets[i][q].hidden == false && collision(boss, bullets[i][q])){
                bullets[i][q].hidden = true;
                boss.hp -= 5;
                play_audio(voice_bomb, 1);
                boss_attack();
            }
        }
    }
    for(int i = 0 ; i < 2 ; i++){
        if(plane[i].hidden == false && collision(plane[i], boss)){
            plane[i].hp -= 5;
            boss.hp -= 5;
            boss_attack();
            plane[i].x = rand() % SCREEN_W;
            plane[i].y = rand() % SCREEN_H;
            play_audio(voice_bomb, 1);
            return;
        }
    }
}
static void update(void) {
    if(!now_status)return;
    double now = al_get_time();
    if(now_status == 2){
        boss_update(now);
        if(now > skill_end){
            now_status = 1;
            skill_prepare = 0;
            stop_bgm(magic_id);
            for(int i = 0 ; i < skill_bullet_size ; i++)skill_bullets[i].hidden = true;
        }else if(now - last_skill_shot > 0){
            if(skill_prepare < skill_bullet_size){
                skill_bullets[skill_prepare].hidden = false;
                skill_bullets[skill_prepare].y = SCREEN_H - 50;
                skill_bullets[skill_prepare].x = (SCREEN_W / skill_bullet_size) * skill_prepare;
                skill_bullets[skill_prepare].vy = 0;
                skill_prepare++;
                return;
            }
            last_skill_shot = now;
            int now_select;
            do{
                now_select = rand() % skill_bullet_size;
            }while(skill_bullets[now_select].hidden == true && skill_bullets[now_select].cvy != 0);
            if(skill_bullets[now_select].hidden){
                skill_bullets[now_select].hidden = false;
                skill_bullets[now_select].y = SCREEN_H - 50;
                skill_bullets[now_select].x = (SCREEN_W / skill_bullet_size) * now_select;
                skill_bullets[now_select].vy = 0;
            }else if(skill_bullets[now_select].y == SCREEN_H - 50){
                skill_bullets[now_select].y = SCREEN_H -100;
            }else if(skill_bullets[now_select].vy == 0){
                skill_bullets[now_select].vy = -10;
            }
        }
        for(int i = 0 ; i < skill_bullet_size ; i++){
            if(!skill_bullets[i].hidden){
                skill_bullets[i].y += skill_bullets[i].vy;
                if(skill_bullets[i].y < 0)skill_bullets[i].hidden = true;
            }
        }
        LinkListMovableObject* now_enemy = enemies;
        while(now_enemy != NULL){
            if(change_state(&(now_enemy->val), now)){
                if(now_enemy->val.type == 4)enemy_4_size--;
                if(now_enemy == tail){
                    tail = now_enemy->last;
                }
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
        return;
    }
    if(!((int)now % 2)){
        if(rand() % 100 == 0)born(1);
        if(rand() % 800 == 0)born(2);
        if(rand() % 800 == 0)born(3);
        if(rand() % 300 == 0 && enemy_4_size < MAX_enemy_4_size){
            enemy_4_size++;
            born(4);
        }
    }
    update_move(now);
    check_state();
    boss_update(now);
}

static void draw_movable_object(MovableObject obj) {
    if (obj.hidden)
        return;
//    if(obj.type == 1 || obj.type == 2 || obj.type == 3)
//        al_draw_filled_rectangle(obj.x-obj.w/2,obj.y-obj.h/2,obj.x+obj.w/2,obj.y+obj.h/2,al_map_rgb(255, 255, 0));
//    else if(obj.type == 4)
//        al_draw_filled_rectangle(obj.x-obj.w/2,obj.y-obj.h/2,obj.x+obj.w/2,obj.y+obj.h/2,al_map_rgb(255, 0, 0));
//    else if(obj.type == 5)
//        al_draw_filled_rectangle(obj.x-obj.w/2,obj.y-obj.h/2,obj.x+obj.w/2,obj.y+obj.h/2,al_map_rgb(90, 0, 173));
//    else if(obj.type == -1)
//        al_draw_filled_rectangle(obj.x-obj.w/2,obj.y-obj.h/2,obj.x+obj.w/2,obj.y+obj.h/2,al_map_rgb(70, 117, 0));
//    else if(obj.type == 6)
//        al_draw_filled_circle(obj.x, obj.y, obj.w, al_map_rgb(255, 51, 153));
//    else if(obj.img == img_bullet)
//        al_draw_filled_circle(obj.x, obj.y, obj.w, al_map_rgb(79, 79, 79));
//    else
        al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
    if (draw_gizmos) {
        // al_draw_rectangle(round(obj.x - obj.w / 2), round(obj.y - obj.h / 2),
        //     round(obj.x + obj.w / 2) + 1, round(obj.y + obj.h / 2) + 1, al_map_rgb(255, 0, 0), 0);
        al_draw_circle(obj.x, obj.y, max(obj.w, obj.h)/2, al_map_rgb(255, 0, 0), 0);
    }
}
int rainbow = 0;
double last_change = 0;
static void draw(void) {
    if(!now_status){
        al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba(0,0,0,2));
        al_draw_text(font_pirulen_32, al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, "STOP");
        return;
    }
    double now = al_get_time();
    //draw background
    al_draw_bitmap(img_background, 0, 0, 0);
//    al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgb(255,255,255));
    if(now_status == 2){
        if(now - last_change > 0.05){
            rainbow++;rainbow%=7;
            last_change = now;
        }
        if(rainbow == 0)
            al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba(255,0,0,3));
        else if(rainbow == 1)
            al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba(255,165,0,3));
        else if(rainbow == 2)
            al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba(255,255,0,3));
        else if(rainbow == 3)
            al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba(0,127,255,3));
        else if(rainbow == 4)
            al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba(0,0,255,3));
        else if(rainbow == 5)
            al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba(0,0,0,3));
        else if(rainbow == 6)
            al_draw_filled_rectangle(0,0,SCREEN_W,SCREEN_H,al_map_rgba(139,0,255,3));
        for(int i = 0 ; i < skill_bullet_size ; i++){
            if(!skill_bullets[i].hidden)draw_movable_object(skill_bullets[i]);
        }
    }


    //draw score
    char score_char[64] = {"Score:"};
    char FPS_char[64] = {"FPS:"};
    to_string(score_char, score);
    to_string(FPS_char, FPS-redraws+1);
    al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), 130, 10, ALLEGRO_ALIGN_CENTER, score_char);
    al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), 115, 80, ALLEGRO_ALIGN_CENTER, FPS_char);
    //draw knife
    for(int i = 0 ; i < 2 ; i++){
        if(!knife_hidden[i]){
            al_draw_rotated_bitmap(img_knife, al_get_bitmap_width(img_knife)/2,al_get_bitmap_height(img_knife)/2, plane[i].x, plane[i].y,knife_ange[i],ALLEGRO_FLIP_VERTICAL);
        }
    }
    //draw boss
    if(!boss.hidden){
        draw_movable_object(boss);
        double length = 1500;
        al_draw_filled_rectangle(300, 0 ,max(300, 300 + (length*((double)boss.hp/boss.max_hp))), 50, al_map_rgb(153, 255, 0));
        al_draw_filled_rectangle(max(300 + (length*((double)boss.hp/boss.max_hp)), 300), 0, 300 + length, 50, al_map_rgb(255, 0, 0));
    }
    //draw plane
    for(int i = 0 ; i < 2 ; i++){
        if(plane[i].hp <= 0 || plane[i].hidden)continue;
        char output[50] = {};
//        to_string(output, plane[i].hp);
//        output[strlen(output)] = '/';
//        to_string(output, plane[i].max_hp);
//        al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), plane[i].x, plane[i].y - 65, ALLEGRO_ALIGN_CENTER, output);
        double length = 100;
        al_draw_filled_rectangle(plane[i].x - plane[i].w, plane[i].y + plane[i].h, plane[i].x - plane[i].w + (length*((double)plane[i].hp/plane[i].max_hp)), plane[i].y + plane[i].h + 10, al_map_rgb(153, 255, 0));
        al_draw_filled_rectangle(plane[i].x - plane[i].w + (length*((double)plane[i].hp/plane[i].max_hp)), plane[i].y + plane[i].h, plane[i].x - plane[i].w + length, plane[i].y + plane[i].h + 10, al_map_rgb(255, 0, 0));
        draw_movable_object(plane[i]);
    }
    //draw bullet
    for(int q = 0 ; q < 2 ; q++)
        for (int i = 0 ; i < MAX_BULLET ; i++)
            if(!bullets[q][i].hidden)
                draw_movable_object(bullets[q][i]);
    //draw enemy
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
    al_destroy_bitmap(img_enemy[2]);
    al_destroy_bitmap(img_bullet);
    game_log("Start scene destroyed");
}

static void on_key_down(int keycode) {
    if (keycode == ALLEGRO_KEY_TAB)
        draw_gizmos = !draw_gizmos;
    if(keycode == ALLEGRO_KEY_BACKSPACE)
        if(now_status == 0 || now_status == 1){
            if(now_status == 1)stop_bgm(bgm_id);
            else bgm_id = play_bgm((boss.hidden?bgm_start:bgm_boss), 1);
            now_status = !now_status;
        }
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
//    scene.initialize();
//    scene.init = false;
    start_scene = scene;
    game_log("Start scene created");
    return;
}
