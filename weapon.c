#include "level.h"
#include "globalConsts.h"
#include "genericDynamicList.h"
#include "render.h"
#include "physics.h"
#include "weapon.h"
#include <assert.h>

Uint32 last_feat_spawn = 0;
// Uint32 last_feat_spawn_time = 0;

GunStats guns[NUMBER_OF_GUNS] = {
    { PISTOL, 500, 5, "Pistol", 12,1, 0, 1 },
    { RIFLE, 600, 10, "AR", 25 ,5, 0, 3},
    { SHOTGUN, 600, 5, "Shotgun", 20 ,10, 20, 1.5},
    { SNIPER, 900, 50, "Sniper Rifle", 2, 1, 0, 5 },
    { RPG, 500, 100, "RPG-7", 1, 1, 0, 5 }
};

PowerClass powers[NUMBER_OF_POWERS] = {RUN_FASTER, JUMP_HIGHER, INCREASE_HEALTH};

// Description: gets moving platform or regular platform
int get_valid_bases(GameObject* valid_base_objects) {
    int num_of_valid_bases = 0;
    int num_of_base_objects = getLengthDynList(curLevel->objects);
    
    for (int i = 0; i < num_of_base_objects; i++) {
        GameObject obj = getElem(curLevel->objects, i);
        if (strcmp(obj->tag, "platform") == 0 || strcmp(obj->tag, "mov_plat") == 0) {
            valid_base_objects[num_of_valid_bases++] = obj;
        }
    }
    return num_of_valid_bases;
}

// Description: calculates weapon damage based on gun
int weapon_damage(GunClass gun) {
    for (int i = 0; i < NUMBER_OF_GUNS; i++) {
        if (guns[i].type == gun) {
            return guns[i].dmg;
        }
    }
    return 0;
}

void fetch_stats(Weapon w){
    GunClass gunc = w -> gun;
    for (int i = 0; i < NUMBER_OF_GUNS; i++) {
        if (guns[i].type == gunc ) {
            w-> dmg =  guns[i].dmg;
            w ->magazineSize = guns[i].magazineSize;
            w ->burst = guns[i].burst;
            w ->spread = guns[i].spread;
            w -> bulletSpeed = guns[i].bulletSpeed;
            w ->reload_time = guns[i].reload_time;
            w -> currentAmmo = guns[i].magazineSize;
            w-> totalAmmo =  (w -> currentAmmo)*3;
            w ->reload_start = 0.0;
            w ->reloaded = true;
        }
    }
}


void add_gun_object(GameObject surface, int x, int y, GunClass gun) {
    GameObject obj = createEmptyGameObject();
    strcpy(obj->tag, "gun");
    obj->coord = (Vector){x, y};
    obj->width = 60;
    obj->height = 18;
    obj->colCheck = checkColItem;
    // obj->onColX = onPlatformCollisionX;
    obj->extraData = malloc(sizeof(GunType));
    assert(obj->extraData != NULL);
    if (strcmp(surface->tag, "mov_plat") == 0) {
        // obj->onColY = onPlatformCollisionY;
        obj->onFrameStart = onMovingPlatformGunFrameStart;
        ((GunType *)obj->extraData)->moving_plat = surface;
    } else {
        // obj->onColY = onPlatformCollisionY;
        ((GunType *)obj->extraData)->moving_plat = NULL;
    }
    obj->onRender = renderGun;
    ((GunType *)obj->extraData)->gun = gun;
    insertDynList(curLevel->objects, obj);
}


void add_power_object(GameObject surface, int x, int y, PowerClass power) {
    GameObject obj = createEmptyGameObject();
    strcpy(obj->tag, "power");
    obj->coord = (Vector){x, y};
    obj->width = 30;
    obj->height = 30;
    // obj->colCheck = checkColItem;
    // obj->onColX = onPlatformCollisionX;
    obj->extraData = malloc(sizeof(PowerType));
    assert(obj->extraData != NULL);
    if (strcmp(surface->tag, "mov_plat") == 0) {
        // obj->onColY = onPlatformCollisionY;
        obj->onFrameStart = onMovingPlatformPowerFrameStart;
        ((PowerType *)obj->extraData)->moving_plat = surface;
    } else {
        // obj->onColY = onPlatformCollisionY;
        ((PowerType *)obj->extraData)->moving_plat = NULL;
    }
    obj->onRender = renderPower;
    ((PowerType *)obj->extraData)->power = power;
    insertDynList(curLevel->objects, obj);
}


void remove_all_guns_and_powers() {
    int num_of_base_objects = getLengthDynList(curLevel->objects);
    for (int i = num_of_base_objects - 1; i >= 0; i--) {
        GameObject obj = getElem(curLevel->objects, i); 
        if (strcmp(obj->tag, "gun") == 0 || strcmp(obj->tag, "power") == 0 ) {
            obj->onFree(obj);
            removeAtIndexDynList(curLevel->objects, i);
        }
    }
}

// add_power_object(curLevel, x, y, power);


bool check_collision_object(int x, int y, int width, int height) {
    int num_of_objects = getLengthDynList(curLevel->objects);
    
    for (int i = 0; i < num_of_objects; i++) {
        GameObject obj = getElem(curLevel->objects, i);

        if (strcmp(obj->tag, "vert_plat") == 0 || strcmp(obj->tag, "jump_pad") == 0) {
            if (x < obj->coord.x + obj->width &&
                x + width > obj->coord.x &&
                y < obj->coord.y + obj->height &&
                y + height > obj->coord.y) {
                return 1; 
            }
        }
    }
    return 0; 
}

bool check_collision_player(int x, int y, int width, int height) {
    int num_of_objects = getLengthDynList(curLevel->objects);
    
    for (int i = 0; i < num_of_objects; i++) {
        GameObject obj = getElem(curLevel->objects, i);

        if (strcmp(obj->tag, "vert_plat") == 0 || strcmp(obj->tag, "platform") == 0 || strcmp(obj->tag, "jump_pad") == 0 || strcmp(obj->tag, "Hazard") == 0) {
            if (checkColRects(x,y,width,height,obj->coord.x, obj->coord.y,obj->width,obj->height)) {
                return 1; 
            }
        }
    }
    return 0; 
}

bool try_spawn_on_surface(GameObject surface, int width, int height, int* final_x, int* final_y, bool obj_or_player) {
    // 0 - obj, 1 - player
    while(true) {
        // if (x + 30 > surface->width) {
        //     x-=30;
        // }
        // if (x - 30 < surface->width) {
        //     x+=30;
        // }
        if (!obj_or_player) {  // object
            int x = surface->coord.x + rand() % (surface->width) - width/2;
            int y = surface->coord.y - height;
            if (!check_collision_object(x, y, width, height)) {
                *final_x = x;
                *final_y = y;
                // printf("%s, %f\n",surface->tag,surface->coord.y);
                return 1;
            }
        } else {  // player
            // printf("%s\n",surface->tag);
            // printf("w: %d, h: %d\n",width,height);
            int x = surface->coord.x + rand() % (surface->width) - width/2; //ADD width & height of player logic to here? didnt work
            int y = surface->coord.y - height;
            if (strcmp(surface->tag, "mov_plat") == 0) {
                // MovingPlatformData m = (MovingPlatformData)surface->extraData;
                // y-=60;
                // x += ((MovingPlatformData)surface->extraData)->vel.x * 0.1;
            }
            if (!check_collision_player(x, y, width, height)) {
                *final_x = x;
                *final_y = y;
                return 1;
            }
        }
    }
    return 0;
}

void spawn_random_feature() {
    // GameObject valid_base_objects[10];
    // int num_of_valid_bases = 0;
    // int num_of_base_objects = getLengthDynList(curLevel->objects);
    // for (int i = 0; i < num_of_base_objects; i++) {
    //     GameObject obj = getElem(curLevel->objects, i); 
    //     if (strcmp(obj->tag, "platform") == 0 || strcmp(obj->tag, "mov_plat") == 0) {
    //         valid_base_objects[num_of_valid_bases++] = obj;
    //     }
    // }
    GameObject valid_base_objects[10];
    int num_of_valid_bases = get_valid_bases(valid_base_objects);


    if (num_of_valid_bases == 0) return;

    for (int i = 0; i < MAX_WEAPONS_PER_INSTANCE; i++) {
        GameObject surface = valid_base_objects[rand() % num_of_valid_bases];
        // int x = surface->coord.x + (rand() % surface->width) - 10;
        // int y = surface->coord.y - 15;
        // GunClass gun = guns[rand() % NUMBER_OF_GUNS].type;
        // add_gun_object(surface, x, y, gun); 
   
        int x, y;
        if (try_spawn_on_surface(surface, MAX_GUN_WIDTH, MAX_GUN_HEIGHT, &x, &y, 0)) {
            add_gun_object(surface, x, y, guns[rand() % NUMBER_OF_GUNS].type);
        }
    }

    for (int i = 0; i < MAX_POWERS_PER_INSTANCE; i++) {
        GameObject surface = valid_base_objects[rand() % num_of_valid_bases];
        // int x = surface->coord.x + (rand() % surface->width ) - 10;
        // int y = surface->coord.y - 30;
        // PowerClass power = powers[rand() % NUMBER_OF_POWERS];
        // add_power_object(surface, x, y, power);

        int x, y;
        if (try_spawn_on_surface(surface, MAX_POWER_WIDTH, MAX_POWER_HEIGHT, &x, &y, 0)) {
            add_power_object(surface, x, y, powers[rand() % NUMBER_OF_POWERS]);
        }
    }
}


void maybe_respawn_feats() {
    Uint32 now = SDL_GetTicks();
    if (now - last_feat_spawn >= FEAT_SPAWN_INTERVAL) {
        remove_all_guns_and_powers();
        spawn_random_feature();       
        last_feat_spawn = now;   
    }
}


