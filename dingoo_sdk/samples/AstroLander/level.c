#include "level.h"

#include <stdlib.h>

#include <sml/fixmath.h>
#include <sml/graphics.h>

#include "game.h"



uint8_t _rand8() {
	uint32_t tempRand = rand();
	return ((tempRand >> 24) ^ (tempRand >> 16) ^ (tempRand >> 8) ^ tempRand);
}



level* levelCreate(uint32_t inPoints, uint32_t inWidth, int32_t inGravity, uint8_t inSmooth) {
	if(inWidth < ((uint32_t)gameDisplay->width << 16))
		inWidth = ((uint32_t)gameDisplay->width << 16);

	level* tempLevel = (level*)malloc(sizeof(level) + ((inPoints + 1) << 2));
	if(tempLevel == NULL)
		return NULL;
	tempLevel->points = (int32_t*)((uintptr_t)tempLevel + sizeof(level));
	tempLevel->points_count = (inPoints + 1);
	tempLevel->width = inWidth;
	tempLevel->target_width = 1;
	uint32_t tempSegWidth = tempLevel->width / (tempLevel->points_count - 1);
	while((tempLevel->target_width * tempSegWidth) < (10 << 16))
		tempLevel->target_width++;
	tempLevel->target_point = (rand() % (tempLevel->points_count - tempLevel->target_width));
	tempLevel->start_time = OSTimeGet();

	uint32_t i;
	tempLevel->points[0] = 0;
	for(i = 1; i < tempLevel->points_count; i++) {
		if((i > tempLevel->target_point) && (i <= (tempLevel->target_point + tempLevel->target_width))) {
			tempLevel->points[i] = tempLevel->points[i - 1];
			continue;
		}
		if((i == 0) || ((_rand8() & 0xFF) >= inSmooth)) {
			// Potential cliff.
			tempLevel->points[i] = fix16_sadd(tempLevel->points[i - 1], ((rand() % 0x007FFFFF) - 0x00400000));
		} else {
			// Smooth
			tempLevel->points[i] = fix16_sadd(tempLevel->points[i - 1], ((rand() % 0x003FFFFF) - 0x00200000));
		}
	}

	tempLevel->spawn_x = (tempLevel->width >> 1);
	tempLevel->spawn_y = levelHeightMax(tempLevel, (tempLevel->spawn_x - (4 << 16)), (8 << 16)).height + (64 << 16);
	tempLevel->gravity = inGravity;

	return tempLevel;
}

void levelDelete(level* inLevel) {
	free(inLevel);
}

void levelDraw(level* inLevel, int32_t inOffX, int32_t inOffY) {
	if(inLevel == NULL)
		return;

	uint32_t tempSegWidth = inLevel->width / (inLevel->points_count - 1);

	uintptr_t i;
	int32_t tempX, tempXLast, tempY[2];

	tempX = inOffX + tempSegWidth;
	tempXLast = inOffX;
	tempY[0] = (inLevel->points[0] + inOffY);
	for(i = 1; i < inLevel->points_count; i++, tempX += tempSegWidth) {
		tempY[i & 1] = (inLevel->points[i] + inOffY);
		if(tempX < 0)
			continue;
		tempXLast = (tempX - tempSegWidth);
		if((tempXLast >> 16) > gameDisplay->width)
			break;
		if((tempY[~i & 1] >> 16) < (tempY[i & 1] >> 16)) {
			gfx_tri_fill_draw((tempXLast >> 16), (gameDisplay->height - (tempY[~i & 1] >> 16)), (tempX >> 16), (gameDisplay->height - (tempY[i & 1] >> 16)), (tempX >> 16), (gameDisplay->height - (tempY[~i & 1] >> 16)), gfx_color_rgb(0x3F, 0x3F, 0x3F));
			gfx_rect_fill_draw((tempXLast >> 16), (gameDisplay->height - (tempY[~i & 1] >> 16)), (((tempX - (tempXLast & 0xFFFF0000)) + 0x00008000) >> 16), (tempY[~i & 1] >> 16), gfx_color_rgb(0x3F, 0x3F, 0x3F));
		} else {
			gfx_tri_fill_draw((tempXLast >> 16), (gameDisplay->height - (tempY[~i & 1] >> 16)), (tempX >> 16), (gameDisplay->height - (tempY[i & 1] >> 16)), (tempXLast >> 16), (gameDisplay->height - (tempY[i & 1] >> 16)), gfx_color_rgb(0x3F, 0x3F, 0x3F));
			gfx_rect_fill_draw((tempXLast >> 16), (gameDisplay->height - (tempY[i & 1] >> 16)), (((tempX - (tempXLast & 0xFFFF0000)) + 0x00008000) >> 16), (tempY[i & 1] >> 16), gfx_color_rgb(0x3F, 0x3F, 0x3F));
		}
	}

	tempX = inOffX + tempSegWidth;
	tempXLast = inOffX;
	tempY[0] = (inLevel->points[0] + inOffY);
	for(i = 1; i < inLevel->points_count; i++, tempX += tempSegWidth) {
		tempY[i & 1] = (inLevel->points[i] + inOffY);
		if(tempX < 0)
			continue;
		tempXLast = (tempX - tempSegWidth);
		if((tempXLast >> 16) > gameDisplay->width)
			break;
		gfx_line_draw((tempXLast >> 16), (gameDisplay->height - (tempY[~i & 1] >> 16)), (tempX >> 16), (gameDisplay->height - (tempY[i & 1] >> 16)), gfx_color_rgb(0x7F, 0x7F, 0x7F));
	}

	tempXLast = inOffX + (tempSegWidth * inLevel->target_point);
	tempY[0] = inOffY + inLevel->points[inLevel->target_point];
	gfx_rect_fill_draw((tempXLast >> 16), (gameDisplay->height - (tempY[0] >> 16)), ((tempSegWidth * inLevel->target_width) >> 16), 8, gfx_color_rgb(0x00, 0xFF, 0x00));
	gfx_rect_draw((tempXLast >> 16), (gameDisplay->height - (tempY[0] >> 16)), ((tempSegWidth * inLevel->target_width) >> 16), 8, gfx_color_rgb(0x00, 0x7F, 0x00));
}

level_point levelHeight(level* inLevel, int32_t inX) {
	level_point tempOut = { 0, false };
	if(inLevel == NULL)
		return tempOut;
	if((inX < 0) || (inX >= (int32_t)inLevel->width))
		return tempOut;
	uint32_t tempSegWidth = (inLevel->width / (inLevel->points_count - 1));
	uint32_t tempIndex = (inX / tempSegWidth);
	tempOut.land = ((tempIndex >= inLevel->target_point) && (tempIndex < (inLevel->target_point + inLevel->target_width)));
	fract32_t tempFract = fract32_create((inX % tempSegWidth), tempSegWidth);
	tempOut.height = fract32_smul(inLevel->points[tempIndex], fract32_invert(tempFract));
	tempOut.height += fract32_smul(inLevel->points[tempIndex + 1], tempFract);
	return tempOut;
}

level_point levelHeightMax(level* inLevel, int32_t inX, uint32_t inWidth) {
	level_point tempOut = { 0, false };
	if(inLevel == NULL)
		return tempOut;
	if(inX < 0) {
		if((int32_t)inWidth < -inX)
			return tempOut;
		inWidth += inX;
		inX = 0;
	}
	if((inX + inWidth) >= inLevel->width) {
		if(inX > (int32_t)inLevel->width)
			return tempOut;
		inWidth = (inLevel->width - (inX + 1));
	}

	uint32_t tempSegWidth = (inLevel->width / (inLevel->points_count - 1));
	uint32_t tempIndex = (inX / tempSegWidth);
	if((int32_t)(tempIndex * tempSegWidth) < inX)
		tempIndex++;

	tempOut = levelHeight(inLevel, inX);
	level_point tempPoint = levelHeight(inLevel, (inX + inWidth - 1));
	if(tempPoint.height > tempOut.height)
		tempOut = tempPoint;

	for(; (tempIndex * tempSegWidth) < (inX + inWidth); tempIndex++) {
		if(inLevel->points[tempIndex] > tempOut.height) {
			tempOut.height = inLevel->points[tempIndex];
			tempOut.land   = ((tempIndex >= inLevel->target_point) && (tempIndex < (inLevel->target_point + inLevel->target_width)));
		}
	}

	return tempOut;
}
