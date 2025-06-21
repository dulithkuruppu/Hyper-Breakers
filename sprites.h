#ifndef SPRITES_H
#define SPRITES_H

#define WEAPON_SIZE 50
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "player.h"

#define WEAPON_SIZE 50

extern void drawNamqun(SDL_Renderer *r, Player player);
extern void drawHtilud(SDL_Renderer* renderer, Player player);
extern void drawAvis(SDL_Renderer* renderer, Player player);
//extern void drawLihkin(SDL_Renderer* renderer, Player player);
//extern void drawOtto(SDL_Renderer* renderer, Player player);
extern void drawShotgun(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle);
extern void drawPistol(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle);
extern void drawRifle(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle);
extern void drawSniper(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle);
extern void drawRPG(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle);
extern void changeLadderAnimation(void);
extern SDL_Texture* loadTexture(char* file, SDL_Renderer* renderer);
extern void renderPlatformImage(GameObject obj);
extern void renderPassablePlatformImage(GameObject obj);
extern void renderLadderImage(GameObject obj);
extern void renderHazardImage(GameObject obj);

#endif