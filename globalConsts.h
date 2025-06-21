#ifndef GLOBALCONSTS_H
#define GLOBALCONSTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>
#include "level.h"
#include "gameMode.h"
#include "menu.h"
#include "audio.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define NUM_PLAYERS 2

#define NO_FPS_LIMIT -1
#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

#define GRAV_CONST 500

#define JUMP_PAD_BOUNCE_CONST 500
#define SIDEWAYS_JUMP_PAD_BOUNCE_CONST 300
#define HAZARD_DAMAGE 20
#define HAZARD_KNOCKBACK 180

#define LADDER_MOVE_SPEED 100
#define LADDER_DROP_SPEED 200
#define LADDER_CLIMB_SPEED 150

#define MAX_WEAPONS_PER_INSTANCE 2
#define MAX_POWERS_PER_INSTANCE 1
// Weapon weapons[MAX_WEAPONS_PER_INSTANCE];
// Powers powers[MAX_POWERS_PER_INSTANCE];
#define BLACK_COLOR ((SDL_Color) {0, 0, 0, 255})
#define WHITE_COLOR ((SDL_Color) {255, 255, 255, 255})
#define BROWN_COLOR ((SDL_Color) {150, 75, 0, 255})
#define YELLOW_COLOR ((SDL_Color) {204, 204, 0, 255})
#define RED_COLOR ((SDL_Color) {255, 0, 0, 255})

#define OBJ_SPRITE_WIDTH 16
#define OBJ_SPRITE_HEIGHT 16
#define OBJ_SCALE 2
#define PASSABLE_PLAT_SCALE 3
#define LADDER_SCALE 3
#define HAZARD_SCALE 3


// #define MAX_WEAPON 3
// #define MAX_POWERS 2
// Weapon weapons[MAX_WEAPON];
// Powers powers[MAX_POWERS];

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern bool game_running;
extern int last_frame_time;
extern Level curLevel;
extern double delta_time;
extern GameSettings game_settings;
extern ScreenType cur_screen;
extern TTF_Font* defaultFont;
extern ScreenList screenList;
extern char* winMessage;
extern SoundBank sounds;
extern SDL_Texture* dungeonTileSheet;


#endif