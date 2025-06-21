#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>
#include "vector.h"
#include "genericDynamicList.h"
#include "player.h"


typedef struct JumpPadData {
    double bounceVel;
} *JumpPadData;

typedef struct PassablePlatformData {
    dynList playerCanPassThrough;
} *PassablePlatformData;

typedef struct MovingPlatformData {
    Vector startPos;
    Vector endPos;
    Vector vel;
    double timeToReachEnd;
    double curDirectionMoveTime;
    bool goingToEnd;
} *MovingPlatformData;

typedef struct HazardData {
    int damage;
    double knockback;
} *HazardData;

typedef struct LadderData {
    double ladderMoveSpeed;
    double ladderDropSpeed;
    double ladderClimbSpeed;
} *LadderData;

typedef struct GameObject *GameObject;


typedef bool(*colCheckFunct) (GameObject obj, Player player);
typedef void (*onCollisionFunct) (GameObject obj,Player player);
typedef void (*onFrameFunct) (GameObject obj);
typedef void (*onFreeFunct) (GameObject obj);
typedef void (*onRenderFunct) (GameObject obj);
typedef bool (*GameObjectColCheckFunct) (GameObject obj1, GameObject obj2);
typedef void (*GameObjectOnColFunct) (GameObject obj1, GameObject obj2);

struct GameObject {
    char tag[10];
    Vector coord;
    colCheckFunct colCheck;
    onCollisionFunct onColX;
    onCollisionFunct onColY;
    int width, height;
    onFrameFunct onFrameStart;
    onFrameFunct onFrameEnd;
    onRenderFunct onRender;
    onFreeFunct onFree; 
    GameObjectColCheckFunct GameObjectColCheck;
    GameObjectOnColFunct GameObjectOnColCheck;
    void *extraData;
    
};

typedef struct Level {
    dynList objects;
} *Level;

extern void freeObjectInList(void* obj);
extern GameObject createEmptyGameObject();
extern Level createLevelOne();
extern void freeLevel(Level l);

#endif