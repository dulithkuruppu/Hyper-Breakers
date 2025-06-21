#ifndef WEAPON_H
#define WEAPON_H

#include "vector.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#define NUMBER_OF_GUNS 5
#define NUMBER_OF_POWERS 3
#define FEAT_SPAWN_INTERVAL 10000
#define SPAWN_ITEM_MARGIN 20
#define MAX_SPAWN_ATTEMPTS 50
#define MAX_GUN_WIDTH 60
#define MAX_GUN_HEIGHT 18
#define MAX_POWER_WIDTH 30
#define MAX_POWER_HEIGHT 30

struct Player;
typedef struct Player* Player;

struct GameObject;
typedef struct GameObject* GameObject;

// Enum Power
typedef enum {
    RUN_FASTER,
    INCREASE_HEALTH,
    JUMP_HIGHER,
} PowerClass;

// Enum Gun
typedef enum {
    PISTOL,
    RIFLE,
    SHOTGUN,
    SNIPER,
    RPG,
} GunClass;

// GunStats Structure
typedef struct {
    GunClass type;
    double bulletSpeed;
    int dmg;
    char name[32];
    int magazineSize;
    int burst;
    int spread;
    double reload_time;
} GunStats ;

extern GunStats guns[NUMBER_OF_GUNS];

typedef struct {
    GunClass source_weapon;
    Player owner;
    Vector vel;
} BulletData;

// Weapon Structure
struct Weapon {
    GunClass gun;
    int totalAmmo;
    int dmg;
    int magazineSize;
    int currentAmmo;
    int burst;
    int spread;
    double bulletSpeed;
    double reload_time;
    double reload_start;
    double reload_end;
    bool reloaded;
};

typedef struct {
    GunClass gun;
    GameObject moving_plat;
} GunType;

typedef struct {
    PowerClass power;
    GameObject moving_plat;
} PowerType;

typedef struct Weapon *Weapon;
extern int weapon_damage(GunClass gun);
extern void spawn_random_feature();
extern void maybe_respawn_feats();
extern void remove_all_guns_and_powers();
extern int get_valid_bases(GameObject* valid_base_objects) ;
extern bool check_collision_object(int x, int y, int width, int height);
extern bool check_collision_player(int x, int y, int width, int height);
extern bool try_spawn_on_surface(GameObject surface, int width, int height, int* final_x, int* final_y, bool obj_or_player);
extern void fetch_stats(Weapon w);

#endif