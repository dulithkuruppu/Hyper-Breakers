#include <assert.h>
#include <stddef.h>
#include "globalConsts.h"
#include "render.h"
#include "physics.h"
#include "sprites.h"
#include "level.h"

// void freePlatform(void *p) {
//     Platform *platform = p;
// }

// Description: Frees game object with no extraData field or with one without needing recusive freeing
void freeStandardObject(GameObject obj) {
    if(obj->extraData != NULL) free(obj->extraData);
    free(obj);
}

// Description: creates an empty game object with generic free funct (will do nothing if created)
GameObject createEmptyGameObject() {
    GameObject obj = malloc(sizeof(struct GameObject));
    assert(obj != NULL);
    *obj = (struct GameObject) {"",(Vector) {0,0}, NULL, NULL, NULL, 0, 0, NULL, NULL, NULL, freeStandardObject, NULL, NULL, NULL};
    return obj;
}
// Description: adds vertical platform to level
void addVerticalPlatform(Level l, int x, int y, int width, int height) {
    GameObject obj = createEmptyGameObject();

    strncpy(obj->tag, "vert_plat", sizeof(obj->tag));
    
    obj->coord = (Vector) {x,y};
    obj->width = width;
    obj->height = height;
    obj->colCheck = checkColPlatform;
    obj->onColX = onPlatformCollisionX;
    obj->onColY = onPlatformCollisionY;
    obj->onRender = renderPlatformImage;



    insertDynList(l->objects,obj);
}

// Description: adds vertical platform to level that can be rendered properly with gameassets
void addRenderableVerticalPlatform(Level l, int x, int y, int tileWidth, int tileHeight) {
    addVerticalPlatform(l,x,y,tileWidth*OBJ_SPRITE_WIDTH*OBJ_SCALE,tileHeight*OBJ_SPRITE_HEIGHT*OBJ_SCALE);
}

// Description: adds platform to level
void addPlatform(Level l, int x, int y, int width, int height) {
    GameObject obj = createEmptyGameObject();

    strncpy(obj->tag, "platform", sizeof(obj->tag));
    
    obj->coord = (Vector) {x,y};
    obj->width = width;
    obj->height = height;
    obj->colCheck = checkColPlatform;
    obj->onColX = onPlatformCollisionX;
    obj->onColY = onPlatformCollisionY;
    obj->onRender = renderPlatformImage;



    insertDynList(l->objects,obj);
}

// Description: adds renderable version platform to level
void addRenderablePlatform(Level l, int x, int y, int tileWidth, int tileHeight) {
    addPlatform(l,x,y,tileWidth*OBJ_SPRITE_WIDTH*OBJ_SCALE,tileHeight*OBJ_SPRITE_HEIGHT*OBJ_SCALE);
}

// Description: adds vertical platform to level
void addJumpPad(Level l, int x, int y, int width, int height) {
    GameObject obj = createEmptyGameObject();

    strncpy(obj->tag, "jump_pad", sizeof(obj->tag));
    
    obj->coord = (Vector) {x,y};
    obj->width = width;
    obj->height = height;
    obj->colCheck = checkColPlatform;
    obj->onColY = onJumpPadCollisionY;
    obj->onRender = renderJumpPad;

    obj->extraData = malloc(sizeof(struct JumpPadData));
    assert(obj->extraData != NULL);
    ((JumpPadData)obj->extraData)->bounceVel = JUMP_PAD_BOUNCE_CONST;



    insertDynList(l->objects,obj);
}

// Description: adds sideways jump pad to level
void addSidewaysJumpPad(Level l, int x, int y, int width, int height) {
    GameObject obj = createEmptyGameObject();

    strncpy(obj->tag, "jump_pad", sizeof(obj->tag));
    
    obj->coord = (Vector) {x,y};
    obj->width = width;
    obj->height = height;
    obj->colCheck = checkColPlatform;
    obj->onColX = onSidewaysJumpPadCollisionX;
    obj->onColY = onPlatformCollisionY;
    obj->onRender = renderJumpPad;

    obj->extraData = malloc(sizeof(struct JumpPadData));
    assert(obj->extraData != NULL);
    *(JumpPadData)(obj->extraData) = (struct JumpPadData) {
        .bounceVel = SIDEWAYS_JUMP_PAD_BOUNCE_CONST
    };



    insertDynList(l->objects,obj);
}

// Description: adds moving platform to level
void addMovingPlatform(Level l, double startX, double startY, int width, int height, double endX, double endY, double timeToReachEnd) {
    GameObject obj = createEmptyGameObject();

    strncpy(obj->tag, "mov_plat", sizeof(obj->tag));
    
    obj->coord = (Vector) {startX,startY};
    obj->width = width;
    obj->height = height;
    obj->colCheck = checkColPlatform;
    obj->onFrameStart = onMovingPlatformFrameStart;
    obj->onColX = onPlatformCollisionX;
    obj->onColY = onMovingPlatformCollisionY;
    obj->onRender = renderMovablePlatform;

    obj->extraData = malloc(sizeof(struct MovingPlatformData));
    assert(obj->extraData != NULL);
    *(MovingPlatformData)(obj->extraData) = (struct MovingPlatformData) {
        .startPos = (Vector) {startX, startY},
        .endPos = (Vector) {endX, endY},
        .vel = {(endX-startX)/timeToReachEnd,(endY-startY)/timeToReachEnd},
        .goingToEnd = true,
        .timeToReachEnd = timeToReachEnd,
        .curDirectionMoveTime = 0.0
    };

    insertDynList(l->objects,obj);
}

// Description: frees passable platform and its created dynamic list
void freePassablePlatform(GameObject obj) {
    freeDynList(((PassablePlatformData)obj->extraData)->playerCanPassThrough);
    free(obj);
}

void addPassablePlatform(Level l, int x, int y, int width, int height) {
    GameObject obj = createEmptyGameObject();

    strncpy(obj->tag, "pass_plat", sizeof(obj->tag));
    
    obj->coord = (Vector) {x,y};
    obj->width = width;
    obj->height = height;
    obj->colCheck = checkColPlatform;
    obj->onColX = onPassablePlatformCollisionX;
    obj->onColY = onPassablePlatformCollisionY;
    obj->onFrameEnd = onPassablePlatformFrameEnd;
    obj->onRender = renderPassablePlatformImage;
    obj->onFree = freePassablePlatform;
    obj->extraData = malloc(sizeof(struct JumpPadData));
    assert(obj->extraData != NULL);
    ((PassablePlatformData)obj->extraData)->playerCanPassThrough = createDynList(NULL,NULL);

    insertDynList(l->objects,obj);
}

void addRenderablePassablePlatform(Level l, int x, int y, int tileWidth) {
    addPassablePlatform(l, x, y, tileWidth *OBJ_SPRITE_WIDTH*PASSABLE_PLAT_SCALE, OBJ_SPRITE_HEIGHT * PASSABLE_PLAT_SCALE / 4);
 }

void addHazard(Level l, int x, int y, int width, int height) {
    GameObject obj = createEmptyGameObject();

    strncpy(obj->tag, "hazard", sizeof(obj->tag));
    
    obj->coord = (Vector) {x,y};
    obj->width = width;
    obj->height = height;
    obj->colCheck = checkColPlatform;
    obj->onColX = onHazardCollisionX;
    obj->onColY = onHazardCollisionY;
    obj->onRender = renderHazardImage;

    obj->extraData = malloc(sizeof(struct HazardData));
    assert(obj->extraData != NULL);
    ((HazardData)obj->extraData)->damage = HAZARD_DAMAGE;
    ((HazardData)obj->extraData)->knockback = HAZARD_KNOCKBACK;

    insertDynList(l->objects,obj);
}

void addRenderableHazard(Level l, int x, int y, int tileWidth) {
    addHazard(l,x,y,tileWidth*OBJ_SPRITE_WIDTH*HAZARD_SCALE,4*HAZARD_SCALE);
}

void addLadder(Level l, int x, int y, int width, int height) {
    GameObject obj = createEmptyGameObject();

    strncpy(obj->tag, "ladder", sizeof(obj->tag));
    
    obj->coord = (Vector) {x,y};
    obj->width = width;
    obj->height = height;
    obj->colCheck = checkColPlatform;
    obj->onFrameEnd = onLadderFrameEnd;
    obj->onFrameStart = onLadderFrameStart;
    obj->onRender = renderLadderImage;

    obj->extraData = malloc(sizeof(struct LadderData));
    assert(obj->extraData != NULL);
    ((LadderData)obj->extraData)->ladderMoveSpeed = LADDER_MOVE_SPEED;
    ((LadderData)obj->extraData)->ladderDropSpeed = LADDER_DROP_SPEED;
    ((LadderData)obj->extraData)->ladderClimbSpeed = LADDER_CLIMB_SPEED;

    insertDynList(l->objects,obj);
}

void addRenderableLadder(Level l, int x, int y, int tileHeight) {
    addLadder(l, x, y, OBJ_SPRITE_WIDTH * LADDER_SCALE, tileHeight * OBJ_SPRITE_HEIGHT * LADDER_SCALE);
}

//Level Creation

void freeObjectInList(void* obj) {
    GameObject o = obj;
    (o->onFree) (o);
}

void freeLevel(Level l) {
    freeDynList(l->objects);
    free(l);
}

// Description: creates empty level with no object in
Level createEmptyLevel() {
     Level l = malloc(sizeof(struct Level));
     assert(l != NULL);

     l->objects = createDynList(freeObjectInList,NULL);
     assert(l->objects != NULL);

     return l;
}

// Description: Creates first level
Level createLevelOne() {
    Level l = createEmptyLevel();
    static const int ground_height = 96;
    addRenderableVerticalPlatform(l,-64,-10,2,100);// THIS IS THE WALl
    addRenderableVerticalPlatform(l,WINDOW_WIDTH,-10,2,100);// THIS IS THE WALl
    addRenderableVerticalPlatform(l,0,WINDOW_HEIGHT-ground_height,40,3);// THIS IS THE FLOOR
    addRenderablePlatform(l,400,WINDOW_HEIGHT-ground_height-300,15,1);// THIS IS THE FLOOR

    // addVerticalPlatform(l,150,0,50,WINDOW_HEIGHT);
    // add the stationary platforms
    addRenderablePlatform(l,0,WINDOW_HEIGHT-200,10,2);
    addRenderablePassablePlatform(l,0,WINDOW_HEIGHT-600,32);
    addRenderablePlatform(l,WINDOW_WIDTH - 320,WINDOW_HEIGHT-200,10,2);
    // add hazards
    addRenderableHazard(l,0,WINDOW_HEIGHT-ground_height - 10,32);

    //add jump pads
    addJumpPad(l,400,WINDOW_HEIGHT-ground_height-30,80,20);
    addJumpPad(l,800,WINDOW_HEIGHT-ground_height-30,80,20);
    addJumpPad(l,100,WINDOW_HEIGHT-220,80,20);
    addJumpPad(l,1100,WINDOW_HEIGHT-220,80,20);
    addSidewaysJumpPad(l,WINDOW_WIDTH - 20,WINDOW_HEIGHT-ground_height-380,20,80);
    addSidewaysJumpPad(l,0,WINDOW_HEIGHT-ground_height-380,20,80);
    //addSidewaysJumpPad(l,650,WINDOW_HEIGHT-ground_height-300,20,80);
    addMovingPlatform(l,340,WINDOW_HEIGHT-200,100,20,WINDOW_WIDTH - 450,WINDOW_HEIGHT-200,4);
    // addRenderablePlatform(l,830,520,3,1);

    addRenderableLadder(l,WINDOW_WIDTH/2 - 10,100,10);
    return l;
}
