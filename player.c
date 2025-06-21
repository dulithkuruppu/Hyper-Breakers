#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "globalConsts.h"
#include "player.h"
#include "level.h"
#include "physics.h"
#include "render.h"
#include "weapon.h"
#include "bar.h"
#include "audio.h"
#include "sprites.h"
#define RIGHT true
#define LEFT false
#define FRICTION_DECELERATION 0.065
#define MINIMUM_MOVING_VELOCITY 0.02
#define JUMP_VELOCITY 325
#define INITIAL_SHOOTING_ANGLE 0.0
#define MINIMUM_SHOOTING_ANGLE -90.0
#define MAXIMUM_SHOOTING_ANGLE 90.0
#define TOTAL_ANGLE 180.0
#define AIMING_DISTANCE 150.0
#define BULLET_VELOCITY 500
#define DEG_TO_RAD (M_PI / 180.0)
#define MULTISHOT_COOLDOWN 0.075 // COOLDOWN BETWEEN SHOTS IN A BURST
#define SHOOT_COOLDOWN 0.3

SDL_TimerID timer;

char* winMessage;

// Get next player id
int getNextID(void) {
    static int id = 1;
    return id++;
}
// Description: initializes players 
Player initPlayers(const char* name) {
    
    // Player initialization
    Player player        = malloc(sizeof (struct Player));
    player->id           = getNextID();
    player->character    = name;
    player->coord.x      = (rand() % WINDOW_WIDTH);
    player->coord.y      = 0;
    player->width        = 80;
    player->height       = 100;
    player->speed        = 5;
    player->healthBar    = createHealthBar(player,RED_COLOR,15);
    player->cur_vel.x    = 0;
    player->cur_vel.y    = 0;

    player->is_shooting  = false;
    player->facing_direction = (rand() % 2);
    player->on_ground = false;
    player->holding_shoot = false;
    player->is_crouching = false;
    player->is_moving = false;
    player->shooting_angle = INITIAL_SHOOTING_ANGLE;
    player->goingRight = false;
    player->goingDown = false;
    player->xTag = STILL_X;
    player->yTag = STILL_Y;
    player->keyXTag = KEY_NONE_X;
    player->keyYTag = KEY_NONE_Y;
    player->aiming_direction.x = 0;
    player->aiming_direction.y = 0;
    player->noKills = 0;
    player->last_shot = 0;
    player->bullets_to_Shoot = 0;
    player->onLadder = false;
    player->isHit = false;

    // Weapon initialization
    player->weapon       = malloc( sizeof(struct Weapon) );
    player->weapon->gun  = PISTOL;
    fetch_stats(player->weapon);

    // Power initialization
    Powers powers                   = malloc(sizeof(struct Powers));
    player->powers                  = *powers;
    player->powers.run_faster       = false;
    player->powers.increase_health  = false;
    player->powers.jump_higher      = false;
    
    // int x, y;
    // try_spawn_on_surface(surface, MAX_WIDTH, MAX_HEIGHT, &x, &y, 1);


    return player;
}

// Description: handle player deaths
void died(Player player, GameObject bullet) {
    Player killer = ((BulletData*)(bullet->extraData))->owner;
    // printf("Player %s was killed by %s using the %u\n", player->character, killer->character, ((BulletData*)(bullet->extraData))->source_weapon);
    killer->noKills ++;
    Mix_HaltChannel(6);
    died_effect();
    // addGenericTextBox(findCurScreen(cur_screen),winMessage,WINDOW_WIDTH/2,100);
}

// Description: handles player taking damage
void takeDamage(Player player, int damage) {
    removePoints(player->healthBar, damage);
    if (player->healthBar->curPoints <= 0) {
        died_effect();
    }
    player->onLadder = false;
    got_shot_effect();
}
// Description: handles player being hit by bullet 
void hit(GameObject bullet,Player player) {  
    if (((BulletData*)bullet->extraData)->owner == player){
        return;
    }
    int dmg = weapon_damage(((BulletData*)(bullet->extraData))->source_weapon);
    if (player->healthBar->curPoints >= 0) {
        takeDamage(player, dmg);
    }
    BulletData *bulletData = bullet->extraData;
    player->cur_vel.x += dmg * bulletData->vel.x / 100.0;
    player->cur_vel.y += dmg * bulletData->vel.y  / 80.0;
    if (bulletData->vel.x == 0) player->xTag = STILL_X;
    else {
        player->xTag = (bulletData->vel.x > 0 ? MOVING_RIGHT : MOVING_LEFT);
    }
    if (bulletData->vel.y == 0) player->yTag = STILL_Y;
    else {
        player->yTag = (bulletData->vel.y > 0 ? MOVING_DOWN : MOVING_UP);
    }
    if (bulletData->vel.x != 0 && bulletData->vel.y != 0) {
    }
    // printf("health is %d\n", player->healthBar->curPoints);
    if (player->healthBar->curPoints <= 0) {
        died(player, bullet);
    }
    removeDynList(curLevel->objects, bullet);
    bullet->onFree(bullet); 
    return; 
}

void updateBullet(GameObject bullet){
    bullet->coord.x += ((BulletData*)(bullet->extraData))->vel.x * delta_time;
    bullet->coord.y += ((BulletData*)(bullet->extraData))->vel.y * delta_time;
    if (bullet->coord.x > WINDOW_WIDTH || bullet->coord.y > WINDOW_HEIGHT || 
        bullet->coord.x < 0 || bullet->coord.y < 0) {
        removeBullet(bullet);
    }
}

void shootBulletEnd(Player player) {
    if (player->bullets_to_Shoot <= 0) {
        player->shooting_angle = INITIAL_SHOOTING_ANGLE;
        return;
    }
    if (SDL_GetTicks()/1000.0 - player->last_shot < MULTISHOT_COOLDOWN && player->weapon->gun != SHOTGUN ) return;

    GameObject obj = createEmptyGameObject();
    assert(obj != NULL);
        
    strncpy(obj->tag, "bullet", sizeof(obj->tag));
    // Convert angle to radians
    double angle_rad = player->shooting_angle * DEG_TO_RAD;

    // Define gun offset relative to top-left corner
    float gun_offset_x = player->width * 0.75f;   // near right edge
    float gun_offset_y = player->height * 0.35f;  // slightly down from top

    // Flip gun offset horizontally if facing left
    if (!player->facing_direction) {
        gun_offset_x = player->width - gun_offset_x;
    }

    // Base position of gun barrel in world space
    float origin_x = player->coord.x + gun_offset_x;
    float origin_y = player->coord.y + gun_offset_y;

    // How far to spawn bullet away from gun barrel
    float bullet_spawn_distance = 10.0f;

    float offset_x = cos(angle_rad) * bullet_spawn_distance;
    float offset_y = sin(angle_rad) * bullet_spawn_distance;

    // Final bullet position in world space
    obj->coord.x = origin_x + offset_x;
    obj->coord.y = origin_y + offset_y;

    obj->extraData = malloc(sizeof(BulletData));
    assert(obj->extraData != NULL);
    obj->width = 3;
    obj->height = 3;
    obj->colCheck = checkColBullet;
    obj->onRender = renderBullet;
    obj->onColX = hit;
    obj->onColY = hit;
    obj->onFrameStart = updateBullet;
    obj->GameObjectColCheck = checkColObjects;
    obj->GameObjectOnColCheck = GameObjectOnColCheck;
    double randomSpread = ((double)rand() / RAND_MAX) *  -player->weapon-> spread  * DEG_TO_RAD ;
    ((BulletData*)(obj->extraData))->vel.x = (player->facing_direction ? player->weapon->bulletSpeed * cos(DEG_TO_RAD * -player->shooting_angle + randomSpread) : -player->weapon->bulletSpeed * cos(DEG_TO_RAD * player->shooting_angle + randomSpread));
    ((BulletData*)(obj->extraData))->vel.y = (player->facing_direction ? player->weapon->bulletSpeed* sin(DEG_TO_RAD * -player->shooting_angle + randomSpread) : -player->weapon->bulletSpeed * sin(DEG_TO_RAD * player->shooting_angle + randomSpread));
    ((BulletData*)(obj->extraData)) -> source_weapon = player -> weapon->gun;
    ((BulletData*)(obj->extraData)) -> owner = player;

    insertDynList(curLevel -> objects,obj);
    // printf("bullet spawned %d remaining of %d!\n", player->weapon->currentAmmo - 1,player->weapon->totalAmmo -1 );
    player->weapon->currentAmmo--;
    player->weapon->totalAmmo--;
    player->bullets_to_Shoot--;
    player->last_shot = SDL_GetTicks()/1000.0;
}

void shootBullet(Player player) {
    // check ammo
    if (SDL_GetTicks()/1000.0 - player->last_shot < SHOOT_COOLDOWN ) return;
    if (player->weapon->totalAmmo <= 0) {
        return;
    }
    //reload if possible
    if (player->weapon->currentAmmo <= 0 && player->weapon->reloaded) {
        player->weapon-> reload_start = last_frame_time/1000;
        player->weapon->reloaded = false;
        out_of_ammo_effect();
        return;
    }
    if (!player->weapon->reloaded){
        if (last_frame_time/1000 - player->weapon-> reload_start > player->weapon-> reload_time){
            player->weapon->currentAmmo = player->weapon->magazineSize;
            player->weapon->reloaded = true;
            player->weapon->reload_end = last_frame_time;
            reload_effect();
        } else {
            return;
        }
    }

    // if (player->weapon->reloaded &&
    //     last_frame_time - player->weapon->reload_end < RELOAD_BUFFER_TIME) {
    //     return;
    // }
    
    for (int i = 0; i < player->weapon->burst; i++){
       player->bullets_to_Shoot++;
    }
    play_relevant_gun_sound (player->weapon->gun);
}

void invalidateBullet(GameObject bullet){
    bullet->coord.x = WINDOW_WIDTH  + 1000;
}

void removeBullet(GameObject bullet){
    removeDynList(curLevel->objects, bullet);
    bullet->onFree(bullet); 
}

void updateAimingIndicatorTest(Player player) {
    double angle_rad = player->shooting_angle * DEG_TO_RAD;

    // Define gun offset relative to top-left corner
    float gun_offset_x = player->width * 0.75f;   // near right edge
    float gun_offset_y = player->height * 0.35f;  // slightly down from top

    // Flip gun offset horizontally if facing left
    if (!player->facing_direction) {
        gun_offset_x = player->width - gun_offset_x;
    }

    // Base position of gun barrel in world space
    float origin_x = player->coord.x + gun_offset_x;
    float origin_y = player->coord.y + gun_offset_y;

    // How far to spawn bullet away from gun barrel
    float bullet_spawn_distance = 10.0f;

    float offset_x = cos(angle_rad) * bullet_spawn_distance;
    float offset_y = sin(angle_rad) * bullet_spawn_distance;

    // Final bullet position in world space

    player->aiming_direction.x = origin_x + offset_x + (player->facing_direction ? AIMING_DISTANCE * cos(DEG_TO_RAD * -player->shooting_angle) : -AIMING_DISTANCE * cos(DEG_TO_RAD * player->shooting_angle));
    player->aiming_direction.y = origin_y + offset_y + (player->facing_direction ? AIMING_DISTANCE * sin(DEG_TO_RAD * -player->shooting_angle) : -AIMING_DISTANCE * sin(DEG_TO_RAD * player->shooting_angle));
}

// Description: updates angle of red dot
void updateAimingIndicator(Player player) {
    updateAimingIndicatorTest(player);
    // return;
    // if (player->holding_shoot) {
    //     updateAimingIndicatorTest(player);
    //     return;

    //     double centre_x = player->coord.x + (player->facing_direction ? player->width : -(player->width)) ;
    //     double centre_y = player->coord.y + player->height / 2 + 1;
    //     double angle_radians = player->shooting_angle * M_PI / 180;
    //     // player->aiming_direction.x = centre_x + AIMING_DISTANCE * (player->facing_direction ? cos(DEG_TO_RAD * -player->shooting_angle) : - cos(DEG_TO_RAD * player->shooting_angle));
    //     // player->aiming_direction.y = centre_y + AIMING_DISTANCE * (player->facing_direction ? sin(DEG_TO_RAD * -player->shooting_angle) : -sin(DEG_TO_RAD * player->shooting_angle));

    //     if (player->facing_direction == RIGHT) {
    //         angle_radians = M_PI - angle_radians;
    //     }

    //     player->aiming_direction.x = centre_x - cos(angle_radians) * AIMING_DISTANCE;
    //     player->aiming_direction.y = centre_y - sin(angle_radians) * AIMING_DISTANCE;
    // } else {
    //     player->aiming_direction.x = 0;
    //     player->aiming_direction.y = 0;
    // }
}

void handlePlayerInput(Player player, const Uint8 *keys,
                       SDL_Scancode left, SDL_Scancode right,
                       SDL_Scancode jump, SDL_Scancode crouch,
                       SDL_Scancode shoot, SDL_Scancode grenade) {
    // printf("flag = %s\n", player->is_crouching? "true" : "false");
    if (player->healthBar->curPoints <= 0) {
        player->cur_vel.x = 0;
        return;
    }

    if (keys[left]) {
        player->is_moving = true;
        // running_effect();
        player->keyXTag = KEY_LEFT;
        player->facing_direction = LEFT;
        if (player->cur_vel.x >= -NO_POWER_MAX_X_VEL) {
                player->cur_vel.x -= HORIZONTAL_ACCELERATION * delta_time * (player->cur_vel.x > 0 ? 3.0 : 1.0);
            }    
    } else if (keys[right]) {
        player->is_moving = true;
        // running_effect();
        player->keyXTag = KEY_RIGHT;
        player->facing_direction = RIGHT;
        if (player->cur_vel.x <= NO_POWER_MAX_X_VEL) {
                player->cur_vel.x += HORIZONTAL_ACCELERATION * delta_time * (player->cur_vel.x < 0 ? 3.0 : 1.0);
            }
    } else {
        player->keyXTag = KEY_NONE_X;
        if (player->cur_vel.x > 0.0) {
            player->cur_vel.x *= pow(FRICTION_DECELERATION, delta_time);
            if (player->cur_vel.x <= MINIMUM_MOVING_VELOCITY) {
                player->cur_vel.x = 0;
            }
        } 
        if (player->cur_vel.x < 0.0) {
            player->cur_vel.x *= pow(FRICTION_DECELERATION, delta_time);
            if (player->cur_vel.x >= -MINIMUM_MOVING_VELOCITY) {
                player->cur_vel.x = 0;
            }
        } 
        player->is_moving = false;
    }

    if (keys[shoot]) {
        player->holding_shoot = true;
        if (keys[jump]) {
            player->shooting_angle += TOTAL_ANGLE * delta_time;
            if (player->shooting_angle >= MAXIMUM_SHOOTING_ANGLE) {
                player->shooting_angle = MAXIMUM_SHOOTING_ANGLE;
            }
        }
        if (keys[crouch]) {
            player->shooting_angle -= TOTAL_ANGLE * delta_time;
            if (player->shooting_angle <= MINIMUM_SHOOTING_ANGLE) {
                player->shooting_angle = MINIMUM_SHOOTING_ANGLE;
            }
        }
        updateAimingIndicator(player);
        return;
    } else if (player->holding_shoot) {
        shootBullet(player);
        player->holding_shoot  = false;
    }
    shootBulletEnd(player); // shoot any pending bullets regardless of the input
        
    // if (keys[jump] && player->canJump && player->cur_vel.y > -JUMP_VELOCITY) {
    //     player->cur_vel.y = -JUMP_VELOCITY;
    //     player->on_ground = false;
    // }

    if(keys[jump]) {
        player->keyYTag = KEY_UP;
    } else if(keys[crouch]) {
        player->keyYTag = KEY_DOWN;
    } else {
        player->keyYTag = KEY_NONE_Y;
    }

    if (keys[jump] && player->on_ground) {
        player->cur_vel.y = -JUMP_VELOCITY;
        player->on_ground = false;
        jump_effect();
    }

    if ((keys[jump] || keys[crouch] || keys[left] || keys[right]) && player->onLadder) {
        changeLadderAnimation();
    }

    if (keys[crouch] && !(player->is_crouching) && player->on_ground) {
        player->keyYTag = KEY_DOWN;
        player->is_crouching = true;
        player->coord.y     += 40;
    } else if (player->is_crouching && !keys[crouch]) {
        player->is_crouching = false;
        player->coord.y     -= 40;
    }

    if (keys[crouch] && keys[grenade]) {
        int num_of_base_objects = getLengthDynList(curLevel->objects);
        for (int i = num_of_base_objects - 1; i >= 0; i--) {
            GameObject obj = getElem(curLevel->objects, i); 
            if (strcmp(obj->tag, "gun") == 0 || strcmp(obj->tag, "power") == 0) {
                if (player->coord.x < obj->coord.x + obj->width &&
                player->coord.x + player->width > obj->coord.x &&
                player->coord.y < obj->coord.y + obj->height &&
                player->coord.y + player->height > obj->coord.y) {
                    removeAtIndexDynList(curLevel->objects, i);
                    player->weapon->gun = ((GunStats*)obj->extraData)->type;
                    fetch_stats(player->weapon);
                    // player->weapon->ammo = obj->extraData->
                }
                gun_pickup_effect();
            }
        }
    }

    player->height = player->is_crouching ? 60 : 100;
    
}


// Description: handles input for each player, passing to handlePlayerInput
void handle_input(Player player1, Player player2) {
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    handlePlayerInput(player1, keys,
                      SDL_SCANCODE_A, SDL_SCANCODE_D,
                      SDL_SCANCODE_W, SDL_SCANCODE_S,
                      SDL_SCANCODE_1, SDL_SCANCODE_2);

    handlePlayerInput(player2, keys,
                      SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
                      SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,
                      SDL_SCANCODE_N, SDL_SCANCODE_M);
}


//Function that makes a void funct that takes in a player apply for both players
void forEachPlayer(void (*funct) (Player player)) {
    Player p1 = game_settings->player1;
    Player p2 = game_settings->player2;
    funct(p1);
    funct(p2);
}

void setMovementFlagsFromVel(Player player, double xVel, double yVel) {
    if(yVel > 0) {
        player->yTag = MOVING_DOWN;
    } else if (yVel < 0) {
        player->yTag = MOVING_UP;
    } else {
       player->yTag = STILL_Y; 
    }

    if(xVel > 0) {
        player->xTag = MOVING_RIGHT;
    } else if (xVel < 0) {
        player->xTag = MOVING_LEFT;
    } else {
       player->xTag = STILL_X; 
    }
}