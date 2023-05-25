#include "graphics.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
uint32_t* colourBuffer = NULL;
SDL_Texture* colourBufferTexture;

bool initializeWindow(){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	int fullScreenWidth = display_mode.w;
	int fullScreenHeight = display_mode.h;
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		fullScreenWidth,
		fullScreenHeight,
		SDL_WINDOW_BORDERLESS
	);
	if(!window){
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0); //1 is get default driver
	if(!renderer){
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //how to blend texture
	
    //Allocate the colour buffer in memory
	colourBuffer = (uint32_t*) malloc(sizeof(uint32_t) * (uint32_t)WINDOW_WIDTH * (uint32_t)WINDOW_HEIGHT);

	//Create SDL_Texture to display the colour buffer.
	colourBufferTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);
    return true;
}

void destroyWindow(){
	free(colourBuffer); //Remember to deallocate the buffer.
	//Deallocate Texture using DestroyTexture since a pointer is returned. 
	SDL_DestroyTexture(colourBufferTexture); 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void clearColourBuffer(uint32_t colour){
	for(int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++){
		colourBuffer[i] = colour;
	}
}

void renderColourBuffer(){
	SDL_UpdateTexture(colourBufferTexture, 
				NULL, 
				colourBuffer, 
				(int) ((uint32_t)WINDOW_WIDTH * sizeof(uint32_t))
				); 

	SDL_RenderCopy(renderer, colourBufferTexture, NULL, NULL);

    //swap the buffer
	SDL_RenderPresent(renderer);
}

void drawPixel(int x, int y, uint32_t colour){
    colourBuffer[(WINDOW_WIDTH * y) + x] = colour;
}

void drawRect(int x, int y, int width, int height, uint32_t colour){
	for(int i = x; i <= (x + width); i++){
		for(int j = y; j <= (y + height); j++){
			drawPixel(i, j, colour);
		}
	}
}

void drawLine(int x0, int y0, int x1, int y1, uint32_t colour){

	//Uses DDA algorithm
	//get delta x and delta y and the side length (longest side).
	int deltaX = x1 - x0;
	int deltaY = y1 - y0;

	float sideLength = abs(deltaX) >= abs(deltaY) ? abs(deltaX) : abs(deltaY);

	//find out how much we should increment in both x and y each step
	float xInc = deltaX / (float) sideLength;
	float yInc = deltaY / (float) sideLength;  

	//set current x and current y
	float currentX = x0;
	float currentY = y0;

	//draw the rasterized line.
	for(int i = 0; i < sideLength; i++){
		drawPixel(round(currentX), round(currentY), colour);
		currentX += xInc;
		currentY += yInc;
	}

}