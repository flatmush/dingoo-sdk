#ifndef __level_h__
#define __level_h__

#include <stdbool.h>
#include <stdint.h>



typedef struct {
	int32_t* points;
	uint32_t points_count;
	uint32_t target_point;
	uint32_t target_width;
	uint32_t width;
	uint32_t start_time;
	int32_t  spawn_x, spawn_y;
	int32_t  gravity;
} level;

typedef struct {
	int32_t height;
	bool    land;
} level_point;



#include "ship.h"



extern level*      levelCreate(uint32_t inPoints, uint32_t inWidth, int32_t inGravity, uint8_t inSmooth);
extern void        levelDelete(level* inLevel);
extern void        levelDraw(level* inLevel, int32_t inOffX, int32_t inOffY);
extern level_point levelHeight(level* inLevel, int32_t inX);
extern level_point levelHeightMax(level* inLevel, int32_t inX, uint32_t inWidth);

#endif
