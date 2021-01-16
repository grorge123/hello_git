// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

#ifndef SCENE_START_H
#define SCENE_START_H
#include "game.h"

typedef struct MovableObject MovableObject;
typedef struct LinkListMovableObject LinkListMovableObject;
typedef float(*change_v)(MovableObject now, double data[]);

struct MovableObject{
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the object¡¦s image.
    ALLEGRO_BITMAP* img;
    int hp;
    int max_hp;
    int type;
    int val;
    double data[10];
    change_v cvx;
    change_v cvy;
};


struct LinkListMovableObject{
    LinkListMovableObject *last, *next;
    MovableObject val;
};


extern bool player_double;
extern char plane_img[2][50];
// Return a Scene that has function pointers pointing to the
// functions that will react to the corresponding events.
void scene_start_create(void);
#endif
