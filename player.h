#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "gameLoop.h"
#include "weapon.h"
#include "vector.h"
#include "genericDynamicList.h"
#include "bar.h"

#define NO_POWER_MAX_X_VEL 200.0
#define NO_POWER_MAX_Y_VEL 450.0
#define WITH_POWER_MAX_X_VEL 7.5
#define WITH_POWER_MAX_Y_VEL 7.5
#define HORIZONTAL_ACCELERATION 100
#define RELOAD_BUFFER_TIME 20 

typedef struct Bar *Bar;

// Powers Structure
struct Powers {
    bool run_faster;
    bool increase_health;
    bool jump_higher;
};

typedef struct Powers *Powers;

typedef enum MoveXTag {
    MOVING_LEFT, MOVING_RIGHT, STILL_X
} MoveXTag;

typedef enum MoveYTag {
    MOVING_UP, MOVING_DOWN, STILL_Y
} MoveYTag;

typedef enum KeyPressXTag {
    KEY_LEFT, KEY_RIGHT, KEY_NONE_X
} KeyPressXTag;

typedef enum KeyPressYTag {
    KEY_UP, KEY_DOWN, KEY_NONE_Y
} KeyPressYTag;


// Player Structure
struct Player {
    int id;
    const char* character;
    Coordinate coord;
    int width;
    int height;
    int health;
    int speed;
    Bar healthBar;
    Vector cur_vel;
    Weapon weapon;
    struct Powers powers; 
    bool is_shooting;
    bool facing_direction;
    bool on_ground;
    bool holding_shoot;
    bool is_moving;
    bool is_crouching;
    double shooting_angle;
    bool goingRight;
    bool goingDown;
    bool onLadder;
    dynList bullets;
    Coordinate aiming_direction;
    bool canJump;
    MoveXTag xTag;
    MoveYTag yTag;
    KeyPressXTag keyXTag;
    KeyPressYTag keyYTag;
    int noKills;
    double last_shot;
    int bullets_to_Shoot;
    bool isHit;
};

typedef struct Player* Player;

extern int getNextID(void);
extern Player initPlayers(const char* charachter);
extern void handleInput(Player player, SDL_Scancode left, SDL_Scancode right, SDL_Scancode shoot);
// extern void takeDamage(Player player, Bullet bullet);
// extern void died(Player player, Bullet bullet);
extern void setMovementFlagsFromVel(Player player, double xVel, double yVel);
extern void shootBullet(Player player);
extern void invalidateBullet(GameObject bullet);
extern void removeBullet(GameObject bullet);
extern void updateAimingIndicator(Player player);
extern void handle_input(Player player1, Player player2);
extern void handlePlayerInput(Player player, const Uint8 *keys,
                       SDL_Scancode left, SDL_Scancode right,
                       SDL_Scancode jump, SDL_Scancode crouch,
                       SDL_Scancode shoot, SDL_Scancode grenade);
extern void forEachPlayer(void (*funct) (Player player));
extern void takeDamage(Player player, int damage);

#endif