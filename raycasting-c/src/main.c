#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "defs.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include "wall.h"

bool isGameRunning = false;
int ticksLastFrame = 0;

void setup(){
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
		
	movePlayer(deltaTime);
	castAllRays();
		
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
 