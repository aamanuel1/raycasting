#include "ray.h"
#include <limits.h>

ray_t rays[NUM_RAYS];

void normalizeAngle(float* angle){
	*angle = remainder(*angle, TWO_PI);
	if(*angle < 0){
		*angle = (TWO_PI + *angle);
	}
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2){
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool isRayFacingUp(float angle){
	return !isRayFacingDown(angle);
}

bool isRayFacingDown(float angle){
	return angle > 0 && angle < PI;
}

bool isRayFacingLeft(float angle){
	return !isRayFacingRight(angle);
}

bool isRayFacingRight(float angle){
	return angle > 1.5 * PI || angle < 0.5 * PI;
}


void castRay(float rayAngle, int stripId){
	normalizeAngle(&rayAngle);

	float xstep;
	float ystep;

	float xintercept;
	float yintercept;

	//HORIZONTAL RAY-GRID INTERSECTION CODE
	bool foundHorizWallHit = false;
	int horizWallHitX = 0;
	int horizWallHitY = 0;
	int horizWallContent = 0;

	yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	yintercept += (isRayFacingDown(rayAngle) ? TILE_SIZE : 0);	
	xintercept = player.x + (yintercept - player.y) / tan(rayAngle);

	ystep = TILE_SIZE;
	ystep *= isRayFacingUp(rayAngle) ? -1: 1;

	xstep = TILE_SIZE / tan(rayAngle);
	xstep *= (isRayFacingLeft(rayAngle) && xstep > 0) ? -1 : 1;
	xstep *= (isRayFacingRight(rayAngle) && xstep < 0) ? -1 : 1;

	float nextHorizTouchX = xintercept;
	float nextHorizTouchY = yintercept;

	while(isInsideMap(nextHorizTouchX, nextHorizTouchY)){
	// while(nextHorizTouchX >= 0 && nextHorizTouchX <= MAP_NUM_COLS * TILE_SIZE && nextHorizTouchY >= 0 && nextHorizTouchY <= MAP_NUM_ROWS * TILE_SIZE){
		float xToCheck = nextHorizTouchX;
		float yToCheck = nextHorizTouchY + (isRayFacingUp(rayAngle) ? -1: 0);
		if(mapHasWallAt(xToCheck, yToCheck)){
			foundHorizWallHit = true;
			horizWallHitX = nextHorizTouchX;
			horizWallHitY = nextHorizTouchY;
			horizWallContent = getMapAt((int)floor(yToCheck/TILE_SIZE), (int)floor(xToCheck/TILE_SIZE));
			break;
		}
		else{
			nextHorizTouchX += xstep;
			nextHorizTouchY += ystep;
		}
	}
	
	//VERTICAL RAY-GRID INTERSECTION CODE
	bool foundVertWallHit = false;
	int vertWallHitX = 0;
	int vertWallHitY = 0;
	int vertWallContent = 0;

	xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
	xintercept += isRayFacingRight(rayAngle) ? TILE_SIZE : 0;

	yintercept = player.y + (xintercept - player.x) * tan(rayAngle);

	xstep = TILE_SIZE;
	xstep *= isRayFacingLeft(rayAngle) ? -1 : 1;

	ystep = TILE_SIZE * tan(rayAngle);
	ystep *= (isRayFacingUp(rayAngle) && ystep > 0) ? -1 : 1;
	ystep *= (isRayFacingDown(rayAngle) && ystep < 0) ? -1 : 1;

	float nextVertTouchX = xintercept;
	float nextVertTouchY = yintercept;

	while (isInsideMap(nextVertTouchX, nextVertTouchY)){
	// while(nextVertTouchX >= 0 && nextVertTouchX <= MAP_NUM_COLS * TILE_SIZE && nextVertTouchY >= 0 && nextVertTouchY <= MAP_NUM_ROWS * TILE_SIZE){
		float xToCheck = nextVertTouchX + (isRayFacingLeft(rayAngle) ? -1 : 0);
		float yToCheck = nextVertTouchY;
		if(mapHasWallAt(xToCheck, yToCheck)){
			foundVertWallHit = true;
			vertWallHitX = nextVertTouchX;
			vertWallHitY = nextVertTouchY;
			vertWallContent = getMapAt((int)floor(yToCheck/TILE_SIZE), (int)floor(xToCheck/TILE_SIZE));
			break;
		}
		else{
			nextVertTouchX += xstep;
			nextVertTouchY += ystep;
		}
	}

	float horizHitDistance = (foundHorizWallHit) 
	? distanceBetweenPoints(player.x, player.y, horizWallHitX, horizWallHitY) 
	: FLT_MAX; 

	float vertHitDistance = (foundVertWallHit)
	? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
	: FLT_MAX;

	if(vertHitDistance < horizHitDistance){
		rays[stripId].distance = vertHitDistance;
		rays[stripId].wallHitX = vertWallHitX;
		rays[stripId].wallHitY = vertWallHitY;
		rays[stripId].wasHitVertical = true;
		rays[stripId].wallHitContent = vertWallContent;
		rays[stripId].rayAngle = rayAngle;
	}
	else{
		rays[stripId].distance = horizHitDistance;
		rays[stripId].wallHitX = horizWallHitX;
		rays[stripId].wallHitY = horizWallHitY;
		rays[stripId].wasHitVertical = false;
		rays[stripId].wallHitContent = horizWallContent;
		rays[stripId].rayAngle = rayAngle;
	}
}

void castAllRays(){
	//Start first ray subtracting half of FOV
	float rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

	for(int col = 0; col < NUM_RAYS; col++){
		float rayAngle = player.rotationAngle + atan((col - NUM_RAYS / 2) / DIST_PROJ_PLANE);
		castRay(rayAngle, col);
		rayAngle += FOV_ANGLE / NUM_RAYS;
	}
}

void renderRays(){
	for(int i = 0; i < NUM_RAYS; i++){
		drawLine(
			MINIMAP_SCALE_FACTOR * player.x,
			MINIMAP_SCALE_FACTOR * player.y,
			MINIMAP_SCALE_FACTOR * rays[i].wallHitX,
			MINIMAP_SCALE_FACTOR * rays[i].wallHitY,
			0xFF0000FF
		);
	}
}