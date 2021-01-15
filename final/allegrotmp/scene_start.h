// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

#ifndef SCENE_START_H
#define SCENE_START_H
#include "game.h"

extern bool player_double;
extern char plane_img[2][50];
// Return a Scene that has function pointers pointing to the
// functions that will react to the corresponding events.
void scene_start_create(void);
#endif