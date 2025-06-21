#ifndef PHYSICS_H
#include "level.h"
#include "player.h"
#define PHYSICS_H

extern bool checkColRects(double x1, double y1, double w1, double h1, double x2, double y2, double w2, double h2);

extern bool checkColPlatform (GameObject obj, Player player);
extern bool checkColItem (GameObject obj, Player player);

extern bool checkPointInRect(double px, double py, double rx, double ry, double rw, double rh);

extern bool checkColBullet (GameObject obj, Player player);

extern bool checkColObjects (GameObject obj, GameObject obj2);

extern void GameObjectOnColCheck (GameObject obj, GameObject obj2);

extern void onPlatformCollisionX (GameObject obj, Player player);

extern void onPlatformCollisionY (GameObject obj, Player player);

extern void onJumpPadCollisionY (GameObject obj, Player player);

extern void onSidewaysJumpPadCollisionX (GameObject obj, Player player);

extern void onMovingPlatformFrameStart(GameObject obj);

extern void onMovingPlatformCollisionY (GameObject obj,Player player);

extern void moveGunWithPlatform(GameObject obj, GameObject platform);

extern void movePowerWithPlatform(GameObject obj, GameObject platform);

extern void onMovingPlatformGunFrameStart(GameObject obj);

extern void onMovingPlatformPowerFrameStart(GameObject obj);
extern void onPassablePlatformCollisionX (GameObject obj,Player player);

extern void onPassablePlatformCollisionY (GameObject obj,Player player);

extern void onPassablePlatformFrameEnd (GameObject obj);

extern void onHazardCollisionX (GameObject obj,Player player);
extern void onHazardCollisionY (GameObject obj,Player player);

extern void onLadderFrameEnd (GameObject obj);
extern void onLadderFrameStart (GameObject obj);
#endif