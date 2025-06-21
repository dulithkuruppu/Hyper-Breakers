#include "sprites.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <math.h>
#include "weapon.h"
#include "player.h"
#include "globalConsts.h"
#define RIGHT 1
#define LEFT 0
#define FRAME_WIDTH_RUN 25
#define FRAME_HEIGHT_RUN 34
#define FRAME_HEIGHT_CLIMB 41
#define ANIMATION_SPEED 0.1
#define JUMPING_ANIMATION_SPEED 3
#define IDLE_ANIMATION_SPEED 2
#define LADDER_ANIMATION_SPEED 20

double ladderState = 0;

extern void renderPlatform(GameObject p);
static void drawGun(SDL_Renderer*renderer, GunClass weaponName, int x, int y, int bodyX, int headHeight, int bodyHeight, int bodyWidth, int direction, int angle);
static void drawPlayer(char* path, SDL_Renderer* renderer, int state, Player player, int startY, int frame_height, int frame_width);
static void drawJumpingPlayer(char* path, SDL_Renderer* renderer, int state, Player player);

void drawJumpingPlayer(char* path, SDL_Renderer* renderer, int state, Player player) {
    int startY = -10;
    const char* playerName = player->character;
    int frameHeight = FRAME_HEIGHT_RUN;
    int frameWidth = FRAME_WIDTH_RUN;
    switch (state) {
        case 0:
            if (strcmp(playerName, "HTILUD") == 0) {
                startY = 13;
            } else if (strcmp(playerName, "NAMQUN") == 0) {
                startY = 15;
                frameWidth = 27;
                frameHeight = 36;
            } else if (strcmp(playerName, "AVIS") == 0) {
                startY = 13;
            }
            break;
        case 1:
            if (strcmp(playerName, "HTILUD") == 0) {
                startY = 9;
            } else if (strcmp(playerName, "NAMQUN") == 0) {
                startY = 10;
                frameWidth = 27;
                frameHeight = 36;
            } else if (strcmp(playerName, "AVIS") == 0) {
                startY = 10;
            }
            break;
        case 2:
            if (strcmp(playerName, "HTILUD") == 0) {
                startY = 8;
            } else if (strcmp(playerName, "NAMQUN") == 0) {
                startY = 14;
                frameWidth = 27;
                frameHeight = 36;
            } else if (strcmp(playerName, "AVIS") == 0) {
                startY = 13;
            }
            break;
        case 3:
            if (strcmp(playerName, "HTILUD") == 0) {
                startY = 12;
            } else if (strcmp(playerName, "NAMQUN") == 0) {
                startY = 17;
                frameWidth = 27;
                frameHeight = 36;
            } else if (strcmp(playerName, "AVIS") == 0) {
                startY = 15;
            }
            break;
        default:
            break;
    }
    drawPlayer(path, renderer, state, player, startY, frameHeight, frameWidth);
}

static void drawPlayer(char* path, SDL_Renderer* renderer, int state, Player player, int startY, int frame_height, int frame_width) {
    SDL_Surface* tempSurface = IMG_Load(path);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    int offSet = 23;
    SDL_Rect srcRect = {2 + FRAME_WIDTH_RUN * state + offSet * state, startY, frame_width, frame_height};
    SDL_Rect destRect = {player->coord.x, player->coord.y, player->width, player->height};
    if (player->facing_direction == RIGHT)
        SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
    else
        SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, NULL, SDL_FLIP_HORIZONTAL);
    SDL_DestroyTexture(texture);
}

void changeLadderAnimation(void) {
    ladderState += delta_time * LADDER_ANIMATION_SPEED;
}

static void drawArms(char* path, Player player) {
    static double state_arms = 0;
    int angleStage;
    if (player-> shooting_angle < -75){
        angleStage = 1;
    } else if (player-> shooting_angle < -45) {
        angleStage = 2;
    } else if (player-> shooting_angle < 45) {
        angleStage = 3;
    } else if (player-> shooting_angle < 75) {
        angleStage = 4;
    } else {
        angleStage = 5;
    }
    char filePath[100];
    strcpy(filePath, path);
    SDL_Surface* tempSurface = IMG_Load(filePath);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    angleStage +=5;
    angleStage = angleStage % 10;
    filePath[strlen(filePath) - 5] = angleStage + '0';
    tempSurface = IMG_Load(filePath);
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    state_arms += delta_time;
    int xOffset = 8 + (int) round(state_arms) % 4 ;
    if (player->is_moving && player->on_ground) {
        xOffset += 20 + 0.5 * ((int) round(state_arms) % 5);
    } else if (!player->on_ground) {
        xOffset += 15;
    }
    if (player->facing_direction == RIGHT) {
            SDL_Rect destRect = {xOffset + player->coord.x, -5 + player->coord.y, player->width, player->height};
            SDL_RenderCopy(renderer, texture, NULL, &destRect);
    } else {
            SDL_Rect destRect = { - xOffset + player->coord.x,  -5 + player->coord.y, player->width, player->height};
            SDL_RenderCopyEx(renderer, texture, NULL, &destRect, 0, NULL, SDL_FLIP_HORIZONTAL);
    }
}

void drawNamqun(SDL_Renderer* renderer, Player player) {
    
    bool isDead = player->healthBar->curPoints <= 0;
    if (isDead) {
        static double state = 0;
        state += delta_time * 4;
        drawPlayer("assets/sprites/characters/char1/Biker_death.png", renderer, fmin(((int) round(state)), 5), player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (player->isHit) {
        drawPlayer("assets/sprites/characters/char1/Biker_hurt.png", renderer, 1, player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (!player->is_moving && player->on_ground) {                                                                                                      // Idle
        static double state = 0;
        state += delta_time * IDLE_ANIMATION_SPEED;
        drawPlayer("assets/sprites/characters/char1/Idle2.png", renderer, ((int) round(state)) % 4, player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (player->is_moving && player->on_ground) {                                                                                                // Moving on ground
        static double state = 0;
        state += delta_time * ANIMATION_SPEED * fmax(fabs(player->cur_vel.x), 0.5);
        drawPlayer("assets/sprites/characters/char1/Run2.png", renderer, ((int) round(state)) % 6, player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (player->onLadder) {                                                                                                                      // On Ladder
        drawPlayer("assets/sprites/characters/char1/Biker_climb.png", renderer, ((int) round(ladderState)) % 6, player, 7, FRAME_HEIGHT_CLIMB, FRAME_WIDTH_RUN);
    } else if (!player->on_ground) {                                                                                                                    // Jumping
        static double state = 0;
        state += delta_time * JUMPING_ANIMATION_SPEED;
        drawJumpingPlayer("assets/sprites/characters/char1/Jump2.png", renderer, ((int) round(state)) % 4, player);
    }

    if (!player->onLadder && !isDead && !player->isHit) {
        drawArms("assets/sprites/characters/hands/1 Biker/X.png", player);
        drawGun(renderer, player->weapon->gun, player->coord.x, player->coord.y, player->coord.x + (player->width - player->width * 0.7f)/2, player->height * 0.3f, player->height * 0.5f, player->width * 0.7f, player->facing_direction, player->shooting_angle);
    }
}

void drawHtilud(SDL_Renderer* renderer, Player player) {
    
    bool isDead = player->healthBar->curPoints <= 0;
    if (isDead) {
        static double state = 0;
        state += delta_time * 4;
        drawPlayer("assets/sprites/characters/char2/Punk_death.png", renderer, fmin(((int) round(state)), 5), player, 14, FRAME_HEIGHT_RUN, 30);
    } else if (player->isHit) {
        drawPlayer("assets/sprites/characters/char2/Punk_hurt.png", renderer, 1, player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (!player->is_moving && player->on_ground) {                                                                                                      // Idle
        static double state = 0;
        state += delta_time * IDLE_ANIMATION_SPEED;
        drawPlayer("assets/sprites/characters/char2/Idle2.png", renderer, ((int) round(state)) % 4, player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (player->is_moving && player->on_ground) {                                                                                                // Moving on ground
        static double state = 0;
        state += delta_time * ANIMATION_SPEED * fmax(fabs(player->cur_vel.x), 0.5);
        drawPlayer("assets/sprites/characters/char2/Run2.png", renderer, ((int) round(state)) % 6, player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (player->onLadder) {                                                                                                                      // On Ladder
        drawPlayer("assets/sprites/characters/char2/Punk_climb.png", renderer, ((int) round(ladderState)) % 6, player, 12, 35, FRAME_WIDTH_RUN);
    } else if (!player->on_ground) {                                                                                                                    // Jumping
        static double state = 0;
        state += delta_time * JUMPING_ANIMATION_SPEED;
        drawJumpingPlayer("assets/sprites/characters/char2/Jump2.png", renderer, ((int) round(state)) % 4, player);
    }
    
    if (!player->onLadder && !isDead && !player->isHit) {
        drawArms("assets/sprites/characters/hands/2 Punk/X.png", player);
        drawGun(renderer, player->weapon->gun, player->coord.x, player->coord.y, player->coord.x + (player->width - player->width * 0.7f)/2, player->height * 0.3f, player->height * 0.5f, player->width * 0.7f, player->facing_direction, player->shooting_angle);
    }
}

void drawAvis(SDL_Renderer* renderer, Player player) {

    bool isDead = player->healthBar->curPoints <= 0;
    if (isDead) {
        static double state = 0;
        state += delta_time * 4;
        drawPlayer("assets/sprites/characters/char3/Cyborg_death.png", renderer, fmin(((int) round(state)), 5), player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (player->isHit) {
        drawPlayer("assets/sprites/characters/char3/Cyborg_hurt.png", renderer, 1, player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (!player->is_moving && player->on_ground) {                                                                                                      // Idle
        static double state = 0;
        state += delta_time * IDLE_ANIMATION_SPEED;
        drawPlayer("assets/sprites/characters/char3/Idle2.png", renderer, ((int) round(state)) % 4, player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (player->is_moving && player->on_ground) {                                                                                                // Moving on ground
        static double state = 0;
        state += delta_time * ANIMATION_SPEED * fmax(fabs(player->cur_vel.x), 0.5);
        drawPlayer("assets/sprites/characters/char3/Run2.png", renderer, ((int) round(state)) % 6, player, 14, FRAME_HEIGHT_RUN, FRAME_WIDTH_RUN);
    } else if (player->onLadder) {                                                                                                                      // On Ladder
        drawPlayer("assets/sprites/characters/char3/Cyborg_climb.png", renderer, ((int) round(ladderState)) % 6, player, 4, 42, FRAME_WIDTH_RUN);
    } else if (!player->on_ground) {                                                                                                                    // Jumping
        static double state = 0;
        state += delta_time * JUMPING_ANIMATION_SPEED;
        drawJumpingPlayer("assets/sprites/characters/char3/Jump2.png", renderer, ((int) round(state)) % 4, player);
    }
    
    if (!player->onLadder && !isDead && !player->isHit) {
        drawArms("assets/sprites/characters/hands/3 Cyborg/X.png", player);
        drawGun(renderer, player->weapon->gun, player->coord.x, player->coord.y, player->coord.x + (player->width - player->width * 0.7f)/2, player->height * 0.3f, player->height * 0.5f, player->width * 0.7f, player->facing_direction, player->shooting_angle);
    }
}

// void drawLihkin(SDL_Renderer* renderer, Player player) {
//     int x = player->coord.x;
//     int y = player->coord.y;
//     int width = player->width;
//     int height = player->height;
//     bool direction = player->facing_direction;
//     GunClass weaponName = player->weapon->gun;
    
//     // Save the current draw color
//     SDL_Color original;
//     SDL_GetRenderDrawColor(renderer, &original.r, &original.g, &original.b, &original.a);
    
//     // Same proportions as original
//     int headHeight = height * 0.3f;
//     int bodyHeight = height * 0.5f;
//     int legHeight = height * 0.2f;
//     int legWidth = width * 0.35f;
//     int bodyWidth = width * 0.7f;
//     int bodyX = x + (width - bodyWidth)/2;
//     int armWidth = width * 0.18f;

//     // Draw legs (trench coat tails)
//     SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);  // Dark charcoal
//     SDL_Rect leftLeg = {x, y + headHeight + bodyHeight, legWidth, legHeight};
//     SDL_Rect rightLeg = {x + width - legWidth, y + headHeight + bodyHeight, legWidth, legHeight};
//     SDL_RenderFillRect(renderer, &leftLeg);
//     SDL_RenderFillRect(renderer, &rightLeg);
    
//     // Draw shoes
//     SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
//     SDL_Rect leftShoe = {x, y + headHeight + bodyHeight + legHeight*0.7f, legWidth, legHeight*0.3f};
//     SDL_Rect rightShoe = {x + width - legWidth, y + headHeight + bodyHeight + legHeight*0.7f, legWidth, legHeight*0.3f};
//     SDL_RenderFillRect(renderer, &leftShoe);
//     SDL_RenderFillRect(renderer, &rightShoe);
    
//     // Draw body (trench coat)
//     SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
//     SDL_Rect body = {bodyX, y + headHeight, bodyWidth, bodyHeight};
//     SDL_RenderFillRect(renderer, &body);
    
//     // Draw inner suit (vest)
//     SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
//     SDL_Rect vest = {bodyX + (bodyWidth - bodyWidth*0.4f)/2, y + headHeight, bodyWidth*0.4f, bodyHeight*0.4f};
//     SDL_RenderFillRect(renderer, &vest);
    
//     // Draw tie
//     SDL_SetRenderDrawColor(renderer, 150, 0, 0, 255);  // Dark red
//     SDL_Rect tie = {bodyX + (bodyWidth - bodyWidth*0.15f)/2, y + headHeight, bodyWidth*0.15f, bodyHeight*0.3f};
//     SDL_RenderFillRect(renderer, &tie);
    
//     // Draw head (fedora shadow covers top)
//     SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
//     SDL_Rect hat = {bodyX + (bodyWidth - bodyWidth*1.0f)/2, y, bodyWidth*1.0f, headHeight*0.3f};
//     SDL_RenderFillRect(renderer, &hat);
    
//     // Draw face (pale with shadow)
//     SDL_SetRenderDrawColor(renderer, 220, 190, 160, 255);
//     SDL_Rect face = {bodyX + (bodyWidth - bodyWidth*1.0f)/2, y + headHeight*0.1f, bodyWidth*1.0f, headHeight*0.9f};
//     SDL_RenderFillRect(renderer, &face);

//     drawGun(renderer, weaponName, x, y, bodyX, headHeight, bodyHeight, bodyWidth, direction);
    
//     if (direction == RIGHT) {
//         // Right arm (holding gun) - same position
//         SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
//         SDL_Rect rightArm = {bodyX + bodyWidth, y + headHeight + bodyHeight*0.2f, armWidth, bodyHeight*0.5f};
//         SDL_RenderFillRect(renderer, &rightArm);
        
//         // Left arm (holding notepad)
//         SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
//         SDL_Rect leftArm = {bodyX - armWidth, y + headHeight + bodyHeight*0.2f, armWidth, bodyHeight*0.5f};
//         SDL_RenderFillRect(renderer, &leftArm);

//         // Lower left arm
//         SDL_SetRenderDrawColor(renderer, 220, 190, 160, 255);
//         SDL_Rect lowerLeftArm = {bodyX - armWidth, y + headHeight + bodyHeight*0.57, armWidth*2, armWidth};
//         SDL_RenderFillRect(renderer, &lowerLeftArm);
        
//         // Sunglasses facing right
//         SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
//         SDL_Rect glassesFrame = {bodyX + (bodyWidth - bodyWidth*0.1f)/2, y + headHeight*0.3f, bodyWidth*0.7f, headHeight*0.4f};
//         SDL_RenderFillRect(renderer, &glassesFrame);

//     } else {
//         // Left arm (holding gun) - same position
//         SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
//         SDL_Rect leftArm = {bodyX - armWidth, y + headHeight + bodyHeight*0.2f, armWidth, bodyHeight*0.5f};
//         SDL_RenderFillRect(renderer, &leftArm);
        
//         // Right arm
//         SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
//         SDL_Rect rightArm = {bodyX + bodyWidth, y + headHeight + bodyHeight*0.2f, armWidth, bodyHeight*0.5f};
//         SDL_RenderFillRect(renderer, &rightArm);

//         // Lower right arm
//         SDL_SetRenderDrawColor(renderer, 220, 190, 160, 255);
//         SDL_Rect lowerRightArm = {bodyX + bodyWidth*0.75f, y + headHeight + bodyHeight*0.57, armWidth*2, armWidth};
//         SDL_RenderFillRect(renderer, &lowerRightArm);

//         // Sunglasses facing left
//         SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
//         SDL_Rect glassesFrame = {bodyX + (bodyWidth - bodyWidth*1.1f)/2, y + headHeight*0.3f, bodyWidth*0.7f, headHeight*0.4f};
//         SDL_RenderFillRect(renderer, &glassesFrame);
//     }
    
//     // Add coat collar details
//     SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
//     SDL_Rect leftCollar = {bodyX - armWidth/2, y + headHeight, armWidth*1.5f, armWidth/2};
//     SDL_Rect rightCollar = {bodyX + bodyWidth - armWidth, y + headHeight, armWidth*1.5f, armWidth/2};
//     SDL_RenderFillRect(renderer, &leftCollar);
//     SDL_RenderFillRect(renderer, &rightCollar);
    
//     // Restore original draw color
//     SDL_SetRenderDrawColor(renderer, original.r, original.g, original.b, original.a);
// }

// void drawOtto(SDL_Renderer* renderer, Player player) {
//     int x = player->coord.x;
//     int y = player->coord.y;
//     int width = player->width;
//     int height = player->height;
//     bool direction = player->facing_direction;
//     GunClass weaponName = player->weapon->gun;
    
//     // Save the current draw color
//     SDL_Color original;
//     SDL_GetRenderDrawColor(renderer, &original.r, &original.g, &original.b, &original.a);
    
//     // Same proportions as original
//     int headHeight = height * 0.3f;
//     int bodyHeight = height * 0.5f;
//     int legHeight = height * 0.2f;
//     int legWidth = width * 0.35f;
//     int bodyWidth = width * 0.7f;
//     int bodyX = x + (width - bodyWidth)/2;
//     int armWidth = width * 0.18f;

//     // Draw legs (torn lab coat)
//     SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);  // Dirty white
//     SDL_Rect leftLeg = {x, y + headHeight + bodyHeight, legWidth, legHeight};
//     SDL_Rect rightLeg = {x + width - legWidth, y + headHeight + bodyHeight, legWidth, legHeight};
//     SDL_RenderFillRect(renderer, &leftLeg);
//     SDL_RenderFillRect(renderer, &rightLeg);
    
//     // Draw shoes (old leather)
//     SDL_SetRenderDrawColor(renderer, 80, 50, 20, 255);
//     SDL_Rect leftShoe = {x, y + headHeight + bodyHeight + legHeight*0.7f, legWidth, legHeight*0.3f};
//     SDL_Rect rightShoe = {x + width - legWidth, y + headHeight + bodyHeight + legHeight*0.7f, legWidth, legHeight*0.3f};
//     SDL_RenderFillRect(renderer, &leftShoe);
//     SDL_RenderFillRect(renderer, &rightShoe);
    
//     // Draw body (lab coat)
//     SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
//     SDL_Rect body = {bodyX, y + headHeight, bodyWidth, bodyHeight};
//     SDL_RenderFillRect(renderer, &body);
    
//     // Draw vest (with chemical stains)
//     SDL_SetRenderDrawColor(renderer, 150, 0, 150, 255);  // Purple stain
//     SDL_Rect vest = {bodyX + (bodyWidth - bodyWidth*0.4f)/2, y + headHeight, bodyWidth*0.4f, bodyHeight*0.4f};
//     SDL_RenderFillRect(renderer, &vest);
    
//     // Draw head (wild hair)
//     SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);  // Gray hair
//     SDL_Rect hair = {bodyX + (bodyWidth - bodyWidth*1.2f)/2, y, bodyWidth*1.2f, headHeight*0.5f};
//     SDL_RenderFillRect(renderer, &hair);
    
//     // Draw face
//     SDL_SetRenderDrawColor(renderer, 220, 190, 160, 255);
//     SDL_Rect face = {bodyX + (bodyWidth - bodyWidth*1.0f)/2, y + headHeight*0.2f, bodyWidth*1.0f, headHeight*0.8f};
//     SDL_RenderFillRect(renderer, &face);
    
//     drawGun(renderer, weaponName, x, y, bodyX, headHeight, bodyHeight, bodyWidth, direction);

//     if (direction == RIGHT) {
//         // Right arm (holding gun) - same position
//         SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);  // Lab coat sleeve
//         SDL_Rect rightArm = {bodyX + bodyWidth, y + headHeight + bodyHeight*0.2f, armWidth, bodyHeight*0.5f};
//         SDL_RenderFillRect(renderer, &rightArm);
        
//         // Left arm (holding flask)
//         SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
//         SDL_Rect leftArm = {bodyX - armWidth, y + headHeight + bodyHeight*0.2f, armWidth, bodyHeight*0.5f};
//         SDL_RenderFillRect(renderer, &leftArm);

//         // Flask with glowing liquid
//         SDL_SetRenderDrawColor(renderer, 0, 200, 0, 150);  // Glowing green
//         SDL_Rect flask = {bodyX - armWidth, y + headHeight + bodyHeight*0.57, armWidth*2, armWidth};
//         SDL_RenderFillRect(renderer, &flask);

//         // Draw goggles looking right
//         SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // Metal frame
//         SDL_Rect gogglesFrame = {bodyX + bodyWidth * 0.4f, y + headHeight*0.4f, bodyWidth*0.7f, headHeight*0.3f};
//         SDL_RenderFillRect(renderer, &gogglesFrame);
        
//         SDL_SetRenderDrawColor(renderer, 30, 144, 255, 150);  // Blue lenses
//         SDL_Rect lenses = {bodyX + bodyWidth * 0.5f, y + headHeight*0.45f, bodyWidth*0.6f, headHeight*0.2f};
//         SDL_RenderFillRect(renderer, &lenses);

//     } else {
//         // Left arm (holding gun) - same position
//         SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
//         SDL_Rect leftArm = {bodyX - armWidth, y + headHeight + bodyHeight*0.2f, armWidth, bodyHeight*0.5f};
//         SDL_RenderFillRect(renderer, &leftArm);
        
//         // Right arm (holding syringe)
//         SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
//         SDL_Rect rightArm = {bodyX + bodyWidth, y + headHeight + bodyHeight*0.2f, armWidth, bodyHeight*0.5f};
//         SDL_RenderFillRect(renderer, &rightArm);

//         // Giant syringe
//         SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
//         SDL_Rect syringe = {bodyX + bodyWidth*0.75f, y + headHeight + bodyHeight*0.4f, armWidth*1.5f, armWidth};
//         SDL_RenderFillRect(renderer, &syringe);
        
//         // Flask with glowing liquid
//         SDL_SetRenderDrawColor(renderer, 0, 200, 0, 150);  // Glowing green
//         SDL_Rect flask = {bodyX + bodyWidth*0.75, y + headHeight + bodyHeight*0.57, armWidth*2, armWidth};
//         SDL_RenderFillRect(renderer, &flask);

//         // Draw goggles looking left
//         SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // Metal frame
//         SDL_Rect gogglesFrame = {bodyX - bodyWidth * 0.1, y + headHeight*0.4f, bodyWidth*0.7f, headHeight*0.3f};
//         SDL_RenderFillRect(renderer, &gogglesFrame);
        
//         SDL_SetRenderDrawColor(renderer, 30, 144, 255, 150);  // Blue lenses
//         SDL_Rect lenses = {bodyX - bodyWidth*0.1f, y + headHeight*0.45f, bodyWidth*0.6f, headHeight*0.2f};
//         SDL_RenderFillRect(renderer, &lenses);
//     }
    
//     // Add lab coat details
//     SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
//     SDL_Rect leftPocket = {bodyX - armWidth/2, y + headHeight + bodyHeight*0.3f, armWidth, armWidth};
//     SDL_Rect rightPocket = {bodyX + bodyWidth, y + headHeight + bodyHeight*0.3f, armWidth, armWidth};
//     SDL_RenderFillRect(renderer, &leftPocket);
//     SDL_RenderFillRect(renderer, &rightPocket);
    
//     // Restore original draw color
//     SDL_SetRenderDrawColor(renderer, original.r, original.g, original.b, original.a);
// }

// DRAW WEAPONS
static void drawGun(SDL_Renderer*renderer, GunClass weaponName, int x, int y, int bodyX, int headHeight, int bodyHeight, int bodyWidth, int direction, int angle) {
    switch (weaponName) {
        case PISTOL:
            if (direction == RIGHT)
                drawPistol(renderer, bodyX + bodyWidth * 0.43f, y + headHeight + bodyHeight*0.1f, WEAPON_SIZE, RIGHT, angle);
            else
                drawPistol(renderer, bodyX + bodyWidth * 0.47f, y + headHeight + bodyHeight*0.1f, WEAPON_SIZE, LEFT, angle);
            break;
        case SHOTGUN:
            if (direction == RIGHT)
                drawShotgun(renderer, bodyX, y + headHeight + bodyHeight*0.3f, WEAPON_SIZE, RIGHT, angle);
            else
                drawShotgun(renderer, bodyX, y + headHeight + bodyHeight*0.3f, WEAPON_SIZE, LEFT, angle);
            break;
        case RIFLE:
            if (direction == RIGHT)
                drawRifle(renderer, bodyX, y + headHeight + bodyHeight*0.38f, WEAPON_SIZE, RIGHT, angle);
            else
                drawRifle(renderer, bodyX - bodyWidth*1.3f, y + headHeight + bodyHeight*0.38f, WEAPON_SIZE, LEFT, angle);
            break;
        case SNIPER:
            if (direction == RIGHT)
                drawSniper(renderer, bodyX, y + headHeight + bodyHeight*0.36f, WEAPON_SIZE, RIGHT, angle);
            else
                drawSniper(renderer, bodyX - bodyWidth*1.85, y + headHeight + bodyHeight*0.36f, WEAPON_SIZE, LEFT, angle);
            break;
        default:
            break;
    }
}

static int sign(int x) {
    return (x > 0) - (x < 0);
}

static int stateAngle(int angle) {
    //if (abs(angle) == 90) return sign(angle) * 90;
    if (abs(angle) >= 75) return sign(angle) * 75;
    if (abs(angle) >= 45) return sign(angle) * 45;
    return 0;
}

void drawShotgun(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle) {
    SDL_Surface* tempSurface = IMG_Load("assets/Weapons/Rifles/ShotgunM.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    int newSize = (int)(size * 1.65f);
    int yOffset = (int)(size * 1.8f); 
    int xOffset = 50;
    int state = stateAngle(angle);
    if (facingRight) {
        switch (state) {
            case 0: {
                SDL_Rect gunRect = {x, y + size - yOffset, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case 45: {
                SDL_Rect gunRect = {x, y + size - yOffset - 20, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case 75: {
                SDL_Rect gunRect = {x - 10, y + size - yOffset - 40, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case -45: {
                SDL_Rect gunRect = {x, y + size - yOffset + 20, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case -75: {
                SDL_Rect gunRect = {x - 20, y + size - yOffset + 20, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
        }
    } else {
        switch (state) {
            case 0: {
                SDL_Rect gunRect = {x - newSize + xOffset, y + size - yOffset, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case 45: {
                SDL_Rect gunRect = {x - newSize + xOffset, y + size - yOffset - 30, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case 75: {
                SDL_Rect gunRect = {x - newSize + xOffset + 10, y + size - yOffset - 40, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case -45: {
                SDL_Rect gunRect = {x - newSize + xOffset + 10, y + size - yOffset + 25, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case -75: {
                SDL_Rect gunRect = {x - newSize + xOffset + 20, y + size - yOffset + 30, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
        }
    } 
    SDL_DestroyTexture(texture);
}

void drawPistol(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle) {
    SDL_Surface* tempSurface = IMG_Load("assets/Weapons/Handgun/PistolD.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    int newSize = (int)(size * 1.25f);       
    int yOffset = (int)(size * 1.25f); 
    int state = stateAngle(angle);
    if (facingRight) {
        switch (state) {
            case 0: {
                SDL_Rect gunRect = {x, y + size - yOffset, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case 45: {
                SDL_Rect gunRect = {x - 5, y + size - yOffset - 25, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case 75: {
                SDL_Rect gunRect = {x - 10, y + size - yOffset - 40, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case -45: {
                SDL_Rect gunRect = {x - 25, y + size - yOffset + 20, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case -75: {
                SDL_Rect gunRect = {x - 40, y + size - yOffset + 20, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
        }
    } else {
        switch (state) {
            case 0: {
                SDL_Rect gunRect = {x - newSize, y + size - yOffset, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case 45: {
                SDL_Rect gunRect = {x - newSize + 5, y + size - yOffset - 30, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case 75: {
                SDL_Rect gunRect = {x - newSize + 15, y + size - yOffset - 40, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case -45: {
                SDL_Rect gunRect = {x - newSize + 30, y + size - yOffset + 25, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case -75: {
                SDL_Rect gunRect = {x - newSize + 50, y + size - yOffset + 30, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
        }
    } 
    SDL_DestroyTexture(texture);
}

void drawRifle(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle) {
    SDL_Surface* tempSurface = IMG_Load("assets/Weapons/Rifles/M60M.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    int newSize = (int)(size * 1.65f);
    int yOffset = (int)(size * 1.8f); 
    int xOffset = 50;
    int state = stateAngle(angle);
    if (facingRight) {
        switch (state) {
            case 0: {
                SDL_Rect gunRect = {x, y + size - yOffset, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case 45: {
                SDL_Rect gunRect = {x, y + size - yOffset - 30, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case 75: {
                SDL_Rect gunRect = {x - 10, y + size - yOffset - 40, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case -45: {
                SDL_Rect gunRect = {x, y + size - yOffset + 20, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case -75: {
                SDL_Rect gunRect = {x - 20, y + size - yOffset + 20, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
        }
    } else {
        switch (state) {
            case 0: {
                SDL_Rect gunRect = {x + xOffset, y + size - yOffset, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case 45: {
                SDL_Rect gunRect = {x + xOffset, y + size - yOffset - 30, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case 75: {
                SDL_Rect gunRect = {x + xOffset + 10, y + size - yOffset - 40, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case -45: {
                SDL_Rect gunRect = {x + xOffset + 10, y + size - yOffset + 25, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case -75: {
                SDL_Rect gunRect = {x + xOffset + 20, y + size - yOffset + 30, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
        }
    } 
    SDL_DestroyTexture(texture);
}

void drawSniper(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle) {
    SDL_Surface* tempSurface = IMG_Load("assets/Weapons/Rifles/CarbineD.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    int newSize = (int)(size * 1.8f);
    int yOffset = (int)(size * 1.8f); 
    int xOffset = 90;
    int state = stateAngle(angle);
    if (facingRight) {
        switch (state) {
            case 0: {
                SDL_Rect gunRect = {x - 20, y + size - yOffset, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case 45: {
                SDL_Rect gunRect = {x - 20, y + size - yOffset - 20, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case 75: {
                SDL_Rect gunRect = {x - 15, y + size - yOffset - 40, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case -45: {
                SDL_Rect gunRect = {x - 20, y + size - yOffset + 10, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
            case -75: {
                SDL_Rect gunRect = {x - 35, y + size - yOffset + 20, newSize, newSize};
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, -state, NULL, SDL_FLIP_NONE);
                break;
            }
        }
    } else {
        switch (state) {
            case 0: {
                SDL_Rect gunRect = {x + xOffset, y + size - yOffset, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case 45: {
                SDL_Rect gunRect = {x + xOffset, y + size - yOffset - 20, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case 75: {
                SDL_Rect gunRect = {x + xOffset - 10, y + size - yOffset - 40, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case -45: {
                SDL_Rect gunRect = {x + xOffset + 10, y + size - yOffset + 5, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
            case -75: {
                SDL_Rect gunRect = {x + xOffset + 15, y + size - yOffset + 25, newSize, newSize };
                SDL_RenderCopyEx(renderer, texture, NULL, &gunRect, state, NULL, SDL_FLIP_HORIZONTAL);
                break;
            }
        }
    }
    SDL_DestroyTexture(texture); 
}

void drawRPG(SDL_Renderer* renderer, int x, int y, int size, bool facingRight, int angle) {
    // Save the current draw color
    SDL_Color original;
    SDL_GetRenderDrawColor(renderer, &original.r, &original.g, &original.b, &original.a);
    
    size *= 1.5; // RPGs are typically larger than rifles
    // Calculate parts based on size
    int barrelLength = size * 0.8f;
    int barrelWidth = size * 0.25f;
    int bodyLength = size * 0.3f;
    int bodyWidth = size * 0.3f;
    int gripLength = size * 0.2f;
    int gripWidth = size * 0.15f;
    int triggerLength = size * 0.1f;
    int triggerWidth = size * 0.05f;
    int sightLength = size * 0.15f;
    int sightWidth = size * 0.05f;
    int rocketLength = size * 0.2f;
    int rocketWidth = size * 0.15f;

    if (facingRight) {
        // Draw main body (launcher tube)
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
        SDL_Rect body = {
            x, 
            y + size * 0.1f, 
            barrelLength, 
            barrelWidth
        };
        SDL_RenderFillRect(renderer, &body);
        
        // Draw back body (where the rocket exits)
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_Rect backBody = {
            x - bodyLength * 0.5f, 
            y + size * 0.05f, 
            bodyLength, 
            bodyWidth
        };
        SDL_RenderFillRect(renderer, &backBody);
        
        // Draw pistol grip
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_Rect grip = {
            x + barrelLength * 0.2f, 
            y + barrelWidth, 
            gripLength, 
            gripWidth
        };
        SDL_RenderFillRect(renderer, &grip);
        
        // Draw trigger
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_Rect trigger = {
            x + barrelLength * 0.25f, 
            y + barrelWidth * 0.7f, 
            triggerLength, 
            triggerWidth
        };
        SDL_RenderFillRect(renderer, &trigger);
        
        // Draw front sight
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_Rect frontSight = {
            x + barrelLength * 0.8f, 
            y, 
            sightWidth, 
            sightLength
        };
        SDL_RenderFillRect(renderer, &frontSight);
        
        // Draw rear sight
        SDL_Rect rearSight = {
            x + barrelLength * 0.1f, 
            y, 
            sightWidth, 
            sightLength * 1.2f
        };
        SDL_RenderFillRect(renderer, &rearSight);
        
        // Draw rocket projectile (loaded)
        SDL_SetRenderDrawColor(renderer, 150, 100, 50, 255);
        SDL_Rect rocket = {
            x + barrelLength * 0.9f, 
            y + barrelWidth * 0.3f, 
            rocketLength, 
            rocketWidth
        };
        SDL_RenderFillRect(renderer, &rocket);
        
        // Rocket tip
        SDL_SetRenderDrawColor(renderer, 180, 120, 60, 255);
        SDL_Rect rocketTip = {
            x + barrelLength * 0.9f + rocketLength, 
            y + barrelWidth * 0.3f, 
            rocketLength * 0.3f, 
            rocketWidth
        };
        SDL_RenderFillRect(renderer, &rocketTip);
        
    } else {
        // Draw main body (launcher tube)
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
        SDL_Rect body = {
            x - barrelLength, 
            y + size * 0.1f, 
            barrelLength, 
            barrelWidth
        };
        SDL_RenderFillRect(renderer, &body);
        
        // Draw back body (where the rocket exits)
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_Rect backBody = {
            x, 
            y + size * 0.05f, 
            bodyLength, 
            bodyWidth
        };
        SDL_RenderFillRect(renderer, &backBody);
        
        // Draw pistol grip
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_Rect grip = {
            x - barrelLength * 0.3f, 
            y + barrelWidth, 
            gripLength, 
            gripWidth
        };
        SDL_RenderFillRect(renderer, &grip);
        
        // Draw trigger
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_Rect trigger = {
            x - barrelLength * 0.2f, 
            y + barrelWidth * 0.7f, 
            triggerLength, 
            triggerWidth
        };
        SDL_RenderFillRect(renderer, &trigger);
        
        // Draw front sight (now on the left side)
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_Rect frontSight = {
            x - barrelLength * 0.85f, 
            y, 
            sightWidth, 
            sightLength
        };
        SDL_RenderFillRect(renderer, &frontSight);
        
        // Draw rear sight (now on the right side)
        SDL_Rect rearSight = {
            x + bodyLength*0.1f, 
            y, 
            sightWidth, 
            sightLength * 1.2f
        };
        SDL_RenderFillRect(renderer, &rearSight);
        
        // Draw rocket projectile (loaded)
        SDL_SetRenderDrawColor(renderer, 150, 100, 50, 255);
        SDL_Rect rocket = {
            x - barrelLength * 0.9f - rocketLength, 
            y + barrelWidth * 0.3f, 
            rocketLength, 
            rocketWidth
        };
        SDL_RenderFillRect(renderer, &rocket);
        
        // Rocket tip (pointing left)
        SDL_SetRenderDrawColor(renderer, 180, 120, 60, 255);
        SDL_Rect rocketTip = {
            x - barrelLength * 0.1f - rocketLength*4.3f, 
            y + barrelWidth * 0.3f, 
            rocketLength * 0.3f, 
            rocketWidth
        };
        SDL_RenderFillRect(renderer, &rocketTip);
    }
    
    // Restore original draw color
    SDL_SetRenderDrawColor(renderer, original.r, original.g, original.b, original.a);
}

// void renderTile(SDL_Renderer *renderer, SDL_Texture *spriteSheet, int spriteX, int spriteY, int posX, int posY, float scale) {
//     SDL_Rect src = {
//         spriteX * OBJ_SPRITE_WIDTH,  
//         spriteY * OBJ_SPRITE_HEIGHT,  
//         OBJ_SPRITE_WIDTH,
//         OBJ_SPRITE_HEIGHT
//     };

//     SDL_Rect dst = {
//         posX,
//         posY,
//         (int)(OBJ_SPRITE_WIDTH * scale),
//         (int)(OBJ_SPRITE_HEIGHT * scale)
//     };

//     SDL_RenderCopy(renderer, spriteSheet, &src, &dst);
// }

SDL_Texture* loadTexture(char* file, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(file);
    if (!surface) {
        printf("IMG_Load error: %s\n", IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        printf("Texture failed to be created\n");
    }

    return texture;
}

void renderPlatformImage(GameObject obj) {
    int centerX = 2 * OBJ_SPRITE_WIDTH;
    int centerY = 2 * OBJ_SPRITE_HEIGHT;

    for(int i = 0; i<obj->width; i+=OBJ_SPRITE_WIDTH * OBJ_SCALE) {
        for(int j = 0; j<obj->height; j+=OBJ_SPRITE_HEIGHT * OBJ_SCALE) {
            int sheetX = centerX;
            int sheetY = centerY;
            if(i == 0) sheetX -= OBJ_SPRITE_WIDTH;
            else if(i == obj->width - OBJ_SPRITE_WIDTH * OBJ_SCALE) sheetX += OBJ_SPRITE_WIDTH;

            if(j == 0) sheetY -= OBJ_SPRITE_HEIGHT;
            else if(j == obj->height - OBJ_SPRITE_HEIGHT * OBJ_SCALE) sheetY += OBJ_SPRITE_HEIGHT;


            SDL_Rect src = {sheetX, sheetY, OBJ_SPRITE_WIDTH, OBJ_SPRITE_HEIGHT};
            SDL_Rect dst = {obj->coord.x + i, obj->coord.y + j, OBJ_SPRITE_WIDTH* OBJ_SCALE, OBJ_SPRITE_HEIGHT* OBJ_SCALE};
            SDL_RenderCopy(renderer, dungeonTileSheet, &src, &dst);
        }
    }
}

void renderPassablePlatformImage(GameObject obj) {
    int centerX = 2 * OBJ_SPRITE_WIDTH;
    int centerY = 10 * OBJ_SPRITE_HEIGHT;

    for(int i = 0; i<obj->width; i+=OBJ_SPRITE_WIDTH * PASSABLE_PLAT_SCALE) {
        int sheetX = centerX;
        int sheetY = centerY;
        if(i == 0) sheetX -= OBJ_SPRITE_WIDTH;
        else if(i == obj->width - OBJ_SPRITE_WIDTH * PASSABLE_PLAT_SCALE) sheetX += OBJ_SPRITE_WIDTH;

        SDL_Rect src = {sheetX, sheetY, OBJ_SPRITE_WIDTH, OBJ_SPRITE_HEIGHT};
        SDL_Rect dst = {obj->coord.x + i, obj->coord.y - PASSABLE_PLAT_SCALE, OBJ_SPRITE_WIDTH * PASSABLE_PLAT_SCALE, OBJ_SPRITE_HEIGHT* PASSABLE_PLAT_SCALE };
        SDL_RenderCopy(renderer, dungeonTileSheet, &src, &dst);
    }
}

void renderLadderImage(GameObject obj) {
    int centerX = 5 * OBJ_SPRITE_WIDTH;
    int centerY = 10 * OBJ_SPRITE_HEIGHT;
    int scale = LADDER_SCALE;
    SDL_Rect src = {centerX, centerY - 2* OBJ_SPRITE_HEIGHT, OBJ_SPRITE_WIDTH, OBJ_SPRITE_HEIGHT};
    SDL_Rect dst = {obj->coord.x , obj->coord.y - OBJ_SPRITE_HEIGHT*scale, OBJ_SPRITE_WIDTH * scale, OBJ_SPRITE_HEIGHT * scale};
    SDL_RenderCopy(renderer, dungeonTileSheet, &src, &dst);

    int i;
    for(i = 0; i<obj->height; i+=OBJ_SPRITE_HEIGHT * scale) {
        int sheetX = centerX;
        int sheetY = centerY;

        if(i == 0) sheetY -= OBJ_SPRITE_HEIGHT;
        else if(i == obj->height - OBJ_SPRITE_HEIGHT * scale) sheetY += OBJ_SPRITE_HEIGHT;

        SDL_Rect src = {sheetX, sheetY, OBJ_SPRITE_WIDTH, OBJ_SPRITE_HEIGHT};
        SDL_Rect dst = {obj->coord.x , obj->coord.y + i, OBJ_SPRITE_WIDTH * scale, OBJ_SPRITE_HEIGHT * scale};
        SDL_RenderCopy(renderer, dungeonTileSheet, &src, &dst);
    }
}

void renderHazardImage(GameObject obj) {
    int startX = 1 * OBJ_SPRITE_WIDTH;
    int startY = 14 * OBJ_SPRITE_HEIGHT;  // Keep it integer
    int scale = HAZARD_SCALE;

    int spriteHeight = OBJ_SPRITE_HEIGHT * scale;

    for (int i = 0; i < obj->width; i += OBJ_SPRITE_WIDTH * scale) {
        int sheetX = startX;
        int sheetY = startY;

        SDL_Rect src = {
            sheetX,
            sheetY,
            OBJ_SPRITE_WIDTH,
            OBJ_SPRITE_HEIGHT
        };

        SDL_Rect dst = {
            obj->coord.x + i,
            obj->coord.y + obj->height - spriteHeight,
            OBJ_SPRITE_WIDTH * scale,
            spriteHeight
        };

        SDL_RenderCopy(renderer, dungeonTileSheet, &src, &dst);
    }
}