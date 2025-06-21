#ifndef BAR_H
#define BAR_H

#include "player.h"

typedef struct Bar {
    int pointCap;
    int curPoints;
    Player player;
    SDL_Color barColor;
    int heightAbove;
    int borderWidth;
    int barHeight;
    double barAcceleration;
    double barVelocity;
    double pointsRemoved;
} *Bar;

extern Bar createHealthBar(Player player, SDL_Color color, int height_above);
extern void removePoints(Bar bar, int points);
#endif

