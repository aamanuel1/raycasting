#ifndef TEXTURES_H // Protection guard against repeated inclusion
#define TEXTURES_H

#include <stdint.h>
#include "constants.h"
#include "upng.h"

typedef struct {
    upng_t* upngTexture;
    int width;
    int height;
    uint32_t* texture_buffer;
} texture_t;

//declare as extern due to multiple definition problem at linker stage.
extern texture_t wallTextures[NUM_TEXTURES];

void loadWallTextures();
void freeWallTextures();

#endif