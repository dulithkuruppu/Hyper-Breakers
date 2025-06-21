#include "globalConsts.h"
#include "gameMode.h"

const char* character_names[] = {
    "HTILUD", 
    "AVIS", 
    //"LIHKIN", 
    "NAMQUN", 
    //"OTTO"
};
const char* mode_names[] = {"BEST_OF_3", "BEST_OF_5", "FIRST_TO_3", "FIRST_OF_5", "INFINITE"}; 
const char* map_names[] = {"SUN", "STAR", "MOON", "MARS", "BAM"};

// Description: initalizes players and map settings

GameSettings initGameSettings() {
    GameSettings game_settings = malloc(sizeof(struct GameSettings));
    // game_settings -> player1 = initPlayers("NAMQUN");
    // game_settings -> player2 = initPlayers("NAMQUN");
    int x = 0;
    int y = 0;

    GameObject valid_base_objects[10];
    int num_of_valid_bases = get_valid_bases(valid_base_objects);
    // printf("%d\n",num_of_valid_bases);
    game_settings -> player1 = initPlayers(character_names[rand() % NUMBER_OF_CHARACTERS]);
    try_spawn_on_surface(valid_base_objects[rand() % num_of_valid_bases], game_settings->player1->width, game_settings->player1->height, &x, &y, 1);
    game_settings -> player1 -> coord.x = x;
    game_settings -> player1 -> coord.y = y;
    printf("p1 %d %d\n", x, y);

    game_settings -> player2 = initPlayers(character_names[rand() % NUMBER_OF_CHARACTERS]); // they need to be different
    try_spawn_on_surface(valid_base_objects[rand() % num_of_valid_bases], game_settings->player2->width, game_settings->player2->height, &x, &y, 1);
    game_settings -> player2 -> coord.x = x;
    game_settings -> player2 -> coord.y = y;
    // printf("p2 %d %d\n", x, y);

    game_settings -> mode    = mode_names[rand() % NUMBER_OF_MODES];
    game_settings -> map     = map_names[rand() % NUMBER_OF_MAPS];
    // game_settings -> ready_to_start = false

    return game_settings;
}

void freeGameSettings() {
    if(game_settings == NULL) return;
    free(game_settings->player1);
    free(game_settings->player2);
    free(game_settings->player1->healthBar);
    free(game_settings->player2->healthBar);
    free(game_settings);
    game_settings = NULL;
}