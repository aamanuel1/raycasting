#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "defs.h"

bool initializeWindow(void);
void destroyWindow(void);
void clearColourBuffer(colour_t colour);
void renderColourBuffer(void);
void drawPixel(int x, int y, colour_t colour);
void drawRect(int x, int y, int widthm, int height, colour_t colour);
void drawLine(int x0, int y0, int x1, int y1, colour_t colour);

#endif