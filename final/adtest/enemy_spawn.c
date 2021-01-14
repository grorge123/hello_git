#include "scene_start.h"
#include "enemy_spawn.h"
#include "game.h"

void enemy_spawn(MovableObject* a, int pat, int nums){
    int i;
    if(pat == 1){
        for(i = 0; i < nums; i++){
            if(i == 0){
                a[i].y = -a[i].h/2;
                a[i].x = (float)rand() / RAND_MAX * (SCREEN_W - a[i].w);
            }else{
                a[i].y = a[i-1].y - 80;
                a[i].x = a[i-1].x;
            }
            a[i].hidden = false;
            a[i].hp = 3;
        }
    }
    else if(pat == 2){
        for(i = 0; i < nums; i++){
            if(i == 0){
                a[i].x = -a[i].w/2;
                a[i].y = (rand() % (int)(SCREEN_H/2 - 2 * a[i].h)) + a[i].h;
            }else{
                a[i].x = a[i-1].x - 100;
                a[i].y = a[i-1].y;
            }
            a[i].hidden = false;
            a[i].hp = 3;
        }
    }
}

void enemy_detect(MovableObject* pb, MovableObject* e, int pbc, int ec){
    int i, j;
    for(i = 0; i < pbc; i++){
        for(j = 0; j < ec; j++){
            if(pb[i].hidden || e[j].hidden) continue;
            if(pb[i].x < e[j].x + e[j].w/3 && pb[i].x > e[j].x - e[j].w/3){
                if(pb[i].y < e[j].y + e[j].h/3 && pb[i].y > e[j].y - e[j].h/3){
                    e[j].hp--;
                    pb[i].hidden = true;
                    //game_log("Ouch");
                }
            }
        }
    }
}

void enemy_detect_bullet(MovableObject* pb, MovableObject* eb, int pbcount, int ebcount){
    int i, j;
    for(i = 0; i < ebcount; i++){
        for(j = 0; j < pbcount; j++){
            if(eb[i].hidden || pb[j].hidden) continue;
            if(pb[j].x < eb[i].x + eb[i].w/2 && pb[j].x > eb[i].x - eb[i].w/2){
                if(pb[j].y < eb[i].y + eb[i].h/2 && pb[j].y > eb[i].y - eb[i].h/2){
                    eb[i].hidden = true;
                    pb[j].hidden = true;
                    game_log("Ouch");
                }
            }
        }
    }
}

void enemy_detect_player(MovableObject* a, MovableObject* b, int Bcount){
    int i;
    for(i = 0; i < Bcount; i++){
        if(b[i].hidden) continue;
        if(a->x < b[i].x + b[i].w/2 && a->x > b[i].x - b[i].w/2){
            if(a->y < b[i].y + b[i].h/2 && a->y > b[i].y - b[i].h/2){
                a->hp--;
                b[i].hp = 0;
                game_log("Destroy E");
                //game_log("Ouch");
                }
        }
    }
}

void enemy_shoot_bullets(MovableObject* e, MovableObject* eb, int ec, int ebc){
    int i, j;
    for(i = 0; i < ec; i++){
        for(j = 0; j < ebc; j++){
            if(!e[i].hidden && eb[j].hidden && e[i].x > 0 && e[i].y > 0){
                eb[j].x = e[i].x;
                eb[j].y = e[i].y;
                eb[j].hidden = false;
                eb[j].hp = 1;
                break;
            }
        }
    }
}
