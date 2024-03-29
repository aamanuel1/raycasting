#ifndef PLAYER_H
#define PLAYER_H

#include "defs.h"
#include "graphics.h"
#include "map.h"

typedef struct{
	float x;
	float y;
	float width;
	float height;
	int turnDirection; //-1 is left, +1 is right
	int walkDirection; //-1 for back, +1 for front
	float rotationAngle;
	float walkSpeed;
	float turnSpeed;
} player_t;

extern player_t player;

void movePlayer(float deltaTime);
void renderPlayer(void);

#endif