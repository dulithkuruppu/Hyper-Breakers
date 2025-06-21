#ifndef GAMEMODE_H
#define GAMEMODE_H

#include "player.h"

#define NUMBER_OF_CHARACTERS 3 //5
#define NUMBER_OF_MODES 5
#define NUMBER_OF_MAPS 5

struct GameSettings{
    Player player1;
    Player player2;
    const char* mode;
    const char* map;
    // bool ready_to_start;
};

typedef struct GameSettings *GameSettings;
extern GameSettings initGameSettings();
extern void freeGameSettings();
#endif