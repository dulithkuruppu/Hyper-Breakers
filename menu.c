#include "menu.h"
#include "globalConsts.h"
#include <assert.h>
#include "gameMode.h"


void goToScreen(ScreenType screen) {
    cur_screen = screen;
} 

void goToGame() {
    goToScreen(GAME_SCREEN);
}

void goToStartScreen() {
    freeGameSettings();
    goToScreen(START_SCREEN);
}

void freeTextBox(void* tb) {
    TextBox *textBox = tb;
    free(textBox->text);
    free(textBox);
}

void onStart() {
    if(game_settings == NULL) {
        game_settings = initGameSettings();
    }
    goToGame();
}

MenuScreen createEmptyMenuScreen() {
    MenuScreen mScreen = malloc(sizeof (struct MenuScreen));
    assert(mScreen != NULL);
    mScreen->screenToDisplay = 0;
    mScreen->buttons = createDynList(NULL,NULL);
    mScreen->textBoxes = createDynList(freeTextBox,NULL);
    mScreen->visualBoxes = createDynList(NULL,NULL);
    return mScreen;
}


// Description: adds button to menu screen
void addButton(MenuScreen menuScreen, char* text, onClickFunct onClick, int x, int y, int width, int height, SDL_Color textColor, SDL_Color buttonColor, TTF_Font *font) {
    Button *b = malloc(sizeof(struct Button));
    assert(b != NULL);
    *b = (Button){.text = "", .onClick = onClick, .coord = {x, y}, .width = width,
              .height = height, .textColor = textColor, .buttonColor = buttonColor, .font = font
    };
    strncpy(b->text, text, sizeof(b->text) - 1);
    b->text[sizeof(b->text) - 1] = '\0'; 
    
    insertDynList(menuScreen->buttons,b);
}

void addGenericButton(MenuScreen menuScreen, char* text, onClickFunct onClick, int x, int y, int width, int height) {
    addButton(menuScreen,text,onClick,x,y,width,height,WHITE_COLOR,BLACK_COLOR,defaultFont);
}

void addTextBox(MenuScreen menuScreen, char* text, int x, int y, SDL_Color textColor, TTF_Font *font, bool isCentered) {
    TextBox *t = malloc(sizeof(TextBox));
    assert(t != NULL);
    *t = (TextBox){.text = strdup(text), .coord = {x, y}, .textColor = textColor, .font = font, .isCentered = isCentered};
    insertDynList(menuScreen->textBoxes,t);
}

void addVisualBox(MenuScreen menuScreen, int x, int y,int width, int height, SDL_Color color) {
    VisualBox *b = malloc(sizeof(VisualBox));
    assert(b != NULL);
    *b = (VisualBox){.coord = {x, y}, .width = width, .height = height, .color = color };
    insertDynList(menuScreen->visualBoxes,b);
}

void addGenericVisualBox(MenuScreen menuScreen, int x, int y,int width, int height) {
    addVisualBox(menuScreen, x, y, width, height, BLACK_COLOR);
}

void addGenericTextBox(MenuScreen menuScreen, char* text, int x, int y) {
    addTextBox(menuScreen,text,x,y,BLACK_COLOR,defaultFont,true);
}

MenuScreen createMainMenu() {
    MenuScreen mainMenu = createEmptyMenuScreen();
    addGenericButton(mainMenu,"START",onStart,WINDOW_WIDTH/2-50,WINDOW_HEIGHT/2-25,100,50);
    addGenericTextBox(mainMenu,"HYPER BREAKERS",WINDOW_WIDTH/2,50);
    mainMenu->screenToDisplay = START_SCREEN;
    return mainMenu;
}

MenuScreen createPauseMenu() {
    MenuScreen pauseMenu = createEmptyMenuScreen();
    addGenericButton(pauseMenu,"CONTINUE",goToGame,WINDOW_WIDTH/2-70,WINDOW_HEIGHT/2-40,140,50);
    addGenericButton(pauseMenu,"MAIN MENU",goToStartScreen,WINDOW_WIDTH/2-70,WINDOW_HEIGHT/2+40,140,50);
    addGenericTextBox(pauseMenu,"PAUSED",WINDOW_WIDTH/2,50);
    addVisualBox(pauseMenu,WINDOW_WIDTH/2-90,WINDOW_HEIGHT/2-60,180,180,WHITE_COLOR);
    pauseMenu->screenToDisplay = PAUSE_SCREEN;
    return pauseMenu;
}

MenuScreen createGameOverMenu() {
    MenuScreen gameOverMenu = createEmptyMenuScreen();
    addGenericButton(gameOverMenu,"MAIN MENU",goToStartScreen,WINDOW_WIDTH/2-70,WINDOW_HEIGHT/2-25,140,50);
    addGenericTextBox(gameOverMenu,"GAME OVER",WINDOW_WIDTH/2,50);
    gameOverMenu->screenToDisplay = GAME_OVER_SCREEN;
    return gameOverMenu;
}

void freeMenuScreen(void* menuScreen) {
    MenuScreen mscreen = menuScreen;
    freeDynList(mscreen->buttons);
    freeDynList(mscreen->textBoxes);
}

ScreenList createScreenList() {
    ScreenList sl = malloc(sizeof(struct ScreenList));
    assert(sl != NULL);

    sl->screens = createDynList(freeMenuScreen,NULL);

    insertDynList(sl->screens,createMainMenu());
    insertDynList(sl->screens,createPauseMenu());
    insertDynList(sl->screens,createGameOverMenu());

    return sl;
}

// Description: finds current screen in MenuScreen
MenuScreen findCurScreen(ScreenList screens) {
        for(int i = 0; i<getLengthDynList(screenList->screens); i++) {
            MenuScreen screen = getElem(screenList->screens,i);
            if(screen->screenToDisplay == cur_screen) {
                return screen;
            }
        }
        return NULL; //Screen not found
}