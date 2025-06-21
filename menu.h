#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> 
#include "vector.h" 
#include "genericDynamicList.h"
#include <stdbool.h>

typedef enum ScreenType {
    START_SCREEN, GAME_SCREEN, PAUSE_SCREEN, GAME_OVER_SCREEN
} ScreenType;

typedef void (*onClickFunct) (void);

typedef struct Button {
    char text[15];
    onClickFunct onClick;
    Vector coord;
    int width, height;
    SDL_Color textColor;
    SDL_Color buttonColor;
    TTF_Font *font;
} Button;

typedef struct TextBox {
    char* text;
    bool isCentered;
    Vector coord;
    SDL_Color textColor;
    TTF_Font *font;
} TextBox;

typedef struct VisualBox {
    Vector coord;
    int width, height;
    SDL_Color color;
} VisualBox;

typedef struct MenuScreen {
    ScreenType screenToDisplay;
    dynList buttons;
    dynList textBoxes;
    dynList visualBoxes;
} *MenuScreen;

typedef struct ScreenList {
    dynList screens;
} *ScreenList;

extern ScreenList createScreenList(void);

extern MenuScreen findCurScreen(ScreenList screens);
#endif