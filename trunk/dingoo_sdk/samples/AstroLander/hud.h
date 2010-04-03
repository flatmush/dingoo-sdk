#ifndef __hud_h__
#define __hud_h__

#include <stdbool.h>
#include <stdint.h>

#include <sml/graphics.h>

#include "ship.h"
#include "level.h"



extern void hudDrawGuage(uint32_t inCur, uint32_t inMax, int32_t inX, int32_t inY, uint32_t inWidth, uint32_t inHeight, gfx_color inBright, gfx_color inDark);
extern void hudDrawLives(uint32_t inLives, int32_t inX, int32_t inY, gfx_color inBright, gfx_color inDark);
extern void hudDrawAltimeter(ship* inShip, level* inLevel);
extern void hudDrawTargetDir(ship* inShip, level* inLevel, int32_t inX, int32_t inY, gfx_color inBright, gfx_color inDark);
extern void hudDraw(ship* inShip);

#endif
