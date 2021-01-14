#ifndef GAME_H

#define GAME_H
#include <allegro5/allegro.h>
#define LOG_ENABLED

// Types of function pointers.
typedef void(*func_ptr)(void);
typedef void(*func_ptr_keyboard)(int keycode);
typedef void(*func_ptr_mouse)(int btn, int x, int y, int dz);
// TODO: More function pointer typedefs for other events.

// Structure containing all scene functions / event callbacks.
typedef struct {
	char* name;
	bool init;
	func_ptr initialize;
	func_ptr update;
	func_ptr draw;
	func_ptr destroy;
	func_ptr_keyboard on_key_down;
	func_ptr_keyboard on_key_up;
	func_ptr_mouse on_mouse_down;
	func_ptr_mouse on_mouse_move;
	func_ptr_mouse on_mouse_up;
	func_ptr_mouse on_mouse_scroll;
} Scene;

// Frame rate (frame per second)
extern const int FPS;
// Display (screen) width.
extern const int SCREEN_W;
// Display (screen) height.
extern const int SCREEN_H;
// At most 4 audios can be played at a time.
extern const int RESERVE_SAMPLES;


/* Input states */

// The active scene. Events will be triggered through function pointers.
extern Scene active_scene;
extern Scene menu_scene;
extern Scene start_scene;
extern Scene setting_scene;
extern Scene gameover_scene;
extern Scene gamewin_scene;

// Keyboard state, whether the key is down or not.
extern bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
extern bool* mouse_state;
// Mouse position.
extern int mouse_x, mouse_y;
// TODO: More variables to store input states such as joysticks, ...

/* Function prototypes */

// Create and start running the game.
void game_create(void);
// Function to change from one scene to another.
void game_change_scene(Scene* next_scene);

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);

#endif
