#include "bar.h"
#include "menu.h"
#include "globalConsts.h"
#define INIT_HEALTH 100
#define BAR_ACCEL_CONST 35

Bar createHealthBar(Player player,SDL_Color color, int height_above) {
    Bar healthBar = malloc(sizeof(struct Bar));
    healthBar->pointCap = INIT_HEALTH;
    healthBar->curPoints = healthBar->pointCap;
    healthBar->player = player;
    healthBar->barColor = color;
    healthBar->borderWidth = 2;
    healthBar->barHeight = 10;
    healthBar->heightAbove = height_above;
    healthBar->barAcceleration = BAR_ACCEL_CONST;
    healthBar->barVelocity = 0;
    healthBar->pointsRemoved = 0;
    return healthBar;
}

Uint32 timer_hit(Uint32 interval, void *player) {
    Player p = player;
    p->isHit = false;
    return 0;
}

Uint32 timer_game_over(Uint32 interval, void *param) {
    cur_screen = GAME_OVER_SCREEN;
    return 0;
}

void removePoints(Bar bar, int points) {
    bar->player->isHit = true;
    SDL_AddTimer(300, timer_hit, (void *) bar->player);
    if (bar->curPoints == 0) return;
    if (bar->curPoints - points > 0) {
        bar->curPoints -= points;
        bar->pointsRemoved += points;
    } else {
        bar->pointsRemoved += bar->curPoints;
        bar->curPoints = 0;
        SDL_AddTimer(5000, timer_game_over, NULL);
    }
}