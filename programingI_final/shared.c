// [shared.c]
// you should define the shared variable declared in the header here.

#include "shared.h"
#include "utility.h"
#include "game.h"
#include "scene_menu.h"

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;

void shared_init(void) {
    font_pirulen_32 = load_font(".\\img\\pirulen.ttf", 32);
    font_pirulen_24 = load_font(".\\img\\pirulen.ttf", 24);
    bgm_menu = load_audio(".\\img\\S31-Night Prowler.ogg");
    bgm_start = load_audio(".\\img\\start.ogg");
}

void shared_destroy(void) {
    al_destroy_font(font_pirulen_32);
    al_destroy_font(font_pirulen_24);
}
