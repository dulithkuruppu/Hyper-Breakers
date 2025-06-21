#include "gameLoop.h"
#include <stdbool.h>
#include <time.h>
#include "render.h"
#include "level.h"
#include "player.h"
#include <assert.h>
#include "globalConsts.h"
#include <stdio.h>
#include "gameMode.h"
#include "physics.h"
#include "menu.h"
#include "audio.h"
#include "sprites.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> 

SDL_Window* window;
SDL_Renderer* renderer;
bool game_running;
int last_frame_time = 0;
Level curLevel;
double delta_time = 0;
GameSettings game_settings = NULL;
ScreenType cur_screen = GAME_SCREEN; //= START_SCREEN;
TTF_Font* defaultFont;
ScreenList screenList;
SDL_Texture* dungeonTileSheet;

// Description: Creates window and renderer
bool initialize_window(void) {
    if(SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr,"Error Initializing SDL\n");
        return false;
    }

    //Initialize text renderer
    if (TTF_Init() == -1) {
        fprintf(stderr, "Error Initializing SDL_ttf\n");
        return false;
    }

    //Create window
    window = SDL_CreateWindow("Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0 //Can change to SDL_WINDOW_BORDERLESS for borderless
    );

    //Check if window was successfully created
    if(!window) {
        fprintf(stderr,"Error Creating SDL Window\n");
        return false;
    }

    //Create renderer
    renderer = SDL_CreateRenderer(window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    //Check if renderer was successfully created
    if(!renderer) {
        fprintf(stderr,"Error Creating SDL Renderer\n");
        return false;
    }

    return true;
}

void initializeFont() {
    defaultFont = TTF_OpenFont("assets/fonts/Lato-Regular.ttf", 24);
    assert(defaultFont != NULL);
}

// Description: Runs on on game start to initialize on start variables
void setup() {
    initializeFont();
    curLevel = createLevelOne();
    game_settings = initGameSettings();
    screenList = createScreenList();
    dungeonTileSheet = loadTexture("assets/sprites/objects/dungeonTileSet.png",renderer);
}

// Description: Extracts key code from event (assumes event is key press)
SDL_Keycode getEventKey(SDL_Event *event) {
    return event->key.keysym.sym;
}


// Description: Waits for frame rate to keep it consistent
void waitFrameRate() {
    if(FPS != NO_FPS_LIMIT) {
        int frame_wait_time = last_frame_time + FRAME_TARGET_TIME - SDL_GetTicks();
        if(frame_wait_time > 0 ) {
            SDL_Delay(frame_wait_time);
        }
    }
    int cur_frame_time = SDL_GetTicks();
    delta_time = (cur_frame_time - last_frame_time) / 1000.0f;
    last_frame_time = cur_frame_time;
}

// Description: process input when in game
void processGameScreenInput() {
    handle_input(game_settings->player1, game_settings->player2);
}


// Description: processes input to quit game
void processQuitInput(SDL_Event event) {
    switch(event.type) {
    case SDL_QUIT:
        game_running = false;
        break;
    case SDL_KEYDOWN:
        if(getEventKey(&event) == SDLK_ESCAPE) {
            game_running = false;
        }
        break;
    }
}


void processPauseInput(SDL_Event event) {
    if(event.type == SDL_KEYDOWN) {
        if(getEventKey(&event) == SDLK_p) {
            cur_screen = PAUSE_SCREEN;
        }
    }
}

void processPlayInput(SDL_Event event) {
    if(event.type == SDL_KEYDOWN) {
        if(getEventKey(&event) == SDLK_p) {
            cur_screen = GAME_SCREEN;
        }
    }
}


// Description: Process each button if clicked on the menu in the current screen
void processMenuInput(SDL_Event event) {
    if(event.type == SDL_MOUSEBUTTONDOWN) {
        if(event.button.button == SDL_BUTTON_LEFT) {
            MenuScreen screen = findCurScreen(screenList);

            if(screen == NULL) return; //No screen found

            for(int i = 0; i<getLengthDynList(screen->buttons); i++) {
                Button *button = getElem(screen->buttons,i);
                if(checkPointInRect(event.button.x, event.button.y, button->coord.x, button->coord.y,button->width,button->height)) {
                    (button->onClick)();
                }
            }
        } 
    }
}

// Description: Processes user inputs (Mostly keys pressed)
void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);
    processQuitInput(event);

    switch(cur_screen) {
        case GAME_SCREEN:
            processPauseInput(event);
            processGameScreenInput();
            break;
        case PAUSE_SCREEN:
            processPlayInput(event);
            break;
        default:
            break;
    }

    processMenuInput(event);
}

// Description: Physics logic on frame start for each object
void checkFrameStartLogic() {
    for(int i = 0; i< getLengthDynList(curLevel->objects); i++) {
        GameObject obj = getElem(curLevel->objects,i);
        if(obj->onFrameStart != NULL) {
            (obj->onFrameStart) (obj);
        }
    }
}

// Description: Sets flags used for physics data
void setMovementFlags(Player player) {
    double yVel = player->cur_vel.y;
    double xVel = player->cur_vel.x;
    setMovementFlagsFromVel(player,xVel,yVel);
}

void checkXCollisions(Player player) {
    for(int i = 0; i< getLengthDynList(curLevel->objects); i++) {
        GameObject obj = getElem(curLevel->objects,i);
        if(obj->colCheck != NULL  && obj->onColX != NULL && (obj->colCheck) (obj,player)) {
            (obj->onColX) (obj,player);
        }
    }
}


void updateX(Player player) {
    player->coord.x += player->cur_vel.x * delta_time;
    checkXCollisions(player);
}

void checkYCollisions(Player player) {
    for(int i = 0; i< getLengthDynList(curLevel->objects); i++) {
        GameObject obj = getElem(curLevel->objects,i);
        if(obj->colCheck != NULL  && obj->onColY != NULL && (obj->colCheck) (obj,player)) {
            (obj->onColY) (obj,player);
        }
    }
}

void applyGravity(Player player) {
    // if (player->coord.y > WINDOW_HEIGHT - player->height) {
    //     player->coord.y = WINDOW_HEIGHT - player->height - 150;
    //     player->cur_vel.y = 0;
    //     return;
    // }
    player->cur_vel.y += GRAV_CONST * delta_time;
    if(player->cur_vel.y > NO_POWER_MAX_Y_VEL) {
        player->cur_vel.y = NO_POWER_MAX_Y_VEL;
    }
}

void updateY(Player player) {
    player->coord.y += player->cur_vel.y * delta_time;
    checkYCollisions(player);
}

void checkFrameEndLogic() {
    for(int i = 0; i< getLengthDynList(curLevel->objects); i++) {
        GameObject obj = getElem(curLevel->objects,i);
        if(obj->onFrameEnd != NULL) {
            (obj->onFrameEnd) (obj);
        }
    }
}

void checkObjectsCollision() {
    for(int i = getLengthDynList(curLevel->objects) -1 ; i >= 0; i--) {
        GameObject obj = getElem(curLevel->objects,i);
        if(obj->GameObjectColCheck != NULL) {
            for(int j = getLengthDynList(curLevel->objects) -1; j >= 0; j--) {
                GameObject obj2 = getElem(curLevel->objects,j);
                if (obj != obj2 && obj->GameObjectColCheck(obj,obj2)) {
                    (obj->GameObjectOnColCheck) (obj, obj2);
                }
            }
        }
    }
}
// void checkObjectsCollision() {
//     for(int i = 0; i< getLengthDynList(curLevel->objects); i++) {
//         GameObject obj = getElem(curLevel->objects,i);
//         if(obj->GameObjectColCheck != NULL) {
//             for(int j = 0; j< getLengthDynList(curLevel->objects); j++) {
//                 GameObject obj2 = getElem(curLevel->objects,j);
//                 if (obj != obj2 && obj->GameObjectColCheck(obj,obj2)) {
//                     (obj->GameObjectOnColCheck) (obj, obj2);
//                 }
//             }
//         }
//     }
// }

void resetOnGround(Player player) {
    player->on_ground = false;
}

void playSounds() {
    static bool climbing_sound_playing = false;
    bool player_climbing = ((game_settings->player1->keyYTag != KEY_NONE_Y && game_settings->player1->onLadder) ||
                        (game_settings->player2->keyYTag != KEY_NONE_Y && game_settings->player2->onLadder));
    //printf("Climbing: %d %d %d %d\n", 
     //  game_settings->player1->is_moving, game_settings->player1->onLadder,
     //  game_settings->player2->is_moving, game_settings->player2->onLadder);
    int result;
    if (player_climbing && !climbing_sound_playing) {
        if (sounds.ladder_climbing_effect) {
            result = Mix_PlayChannel(-1, sounds.ladder_climbing_effect, -1);
            if (result != -1) {
                climbing_sound_playing = true;
            } else {
            }
        } else {
            printf("Climbing sound effect not loaded\n");
        }
        return;
    } else if (!player_climbing && climbing_sound_playing) {
        Mix_HaltChannel(-1);
        climbing_sound_playing = false;
    }
}

// Description: Handles updating the game state when in main game using delta_time to be consistent regardless of frame rate
void updateGameScreen() {

    playSounds();

    forEachPlayer(resetOnGround);

    forEachPlayer(applyGravity);

    forEachPlayer(setMovementFlags);

    checkFrameStartLogic();

    forEachPlayer(updateY);

    forEachPlayer(updateX);
    
    checkFrameEndLogic();

    checkObjectsCollision();
}

// Description: Handles updating the game state
void update() {

    
    static ScreenType last_screen = START_SCREEN;
        if (last_screen != cur_screen) {
            play_relevant_theme_sound(last_screen);
            last_screen = cur_screen;
        }
            
    //Intentional Feature: allowing one jump even if not on ground (uncomment for normal)
    switch(cur_screen) {
        case GAME_SCREEN:
            updateGameScreen();
            break;
        default:
            break;
    }

    maybe_respawn_feats();

    last_frame_time = SDL_GetTicks();
}


// Description: Safely destroys initialized renderer and window for end of program
void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_CloseFont(defaultFont);
    TTF_Quit();
}

void freeScreens() {
    if(screenList != NULL) {
        freeDynList(screenList->screens);
    }
}

// Description: terminates program, freeing everything
void exit_program() {
    destroy_window();
    if(curLevel != NULL) {
        freeLevel(curLevel);
    }
    freeScreens();
    freeGameSettings();
    free_sounds();
}

int main() {
    srand(time(NULL));
    game_running = initialize_window();

    init_audio();
    play_game_menu_song();
    setup();

    play_game_theme_song();
    while(game_running) {
        waitFrameRate();
        if (delta_time > 0.05) delta_time = 0.05;
        process_input();
        update();
        render();
    }
    exit_program();
    return 0;
}