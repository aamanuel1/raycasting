#include "wall.h"

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
			colour_t texelColour = wallTextures[texNum].texture_buffer[(texture_width * textureOffsetY) + textureOffsetX];
			
			//make pixel colour darker if ray hit was vertical.
			if(rays[x].wasHitVertical){
				changeColourIntensity(&texelColour, 0.7);
			}
			
			drawPixel(x, y, texelColour);
		}

		//Render floor
		for(int y = wallBottomPixel; y < WINDOW_HEIGHT; y++){
			drawPixel(x, y, 0xFF5A5A5A);
		}
	}
}


void changeColourIntensity(colour_t* colour, float factor){
	colour_t a = (*colour & 0xFF000000);
	colour_t r = (*colour & 0x00FF0000) * factor;
	colour_t g = (*colour & 0x0000FF00) * factor;
	colour_t b = (*colour & 0x000000FF) * factor;
	
	*colour = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}