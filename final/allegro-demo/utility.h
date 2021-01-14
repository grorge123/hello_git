// [utility.h]
// you should put functions shared across files in this header.
// utility functions such as image processing, collision detection
// should be put here.

#ifndef SCENE_UTILITY_H
#define SCENE_UTILITY_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

/* Utility functions */

// Load audio and check if failed.
// Supports .wav, .flac, .ogg, .it, .mod, .s3m, .xm.
ALLEGRO_SAMPLE* load_audio(const char* filename);
// Play audio once.
ALLEGRO_SAMPLE_ID play_audio(ALLEGRO_SAMPLE* sample, float volume);
// Play audio loop.
ALLEGRO_SAMPLE_ID play_bgm(ALLEGRO_SAMPLE* sample, float volume);
// Stop audio.
void stop_bgm(ALLEGRO_SAMPLE_ID sample);
// Load font and check if failed.
// Supports .ttf.
ALLEGRO_FONT* load_font(const char* filename, int size);
// Load bitmap and check if failed.
// Supports .bmp, .pcx, .tga, .jpg, .png.
ALLEGRO_BITMAP* load_bitmap(const char* filename);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP* load_bitmap_resized(const char* filename, int w, int h);

bool pnt_in_rect(int px, int py, int x, int y, int w, int h);

// across different scenes. initialize the value in 'shared.c'.
extern ALLEGRO_FONT* font_pirulen_32;
extern ALLEGRO_FONT* font_pirulen_24;

// Initialize shared variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void shared_init(void);
// Free shared variables and resources.
void shared_destroy(void);
#endif