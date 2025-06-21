#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "level.h"

typedef struct Color {
    int r, g, b, a;
} Color;
extern void render();

extern void renderPlatform(GameObject p);
extern void renderMovablePlatform(GameObject p);
extern void renderBullet(GameObject p);
extern void renderGun(GameObject p);
extern void renderPower(GameObject p);
extern void renderJumpPad(GameObject p);
extern void renderPassablePlatform(GameObject p);
extern void renderHazard(GameObject p);
extern void renderLadder(GameObject p);

#endif