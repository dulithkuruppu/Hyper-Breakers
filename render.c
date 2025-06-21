#include "globalConsts.h"
#include "sprites.h"
#include "render.h"
#include "menu.h"
#include "bar.h"
#include "weapon.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <assert.h>

#define RAD_TO_DEG (180.0 / M_PI)

void renderRectangle(int x, int y, int width, int height, SDL_Color color) {
    SDL_Rect rect = {x,y,width,height};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer,&rect);
}

// Description: Renders a platform as a black rectangle
void renderPlatform(GameObject p) {
    renderRectangle(p->coord.x,p->coord.y,p->width,p->height,BLACK_COLOR);
}

// Description: Renders a platform as a black rectangle
void renderMovablePlatform(GameObject p) {
    SDL_Surface* tempSurface = IMG_Load("assets/sprites/objects/MovingPlatform.jpg");
    SDL_Rect rect = {p->coord.x,p->coord.y,p->width,p->height};
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(tempSurface);
    SDL_DestroyTexture(texture);
}

void renderBullet(GameObject b) {
    SDL_Rect rect = {b->coord.x,b->coord.y,b->width,b->height};
    SDL_Surface* tempSurface = IMG_Load("assets/sprites/bullets/5 Bullets/2.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    double angle = 0;
    if ((((BulletData*)b->extraData)->vel.x) == 0){
        angle = 90 * ((BulletData*)b->extraData)->vel.y > 0 ? 1 : -1;
    } else {
        angle = RAD_TO_DEG * atan(((BulletData*)b->extraData)->vel.y/((BulletData*)b->extraData)->vel.x);
    }
    SDL_RenderCopyEx(renderer, texture, NULL, &rect,angle,NULL,SDL_FLIP_NONE);
    SDL_FreeSurface(tempSurface);
    SDL_DestroyTexture(texture);
}

void renderJumpPad(GameObject p) {
    renderRectangle(p->coord.x,p->coord.y,p->width,p->height,YELLOW_COLOR);
}

void renderPassablePlatform(GameObject p) {
    renderRectangle(p->coord.x,p->coord.y,p->width,p->height,BROWN_COLOR);
}

void renderHazard(GameObject p) {
    renderRectangle(p->coord.x,p->coord.y,p->width,p->height,RED_COLOR);
}

void renderLadder(GameObject p) {
    renderRectangle(p->coord.x,p->coord.y,p->width,p->height,BROWN_COLOR);
}

void renderGun(GameObject p) {
    // printf("i printed\n");
    // renderRectangle(p->coord.x, p->coord.y, p->width, p->width, (Color) {0, 0, 0, 255});
    switch (((GunStats*)p->extraData)->type) {
        case PISTOL:
            drawPistol(renderer, p->coord.x, p->coord.y, WEAPON_SIZE, 1, 0);
            break;
        case RIFLE:
            drawRifle(renderer, p->coord.x, p->coord.y, WEAPON_SIZE, 1, 0);
            break;
        case SHOTGUN:
            drawShotgun(renderer, p->coord.x, p->coord.y, WEAPON_SIZE, 1, 0);
            break;
        case SNIPER:
            drawSniper(renderer, p->coord.x, p->coord.y,WEAPON_SIZE, 1, 0);
            break;
        default:
            break;
    }
}
    //     case RPG:
    //         renderRpg(p);
    //         break;
    // }


void renderPower(GameObject p) {
    // switch (p->extraData->power) {
    //     case RUN_FASTER:
    //         renderRunFaster(p);
    //         break;
    //     case INCREASE_HEALTH:
    //         renderIncreaseHealth(p);
    //         break;
    //     case JUMP_HIGHER:
    //         renderJumpHigh(p);
    //         break;
    // }
}

void renderAimingIndicator(Player player) {
    if (player->holding_shoot) {
        renderRectangle((player->aiming_direction.x - 4), 
        (player->aiming_direction.y - 4), 8, 8, (SDL_Color){255, 0, 0, 255});
    }
}

void renderBar(Bar bar) {
    
    int barWidth = bar->curPoints;
    int barHeight = bar->barHeight;
    int barX = bar->player->coord.x-bar->pointCap/2 + bar->player->width/2;
    int barY = bar->player->coord.y - bar->heightAbove - bar->barHeight;

    int borderWidth = bar->pointCap + bar->borderWidth * 2;
    int borderHeight = barHeight + bar->borderWidth * 2;
    int borderX = barX - bar->borderWidth;
    int borderY = barY - bar->borderWidth;

    int damageBarX = barX  + barWidth;
    int damageBarY = barY;
    int damageBarWidth = bar->pointsRemoved;
    int damageBarHeight = barHeight;

    renderRectangle(borderX,borderY,borderWidth,borderHeight,BLACK_COLOR);
    renderRectangle(barX,barY,barWidth,barHeight,bar->barColor);
    if(damageBarWidth > 0){
        renderRectangle(damageBarX,damageBarY,damageBarWidth,damageBarHeight,WHITE_COLOR);
        bar->pointsRemoved -= bar->barVelocity * delta_time;
        bar->barVelocity += bar->barAcceleration * delta_time;
    } else {
        bar->barVelocity = 0;
        bar->pointsRemoved = 0;
    }

}
    int pointCap;
    int curPoints;
    Player player;
    SDL_Color barColor;
    int heightAbove;
    double barAcceleration;
    double barVelocity;
    double pointsRemoved;

void renderAmmo(int currAmm, int totAmm, int x, int y){
    return;
}



// Description: Renders player as a rectangle
void renderPlayer(Player p) {
    if (strcmp(p->character, "NAMQUN") == 0) {
        drawNamqun(renderer, p);
    } else if (strcmp(p->character, "HTILUD") == 0) {
        drawHtilud(renderer, p);
    } else if (strcmp(p->character, "AVIS") == 0) {
        drawAvis(renderer, p);
    } //else if (strcmp(p->character, "LIHKIN") == 0) {
        //drawLihkin(renderer, p);
    //} else {
    //    drawOtto(renderer, p);
    //}
    renderBar(p->healthBar);
    renderAmmo(p->weapon->currentAmmo, p-> weapon->totalAmmo, p-> coord.x, p->coord.y);
    //drawShotgun(renderer, 50, 50, 50, true);
    //drawTexture(renderer, texture, p->coord.x, p->coord.y, p->width, p->height)
    //renderRectangle(p->coord.x,p->coord.y,p->width,p->height,(Color) {80, 0, 0, 255});
}


// Description: Renders all platforms in a level
static void renderLevel(Level l) {
    for(int i = 0; i<getLengthDynList(l->objects); i++) {
        GameObject g = getElem(l->objects, i);
        (g->onRender) (g);
    }
}

void renderGameScreen() {
        //Render level
        renderLevel(curLevel);

        //Render players
        renderPlayer(game_settings->player1);
        renderPlayer(game_settings->player2);

        renderAimingIndicator(game_settings->player1);
        renderAimingIndicator(game_settings->player2);

    
}

// Description: renders text on screen
void renderText(int x, int y, SDL_Color color, TTF_Font* font, char* text) {
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, text, color);
    assert(textSurface != NULL);

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_FreeSurface(textSurface); 
    SDL_Rect dstRect = (SDL_Rect) {x,y,0,0};
    SDL_QueryTexture(textTexture, NULL, NULL, &dstRect.w, &dstRect.h);
    SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
    SDL_DestroyTexture(textTexture);
}
// Description: renders text and centers it on a rectangle
void renderCenteredText(int x, int y, int width, int height, SDL_Color color, TTF_Font* font, char* text) {
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, text, color);
    assert(textSurface != NULL);

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_FreeSurface(textSurface); 
    SDL_Rect dstRect = (SDL_Rect) {0,0,0,0};
    SDL_QueryTexture(textTexture, NULL, NULL, &dstRect.w, &dstRect.h);
    dstRect.x = x + (width-dstRect.w)/2;
    dstRect.y = y + (height-dstRect.h)/2;
    SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
    SDL_DestroyTexture(textTexture);
}

void renderButton(Button button) {
    renderRectangle(button.coord.x,button.coord.y,button.width,button.height,button.buttonColor);

    renderCenteredText(button.coord.x,button.coord.y,button.width,button.height,button.textColor,button.font,button.text);
    
}

void renderTextBox(TextBox textBox) {
    if(textBox.isCentered) {
        renderCenteredText(textBox.coord.x,textBox.coord.y,0,0,textBox.textColor,textBox.font,textBox.text);
    } else {
        renderText(textBox.coord.x,textBox.coord.y,textBox.textColor,textBox.font,textBox.text);
    }
}

void renderVisualBox(VisualBox visualBox) {
    renderRectangle(visualBox.coord.x,visualBox.coord.y,visualBox.width,visualBox.height,visualBox.color);
}

// Description: renders the menu by rendering the buttons, text and boxes
void renderMenuScreen(MenuScreen screen) {
    if(screen == NULL) return;

    for(int i = 0; i<getLengthDynList(screen->visualBoxes); i++) {
        VisualBox *visualBoxPtr = getElem(screen->visualBoxes,i);
        renderVisualBox(*visualBoxPtr);
    }

    for(int i = 0; i<getLengthDynList(screen->buttons); i++) {
        Button *buttonPtr = getElem(screen->buttons,i);
        renderButton(*buttonPtr);
    }
    for(int i = 0; i<getLengthDynList(screen->textBoxes); i++) {
        TextBox *textBoxPtr = getElem(screen->textBoxes,i);
        renderTextBox(*textBoxPtr);
    }

}


void renderMenuScreenList() {
    renderMenuScreen(findCurScreen(screenList));
}

// Description: Renders all images
void render() {
    //Clear render
    SDL_SetRenderDrawColor(renderer, 255, 122, 255, 255);
    SDL_RenderClear(renderer);

    switch(cur_screen) {
        case(GAME_SCREEN): {
            SDL_Surface* tempSurface = IMG_Load("assets/BackGround/fortnite.png");
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_FreeSurface(tempSurface);
            SDL_DestroyTexture(texture);
            renderGameScreen();
            break;
        }
        default:
            renderMenuScreenList();
            break;
    }

    //Refresh render
    SDL_RenderPresent(renderer);
}
