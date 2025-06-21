#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>
#include "physics.h"
#include "globalConsts.h"
#include <math.h>

#define MOV_PLAT_LENIANCY 0.01

double roundToNearest(double value, double step) {
    return round(value / step) * step;
}
// Description: checks colision between two rects
bool checkColRects(double x1, double y1, double w1, double h1, double x2, double y2, double w2, double h2) {
    return (x1 < x2 + w2) &&
           (x2 < x1 + w1) &&
           (y1 < y2 + h2) &&
           (y2 < y1 + h1);
}
// Description: check colision between point and rectangle
bool checkPointInRect(double px, double py, double rx, double ry, double rw, double rh) {
    return (px >= rx) &&
           (px <= rx + rw) &&
           (py >= ry) &&
           (py <= ry + rh);
}

bool checkColObjects (GameObject obj, GameObject obj2) {
    if(strcmp(obj2->tag,"ladder")  == 0) return false;
    return checkColRects(obj->coord.x,obj->coord.y,obj->width,obj->height,obj2->coord.x,obj2->coord.y,obj2->width,obj2->height);
}

bool checkColPlatform (GameObject obj, Player player) {
    return checkColRects(obj->coord.x,obj->coord.y,obj->width,obj->height,player->coord.x, player->coord.y, player->width,player->height);
}

bool checkColBullet (GameObject obj, Player player) {
    return checkColRects(player->coord.x, player->coord.y, player->width,player->height,obj->coord.x,obj->coord.y,obj->width,obj->height);
}


bool checkColItem (GameObject obj, Player player) {
    return checkColRects(player->coord.x, player->coord.y, player->width,player->height,obj->coord.x,obj->coord.y,obj->width,obj->height);
}

void GameObjectOnColCheck (GameObject obj, GameObject obj2) {
    invalidateBullet(obj);
}

void onPlatformCollisionX (GameObject obj,Player player) {
    MoveXTag xTag = player->xTag;
    if(xTag == MOVING_RIGHT) {
        player->coord.x = obj->coord.x-player->width;
    } else if (xTag == MOVING_LEFT) {
        player->coord.x = obj->coord.x+obj->width;
    } else {
        fprintf(stderr,"UNDEFINED BEHAVIOR - x collision while standing still");
    }
    player->cur_vel.x = 0;
}

void onPlatformCollisionY (GameObject obj,Player player) {
    MoveYTag yTag = player->yTag;
    if(yTag == MOVING_DOWN) {
        player->coord.y = obj->coord.y - player->height;
        player->on_ground = true;
        player->canJump = true;
    } else if(yTag == MOVING_UP) {
        player->coord.y = obj->coord.y + obj->height;
    } else {
        fprintf(stderr,"UNDEFINED BEHAVIOR - y collision while standing still");
    }
    player->cur_vel.y = 0;
}

void onJumpPadCollisionY (GameObject obj, Player player) {
    JumpPadData jumpData = obj->extraData;
    player->coord.y = obj->coord.y - player->height;
    player->on_ground = true; //reset jump
    player->canJump = true;
    player->cur_vel.y = -jumpData->bounceVel;
    bounce_pad_effect();
}

void onSidewaysJumpPadCollisionX (GameObject obj, Player player) {
    JumpPadData jumpData = obj->extraData;
    player->cur_vel.y = -jumpData->bounceVel;
    if(player->xTag == MOVING_RIGHT) {
        player->coord.x = obj->coord.x-player->width;
        player->cur_vel.x = -jumpData->bounceVel;
    } else if (player->xTag == MOVING_LEFT) {
        player->coord.x = obj->coord.x+obj->width;
        player->cur_vel.x = jumpData->bounceVel;
    } else {
         fprintf(stderr,"UNDEFINED BEHAVIOR - x collision on bounce pad while standing still");
    } 
    bounce_pad_effect();
}

void alignPlayerX(GameObject obj, Player player) {
    MovingPlatformData platData = obj->extraData;

    double nextPlatX = obj->coord.x + platData->vel.x * delta_time;
    double targetFracX = fmod(nextPlatX, 1.0);

    double nextPlayerX = player->coord.x + player->cur_vel.x * delta_time;
    double alignedX = floor(nextPlayerX) + targetFracX;

    player->coord.x = alignedX - player->cur_vel.x * delta_time;
}

void alignPlayerY(GameObject obj, Player player) {
    MovingPlatformData platData = obj->extraData;

    double nextPlatY = obj->coord.y + platData->vel.y * delta_time;
    double targetFracY = fmod(nextPlatY, 1.0);

    double nextPlayerY = player->coord.y + player->cur_vel.y * delta_time;
    double alignedY = floor(nextPlayerY) + targetFracY;

    player->coord.y = alignedY - player->cur_vel.y * delta_time;
}


void movingPlatformFrameStartCollisionX(GameObject obj, Player player, bool goingRight) {
    if((obj->colCheck)(obj,player)) {
        if(!goingRight) {
            player->coord.x = obj->coord.x-player->width;
        } else {
            player->coord.x = obj->coord.x+obj->width;
        }
    }
}

void movingPlatformFrameStartCollisionY(GameObject obj, Player player, bool goingDown) {
    if((obj->colCheck)(obj,player)) {
        if(!goingDown) {
            player->coord.y = obj->coord.y-player->height;
        } else {
            player->coord.y = obj->coord.y+obj->height;
        }
    }
}

void onMovingPlatformFrameStart(GameObject obj) {
    MovingPlatformData movPlatData = obj->extraData;
    //printf("xpos : %f ypos: %f xvel: %f yvel: %f dt: %f curTime: %u\n",obj->coord.x,obj->coord.y,movPlatData->vel.x,movPlatData->vel.y,delta_time,SDL_GetTicks());
    //Note: Using num player for accounting that the funct is run for each player
    if(movPlatData->curDirectionMoveTime >= movPlatData->timeToReachEnd) {
        Player players[] = { game_settings->player1, game_settings->player2 };
        for (int i = 0; i < 2; i++) {
            Player p = players[i];

            if(p->yTag != MOVING_DOWN) continue;

            // Predict next Y position after gravity (or small downward move)
            double predictedY = p->coord.y + p->cur_vel.y * delta_time;

            // Check collision as if player was at predicted Y
            if (checkColRects(obj->coord.x, obj->coord.y, obj->width, obj->height,
                                p->coord.x, predictedY, p->width, p->height)) {
                //Snap player to platform
                p->coord.y = (movPlatData->goingToEnd ? movPlatData->endPos.y : movPlatData->startPos.y) - p->height;
            }
        }
        printf("OLD COORDS: %f, %f\n",obj->coord.x,obj->coord.y);
        obj->coord = (movPlatData->goingToEnd ? movPlatData->endPos : movPlatData->startPos);
        printf("NEW COORDS: %f, %f\n",obj->coord.x,obj->coord.y);
        movPlatData->goingToEnd = !movPlatData->goingToEnd;
        movPlatData->vel.x = -movPlatData->vel.x;
        movPlatData->vel.y = -movPlatData->vel.y;
        movPlatData->curDirectionMoveTime = 0;
        return;
    }
    double platXVel = movPlatData->vel.x * delta_time;
    double platYVel = movPlatData->vel.y * delta_time;
    bool platformGoingRight = platXVel > 0;
    bool platformGoingDown  = platYVel > 0;
    
    obj->coord.x += platXVel;
    movingPlatformFrameStartCollisionX(obj,game_settings->player1, platformGoingRight);
    movingPlatformFrameStartCollisionX(obj,game_settings->player2, platformGoingRight);

    Player players[] = { game_settings->player1, game_settings->player2 };
    for (int i = 0; i < 2; i++) {
        Player p = players[i];

        if(p->yTag != MOVING_DOWN) continue;

        // Predict next Y position after gravity (or small downward move)
        double predictedY = p->coord.y + p->cur_vel.y * delta_time;

        // Check collision as if player was at predicted Y
        if (checkColRects(obj->coord.x, obj->coord.y, obj->width, obj->height,
                            p->coord.x, predictedY, p->width, p->height)) {
            // Snap player down onto platform
            p->coord.y = obj->coord.y - p->height + platYVel;
        }
    
    }

    obj->coord.y += platYVel;
    movingPlatformFrameStartCollisionY(obj,game_settings->player1, platformGoingDown);
    movingPlatformFrameStartCollisionY(obj,game_settings->player2, platformGoingDown);

    movPlatData->curDirectionMoveTime += delta_time;
}

void onMovingPlatformCollisionY (GameObject obj,Player player) {
    MovingPlatformData movPlatData = obj->extraData;
    onPlatformCollisionY(obj,player);
    double platXVel = movPlatData->vel.x * delta_time;
    double platYVel = movPlatData->vel.y * delta_time;
    player->coord.x += platXVel;
    // player->coord.y += platYVel;
    setMovementFlagsFromVel(player,platXVel+player->cur_vel.x,platYVel+player->cur_vel.y);
}



// Fixed functions for moving guns and powers with platforms
void moveGunWithPlatform(GameObject obj, GameObject platform) {
    if (platform == NULL || platform->extraData == NULL) return;
    
    // Platform's extraData is MovingPlatformData
    MovingPlatformData plat_data = (MovingPlatformData)platform->extraData;
    if (plat_data == NULL) return;
    
    double plat_x_vel = plat_data->vel.x * delta_time;
    double plat_y_vel = plat_data->vel.y * delta_time;
    
    obj->coord.x += plat_x_vel;
    obj->coord.y += plat_y_vel;
}

void movePowerWithPlatform(GameObject obj, GameObject platform) {
    if (platform == NULL || platform->extraData == NULL) return;
    
    // Platform's extraData is MovingPlatformData
    MovingPlatformData plat_data = (MovingPlatformData)platform->extraData;
    if (plat_data == NULL) return;
    
    double plat_x_vel = plat_data->vel.x * delta_time;
    double plat_y_vel = plat_data->vel.y * delta_time;
    
    obj->coord.x += plat_x_vel;
    obj->coord.y += plat_y_vel;
}

void onMovingPlatformGunFrameStart(GameObject obj) {
    GunType* gun_data = (GunType*)obj->extraData;
    if (gun_data != NULL && gun_data->moving_plat != NULL) {
        moveGunWithPlatform(obj, gun_data->moving_plat);
    }
}

void onMovingPlatformPowerFrameStart(GameObject obj) {
    PowerType* power_data = (PowerType*)obj->extraData;
    if (power_data != NULL && power_data->moving_plat != NULL) {
        movePowerWithPlatform(obj, power_data->moving_plat);
    }
}

bool canPassThrough(GameObject obj, Player player) {
    PassablePlatformData pData = obj->extraData;
    return getIndexOf(pData->playerCanPassThrough,player) != -1;
}

void addPlayerCanPassThrough(GameObject obj, Player player) {
    PassablePlatformData pData = obj->extraData;
    insertDynList(pData->playerCanPassThrough,player);
}

void removePlayerCanPassThrough(GameObject obj, Player player) {
    PassablePlatformData pData = obj->extraData;
    removeDynList(pData->playerCanPassThrough,player);
}

void onPassablePlatformCollisionX (GameObject obj,Player player) {
    if(!canPassThrough(obj,player)) {
        addPlayerCanPassThrough(obj,player);
    }
}

void onPassablePlatformCollisionY (GameObject obj,Player player) {
    if(player->yTag == MOVING_UP) {
        addPlayerCanPassThrough(obj,player);
    }
    else if(!canPassThrough(obj,player) && player->yTag == MOVING_DOWN) {
        if(player->is_crouching) {
            addPlayerCanPassThrough(obj,player);
        } else {
            onPlatformCollisionY(obj,player);
        }
    }
}

void onPassablePlatformFrameEnd (GameObject obj) {
    Player p1 = game_settings->player1;
    Player p2 = game_settings->player2;
    if(canPassThrough(obj,p1) && !checkColPlatform(obj,p1)) {
        removePlayerCanPassThrough(obj,p1);
    }
    if(canPassThrough(obj,p2) && !checkColPlatform(obj,p2)) {
        removePlayerCanPassThrough(obj,p2);
    }
}

void onHazardCollisionX (GameObject obj,Player player) {
    MoveXTag xTag = player->xTag;
    HazardData hazardData = obj->extraData;

    onPlatformCollisionX(obj,player);

    if(xTag == MOVING_RIGHT) {
        player->cur_vel.x = -hazardData->knockback;
    } else if (xTag == MOVING_LEFT) {
        player->cur_vel.x = hazardData->knockback;
    } else {
        fprintf(stderr,"UNDEFINED BEHAVIOR - x collision on hazard while standing still");
    }

    takeDamage(player,hazardData->damage);
    got_shot_effect();
}

void onHazardCollisionY (GameObject obj,Player player) {
    MoveYTag yTag = player->yTag;
    HazardData hazardData = obj->extraData;

    onPlatformCollisionY(obj,player);
    // printf("ytag : %d, xtag: %d\n", yTag, player->xTag);
    
    if(yTag == MOVING_DOWN) {
        player->cur_vel.y = -hazardData->knockback;
        player->coord.y -= 1;
    } else if (yTag == MOVING_UP) {
        player->cur_vel.y = hazardData->knockback;
        player->coord.y += 1;
    } else {
        fprintf(stderr,"UNDEFINED BEHAVIOR - y collision on hazard while standing still");
    }
    takeDamage(player,hazardData->damage);
    got_shot_effect();
}


void onLadderFrameEnd (GameObject obj) {
    Player p1 = game_settings->player1;
    Player p2 = game_settings->player2;
    if(!checkColPlatform(obj,p1)) {
        p1->onLadder = false;
    }
    if(!checkColPlatform(obj,p2)) {
        p2->onLadder = false;
    }
}

void onLadderFrameStart (GameObject obj) {
    LadderData ladderData = obj->extraData;
    Player players[] = { game_settings->player1, game_settings->player2 };
    for (int i = 0; i < 2; i++) {
        Player player = players[i];
        if(checkColPlatform(obj,player)) {
            if(player->keyYTag == KEY_UP || player->keyYTag == KEY_DOWN) {
                player->onLadder = true;
            } else if(!player->onLadder) {
                continue;
            }

            if(player->keyXTag == KEY_LEFT) {
                player->cur_vel.x = -ladderData->ladderMoveSpeed;
                player->xTag = MOVING_LEFT;
            } else if(player->keyXTag == KEY_RIGHT) {
                player->cur_vel.x = ladderData->ladderMoveSpeed;
                player->xTag = MOVING_RIGHT;
            } else {
                player->cur_vel.x = 0;
                player->xTag = STILL_X;
            }

            if(player->keyYTag == KEY_UP) {
                player->cur_vel.y = -ladderData->ladderClimbSpeed;
                player->yTag = MOVING_UP;
            } else if(player->keyYTag == KEY_DOWN) {
                player->cur_vel.y = ladderData->ladderDropSpeed;
                player->yTag = MOVING_DOWN;
            } else {
                player->cur_vel.y = 0;
                player->yTag = STILL_Y;
            }
        }
    }
}
#endif