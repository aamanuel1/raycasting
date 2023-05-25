#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <limits.h>
#include <stdbool.h>
#include "defs.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"
#include "player.h"
#include "ray.h"

bool isGameRunning = false;
int ticksLastFrame = 0;

void setup(){

	//see textures.h and textures.c
	//asks uPNG to decode all PNG files and loads wallTextures array
	loadWallTextures();
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
	renderMap();
	renderRays();
	renderPlayer();	
	
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
 