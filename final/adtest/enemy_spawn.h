//Enemy spawn
#ifndef ENEMY_SPAWN_H
#define ENEMY_SPAWN_H
#include "scene_start.h"

void enemy_spawn(MovableObject* a, int pat, int nums);

void enemy_detect(MovableObject* pb, MovableObject* e, int pbc, int ec);
void enemy_detect_bullet(MovableObject* pb, MovableObject* eb, int pbcount, int ebcount);
void enemy_detect_player(MovableObject* a, MovableObject* b, int Bcount);
void enemy_shoot_bullets(MovableObject* e, MovableObject* eb, int ec, int ebc);
#endif // ENEMY_SPAWN_H
