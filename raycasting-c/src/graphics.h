#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "defs.h"

bool initializeWindow(void);
void destroyWindow(void);
void clearColourBuffer(uint32_t colour);
void renderColourBuffer(void);
void drawPixel(int x, int y, uint32_t colour);

#endif