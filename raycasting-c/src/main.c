#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <limits.h>
#include <stdbool.h>
#include "defs.h"
#include "textures.h"
#include "graphics.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5}
};


bool mapHasWallAt(float x, float y){
	if(x < 0 || x >=  MAP_NUM_COLS * TILE_SIZE || y < 0 || y >= MAP_NUM_ROWS * TILE_SIZE){
		return true;
	}
	int mapX = floor(x / TILE_SIZE);
	int mapY = floor(y / TILE_SIZE);

	if(map[mapY][mapX] == 0)
		return false;
	
	return true;
}

struct Player{
	float x;
	float y;
	float width;
	float height;
	int turnDirection; //-1 is left, +1 is right
	int walkDirection; //-1 for back, +1 for front
	float rotationAngle;
	float walkSpeed;
	float turnSpeed;
} player;

struct Ray{
	float rayAngle;
	float wallHitX;
	float wallHitY;
	float distance;
	bool wasHitVertical;
	int wallHitContent;
} rays[NUM_RAYS];


bool isGameRunning = false;
int ticksLastFrame = 0;

void setup(){
	//initialize and setup game objects
	player.x = WINDOW_WIDTH / 2;
	player.y = WINDOW_HEIGHT / 2;
	player.width = 5;
	player.height = 5;
	player.turnDirection = 0;
	player.walkDirection = 0;
	player.rotationAngle = PI / 2;
	player.walkSpeed = 100;
	player.turnSpeed = 45 * (PI / 180);

	//see textures.h and textures.c
	//asks uPNG to decode all PNG files and loads wallTextures array
	loadWallTextures();
}

void movePlayer(float deltaTime){
	player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;

	float moveStep = player.walkDirection * player.walkSpeed * deltaTime;
	
	float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
	float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

	//perform wall collision.
	if(!mapHasWallAt(newPlayerX, newPlayerY)){
		player.x = newPlayerX;
		player.y = newPlayerY;
	}
}

void renderPlayer(){
	// SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	// SDL_Rect playerRect = {
	// 	player.x * MINIMAP_SCALE_FACTOR,
	// 	player.y * MINIMAP_SCALE_FACTOR,
	// 	player.width * MINIMAP_SCALE_FACTOR,
	// 	player.height * MINIMAP_SCALE_FACTOR
	// };
	// SDL_RenderFillRect(renderer, &playerRect);

	// //Draw the mid line
	// SDL_RenderDrawLine(
	// 	renderer, 
	// 	MINIMAP_SCALE_FACTOR * player.x,
	// 	MINIMAP_SCALE_FACTOR * player.y,
	// 	MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 40,
	// 	MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 40
	// );
}

float normalizeAngle(float angle){
	angle = remainder(angle, TWO_PI);
	if(angle < 0){
		angle = (TWO_PI + angle);
	}
	return angle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2){
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void castRay(float rayAngle, int stripId){
	rayAngle = normalizeAngle(rayAngle);

	//up is north down is right.
	int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
	int isRayFacingUp = !isRayFacingDown;
	
	//right is east left is west.
	int isRayFacingRight = rayAngle > 1.5 * PI || rayAngle < 0.5 * PI;
	int isRayFacingLeft = !isRayFacingRight;

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
	yintercept += isRayFacingDown ? TILE_SIZE : 0;
	
	xintercept = player.x + (yintercept - player.y) / tan(rayAngle);

	ystep = TILE_SIZE;
	ystep *= isRayFacingUp ? -1: 1;

	xstep = TILE_SIZE / tan(rayAngle);
	xstep *= (isRayFacingLeft && xstep > 0) ? -1 : 1;
	xstep *= (isRayFacingRight && xstep < 0) ? -1 : 1;

	float nextHorizTouchX = xintercept;
	float nextHorizTouchY = yintercept;

	while(nextHorizTouchX >= 0 && nextHorizTouchX <= MAP_NUM_COLS * TILE_SIZE && nextHorizTouchY >= 0 && nextHorizTouchY <= MAP_NUM_ROWS * TILE_SIZE){
		float xToCheck = nextHorizTouchX;
		float yToCheck = nextHorizTouchY + (isRayFacingUp ? -1: 0);
		if(mapHasWallAt(xToCheck, yToCheck)){
			foundHorizWallHit = true;
			horizWallHitX = nextHorizTouchX;
			horizWallHitY = nextHorizTouchY;
			horizWallContent = map[(int)floor(yToCheck/TILE_SIZE)][(int)floor(xToCheck/TILE_SIZE)];
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
	xintercept += isRayFacingRight ? TILE_SIZE : 0;

	yintercept = player.y + (xintercept - player.x) * tan(rayAngle);

	xstep = TILE_SIZE;
	xstep *= isRayFacingLeft ? -1 : 1;

	ystep = TILE_SIZE * tan(rayAngle);
	ystep *= (isRayFacingUp && ystep > 0) ? -1 : 1;
	ystep *= (isRayFacingDown && ystep < 0) ? -1 : 1;

	float nextVertTouchX = xintercept;
	float nextVertTouchY = yintercept;

	while(nextVertTouchX >= 0 && nextVertTouchX <= MAP_NUM_COLS * TILE_SIZE && nextVertTouchY >= 0 && nextVertTouchY <= MAP_NUM_ROWS * TILE_SIZE){
		float xToCheck = nextVertTouchX + (isRayFacingLeft ? -1 : 0);
		float yToCheck = nextVertTouchY;
		if(mapHasWallAt(xToCheck, yToCheck)){
			foundVertWallHit = true;
			vertWallHitX = nextVertTouchX;
			vertWallHitY = nextVertTouchY;
			vertWallContent = map[(int)floor(yToCheck/TILE_SIZE)] [(int)floor(xToCheck/TILE_SIZE)];
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

void renderMap(){
	// for(int i = 0; i < MAP_NUM_ROWS; i++){
    //     for (int j = 0; j < MAP_NUM_COLS; j++){
    //         int tileX = j * TILE_SIZE;
    //         int tileY = i * TILE_SIZE;
    //         int tileColor = map[i][j] != 0 ? 255 : 0;

	// 		SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
	// 		SDL_Rect mapTileRect = {
	// 			tileX * MINIMAP_SCALE_FACTOR,
	// 			tileY * MINIMAP_SCALE_FACTOR,
	// 			TILE_SIZE * MINIMAP_SCALE_FACTOR,
	// 			TILE_SIZE * MINIMAP_SCALE_FACTOR
	// 		};

	// 		SDL_RenderFillRect(renderer, &mapTileRect);
    //     }
    // }
}

void renderRays(){
	// SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	// for(int i = 0; i < NUM_RAYS; i++){
	// 	SDL_RenderDrawLine(
	// 		renderer,
	// 		MINIMAP_SCALE_FACTOR * player.x,
	// 		MINIMAP_SCALE_FACTOR * player.y,
	// 		MINIMAP_SCALE_FACTOR * rays[i].wallHitX,
	// 		MINIMAP_SCALE_FACTOR * rays[i].wallHitY
	// 	);
	// }
}

void processInput(){
	SDL_Event event;
	SDL_PollEvent(&event);
	switch(event.type){
		case SDL_QUIT:{
			isGameRunning = false;
			break;
		}
		case SDL_KEYDOWN:{
			if(event.key.keysym.sym == SDLK_ESCAPE)
				isGameRunning = false;
			if(event.key.keysym.sym == SDLK_UP)
				player.walkDirection = +1;
			if(event.key.keysym.sym == SDLK_DOWN)
				player.walkDirection = -1;
			if(event.key.keysym.sym == SDLK_RIGHT)
				player.turnDirection = +1;			
			if(event.key.keysym.sym == SDLK_LEFT)
				player.turnDirection = -1;
			break;
		}
		case SDL_KEYUP:{
			if(event.key.keysym.sym == SDLK_UP)
				player.walkDirection = 0;
			if(event.key.keysym.sym == SDLK_DOWN)
				player.walkDirection = 0;
			if(event.key.keysym.sym == SDLK_RIGHT)
				player.turnDirection = 0;			
			if(event.key.keysym.sym == SDLK_LEFT)
				player.turnDirection = 0;
			break;
		}		
	}
}

void update(){
	//calculate time to wait until we reach target frame time
	int timeToWait = (SDL_GetTicks() - ticksLastFrame);
	
	//waste or sleep until we reach the target frame time length. Use delay function
	//to save on resources.
	if(timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH){
		SDL_Delay(timeToWait);
	}	

	//Implement deltatime
	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

	ticksLastFrame = SDL_GetTicks();
		
	//TODO: remember to update game objects as a function of deltatime.
	movePlayer(deltaTime);
	castAllRays();
		
}

void renderWallProjection(){
	for(int x = 0; x < NUM_RAYS; x++){
		float perpDistance = rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle);
		float projectedWallHeight = (TILE_SIZE / perpDistance) * DIST_PROJ_PLANE;
		
		int wallStripHeight = (int) projectedWallHeight;

		int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
		wallTopPixel = wallTopPixel < 0 ?  0 : wallTopPixel; //we have to clamp to 0 or we'll get segfault.

		int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
		wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

		//render ceiling
		for(int y = 0; y < wallTopPixel; y++){
			drawPixel(x, y, 0xFF333333);
		}

		int textureOffsetX;
		//calculate textureOffsetX
		if(rays[x].wasHitVertical){
			//perform offset for vert hit
			textureOffsetX = (int) rays[x].wallHitY % TILE_SIZE;
		}
		else{
			//perform offset for horiz hit.
			textureOffsetX = (int) rays[x].wallHitX % TILE_SIZE;
		}

		//get correct texture id number from the map content
		int texNum = rays[x].wallHitContent - 1;

		int texture_width = wallTextures[texNum].width;
		int texture_height = wallTextures[texNum].height;

		//render the wall from wallTopPixel to wallBottomPixel
		for(int y = wallTopPixel; y < wallBottomPixel; y++){
			
			//calculate textureOffsetY (y offset from top * height factor)
			int distanceFromTop = y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2);
			int textureOffsetY = distanceFromTop * ((float)texture_height/ wallStripHeight);

			//Set the colours of the pixels based on the wallTexture in memory.
			uint32_t texelColour = wallTextures[texNum].texture_buffer[(texture_width * textureOffsetY) + textureOffsetX];
			drawPixel(x, y, texelColour);
		}

		//Render floor
		for(int y = wallBottomPixel; y < WINDOW_HEIGHT; y++){
			drawPixel(x, y, 0xFF5A5A5A);
		}
	}
}

void render(){
	//Clear the colour buffer
	clearColourBuffer(0xFF000000);

	renderWallProjection();

	//Display the minimap
	// renderMap();
	// renderRays();
	// renderPlayer();	
	
	renderColourBuffer();
}

void releaseResources(void){
	freeWallTextures();
	destroyWindow();
}

int main(){
	isGameRunning = initializeWindow();

	setup();

	while(isGameRunning){
		processInput();
		update();
		render();
	}
	
	releaseResources();

	return 0;
}
