#ifndef TEXTURES_H // Protection guard against repeated inclusion
#define TEXTURES_H

#include <stdint.h>
#include "defs.h"
#include "upng.h"

typedef struct {
    upng_t* upngTexture;
    int width;
    int height;
    colour_t* texture_buffer;
} texture_t;

//declare as extern due to multiple definition problem at linker stage.
extern texture_t wallTextures[NUM_TEXTURES];

void loadWallTextures();
void freeWallTextures();

#endif