#include <stdio.h>
#include <SDL2/SDL.h>
#include "constants.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

int mapHasWallAt(float x, float y){
	if(x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT){
		return TRUE;
	}
	int mapX = floor(x / TILE_SIZE);
	int mapY = floor(y / TILE_SIZE);

	if(map[mapY][mapX] == 0)
		return FALSE;
	
	return TRUE;
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

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int isGameRunning = FALSE;
int ticksLastFrame = 0;

int initializeWindow(){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		fprintf(stderr, "Error initializing SDL.\n");
		return FALSE;
	}
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_BORDERLESS
	);
	if(!window){
	fprintf(stderr, "Error creating SDL window.\n");
	}

	renderer = SDL_CreateRenderer(window, -1, 0); //1 is get default driver
	if(!renderer){
	fprintf(stderr, "Error creating SDL renderer.\n");
	return FALSE;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //how to blend texture
	return TRUE;
}

void destroyWindow(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

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
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect playerRect = {
		player.x * MINIMAP_SCALE_FACTOR,
		player.y * MINIMAP_SCALE_FACTOR,
		player.width * MINIMAP_SCALE_FACTOR,
		player.height * MINIMAP_SCALE_FACTOR
	};
	SDL_RenderFillRect(renderer, &playerRect);

	//Draw the mid line
	SDL_RenderDrawLine(
		renderer, 
		MINIMAP_SCALE_FACTOR * player.x,
		MINIMAP_SCALE_FACTOR * player.y,
		MINIMAP_SCALE_FACTOR + player.x + cos(player.rotationAngle) * 40,
		MINIMAP_SCALE_FACTOR + player.y + sin(player.rotationAngle) * 40
	);
}

void renderMap(){
	for(int i = 0; i < MAP_NUM_ROWS; i++){
        for (int j = 0; j < MAP_NUM_COLS; j++){
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            int tileColor = map[i][j] != 0 ? 255 : 0;

			SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
			SDL_Rect mapTileRect = {
				tileX * MINIMAP_SCALE_FACTOR,
				tileY * MINIMAP_SCALE_FACTOR,
				TILE_SIZE * MINIMAP_SCALE_FACTOR,
				TILE_SIZE * MINIMAP_SCALE_FACTOR
			};

			SDL_RenderFillRect(renderer, &mapTileRect);
        }
    }
}

void processInput(){
	SDL_Event event;
	SDL_PollEvent(&event);
	switch(event.type){
		case SDL_QUIT:{
			isGameRunning = FALSE;
			break;
		}
		case SDL_KEYDOWN:{
			if(event.key.keysym.sym == SDLK_ESCAPE)
				isGameRunning = FALSE;
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
		
}

void render(){
	
	//clear the frame
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	//TODO:	
	//render all game objects for the current frame
	renderMap();
	// renderRays();
	renderPlayer();
	

	//swap the buffer
	SDL_RenderPresent(renderer);
}

int main(){
	isGameRunning = initializeWindow();

	setup();

	while(isGameRunning){
		processInput();
		update();
		render();
	}
	
	destroyWindow();

	return 0;
}
