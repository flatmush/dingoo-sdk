#include "hud.h"

#include <stdio.h>

#include <sml/fixmath.h>
#include <sml/graphics.h>

#include "game.h"



void hudDrawGuage(uint32_t inCur, uint32_t inMax, int32_t inX, int32_t inY, uint32_t inWidth, uint32_t inHeight, gfx_color inBright, gfx_color inDark) {
	uint32_t tempFill = fract_usmul(inWidth, fract_create(inCur, inMax));
	gfx_rect_fill_draw(inX, inY, tempFill, ((inHeight + 1) >> 1), inBright);
	gfx_rect_fill_draw(inX, (inY + ((inHeight + 1) >> 1)), tempFill, (inHeight >> 1), inDark);
	gfx_rect_draw(inX, inY, inWidth, inHeight, gfx_color_rgb(0xFF, 0xFF, 0xFF));
}

void hudDrawLives(uint32_t inLives, int32_t inX, int32_t inY, gfx_color inBright, gfx_color inDark) {
	uintptr_t i;
	for(i = 0; i < (inLives > 5 ? 4 : inLives); i++, inX += 10) {
		gfx_rect_fill_draw(inX, inY, 8, 4, inBright);
		gfx_rect_fill_draw(inX, (inY + 4), 8, 4, inDark);
		gfx_rect_draw(inX, inY, 8, 8, gfx_color_rgb(0xFF, 0xFF, 0xFF));
	}
	if(inLives > 5) {
		gfx_rect_draw(inX, (inY + 2), 8, 4, gfx_color_rgb(0xFF, 0xFF, 0xFF));
		gfx_rect_draw((inX + 2), inY, 4, 8, gfx_color_rgb(0xFF, 0xFF, 0xFF));
		gfx_rect_fill_draw((inX + 1), (inY + 3), 6, 1, inBright);
		gfx_rect_fill_draw((inX + 1), (inY + 4), 6, 1, inDark);
		gfx_rect_fill_draw((inX + 3), (inY + 1), 2, 3, inBright);
		gfx_rect_fill_draw((inX + 3), (inY + 4), 2, 3, inDark);
	}
}

void hudDrawAltimeter(ship* inShip, level* inLevel) {
	if((inShip == NULL) || (inLevel == NULL))
		return;

	uintptr_t tempTop    = 16;
	uintptr_t tempBottom = (gameDisplay->height - tempTop);
	uintptr_t tempRight  = (gameDisplay->width - 8);
	uintptr_t tempLeft   = (tempRight - 5);

	int32_t tempLevelTop    = inLevel->points[0];
	int32_t tempLevelBottom = inLevel->points[0];
	uintptr_t i;
	for(i = 1; i < inLevel->points_count; i++) {
		if(inLevel->points[i] > tempLevelTop)
			tempLevelTop = inLevel->points[i];
		else if(inLevel->points[i] < tempLevelBottom)
			tempLevelBottom = inLevel->points[i];
	}

	int32_t tempShipAlt = inShip->y >> 16;
	tempShipAlt -= tempLevelBottom >> 16;
	tempShipAlt *= (tempBottom - tempTop);
	tempShipAlt /= (tempLevelTop - tempLevelBottom) >> 15;
	tempShipAlt  = (tempBottom - tempShipAlt);

	int32_t tempTargetAlt = inLevel->points[inLevel->target_point] >> 16;
	tempTargetAlt -= tempLevelBottom >> 16;
	tempTargetAlt *= (tempBottom - tempTop);
	tempTargetAlt /= (tempLevelTop - tempLevelBottom) >> 15;
	tempTargetAlt  = (tempBottom - tempTargetAlt);

	int32_t tempLandAlt = levelHeightMax(inLevel, (inShip->x - (4 << 16)), (8 << 16)).height >> 16;
	tempLandAlt -= tempLevelBottom >> 16;
	tempLandAlt *= (tempBottom - tempTop);
	tempLandAlt /= (tempLevelTop - tempLevelBottom) >> 15;
	tempLandAlt  = (tempBottom - tempLandAlt);

	gfx_line_draw(tempLeft, tempTop, tempRight, tempTop, gfx_color_rgb(0xFF, 0xFF, 0xFF));
	gfx_line_draw(tempLeft, tempBottom, tempRight, tempBottom, gfx_color_rgb(0xFF, 0xFF, 0xFF));

	gfx_line_draw(tempLeft, tempLandAlt, tempRight, tempLandAlt, gfx_color_rgb(0x7F, 0x7F, 0x7F));
	gfx_line_draw(tempLeft, tempTargetAlt, tempRight, tempTargetAlt, gfx_color_rgb(0x00, 0xBF, 0x00));

	if(inShip->y > (tempLevelTop + (tempLevelTop - tempLevelBottom))) {
		char tempString[256];
		tempShipAlt = (inShip->y - tempLevelBottom) >> 24;
		sprintf(tempString, "Altitude: %li km", tempShipAlt);
		gfx_font_print_center(((gameDisplay->height >> 1) - 16), gameFont, tempString);
	} else
		gfx_line_draw(tempLeft, tempShipAlt, tempRight, tempShipAlt, gfx_color_rgb(0xFF, 0xFF, 0xFF));

	gfx_line_draw(tempRight, tempTop, tempRight, (tempBottom + 1), gfx_color_rgb(0xFF, 0xFF, 0xFF));
}

void hudDrawTargetDir(ship* inShip, level* inLevel, int32_t inX, int32_t inY, gfx_color inBright, gfx_color inDark) {
	if((inShip == NULL) || (inLevel == NULL))
		return;

	inX -= 4;
	uint32_t tempSegWidth = inLevel->width / (inLevel->points_count - 1);
	int32_t tempTargetStart = inLevel->target_point * tempSegWidth;
	int32_t tempTargetEnd   = tempTargetStart + (inLevel->target_width * tempSegWidth);
	if(inShip->x >= tempTargetStart) {
		if(inShip->x < tempTargetEnd) {
			gfx_rect_fill_draw(inX, inY, 8, 4, inBright);
			gfx_rect_fill_draw(inX, (inY + 4), 8, 4, inDark);
			gfx_rect_draw(inX, inY, 8, 8, gfx_color_rgb(0xFF, 0xFF, 0xFF));
		} else {
			if((inShip->x - (gameDisplay->width << 14)) >= tempTargetEnd) {
				inX -= 6;
				gfx_tri_fill_draw((inX + 8), inY, inX, (inY + 4), (inX + 8), (inY + 4), inBright);
				gfx_tri_fill_draw((inX + 8), (inY + 8), inX, (inY + 4), (inX + 8), (inY + 4), inDark);
				gfx_tri_draw((inX + 8), inY, inX, (inY + 4), (inX + 8), (inY + 8), gfx_color_rgb(0xFF, 0xFF, 0xFF));
				inX += 12;
			}
			gfx_tri_fill_draw((inX + 8), inY, inX, (inY + 4), (inX + 8), (inY + 4), inBright);
			gfx_tri_fill_draw((inX + 8), (inY + 8), inX, (inY + 4), (inX + 8), (inY + 4), inDark);
			gfx_tri_draw((inX + 8), inY, inX, (inY + 4), (inX + 8), (inY + 8), gfx_color_rgb(0xFF, 0xFF, 0xFF));
		}
	} else {
		if((inShip->x + (gameDisplay->width << 14)) < tempTargetStart) {
			inX -= 6;
			gfx_tri_fill_draw(inX, inY, (inX + 8), (inY + 4), inX, (inY + 4), inBright);
			gfx_tri_fill_draw(inX, (inY + 8), (inX + 8), (inY + 4), inX, (inY + 4), inDark);
			gfx_tri_draw(inX, inY, (inX + 8), (inY + 4), inX, (inY + 8), gfx_color_rgb(0xFF, 0xFF, 0xFF));
			inX += 12;
		}
		gfx_tri_fill_draw(inX, inY, (inX + 8), (inY + 4), inX, (inY + 4), inBright);
		gfx_tri_fill_draw(inX, (inY + 8), (inX + 8), (inY + 4), inX, (inY + 4), inDark);
		gfx_tri_draw(inX, inY, (inX + 8), (inY + 4), inX, (inY + 8), gfx_color_rgb(0xFF, 0xFF, 0xFF));
	}
}

void hudDraw(ship* inShip) {
	if(inShip == NULL)
		return;

	hudDrawGuage(inShip->hp, inShip->hp_max, 4, 4, 64, 8, gfx_color_rgb(0xFF, 0x00, 0x00), gfx_color_rgb(0xBF, 0x00, 0x00));
	hudDrawGuage(inShip->fuel, inShip->fuel_max, 4, 16, 64, 8, gfx_color_rgb(0xFF, 0xFF, 0x00), gfx_color_rgb(0xBF, 0xBF, 0x00));
	hudDrawLives(gameLives, 4, 32, gfx_color_rgb(0x00, 0xFF, 0x00), gfx_color_rgb(0x00, 0xBF, 0x00));
	hudDrawTargetDir(inShip, gameLevel, (gameDisplay->width >> 1), 32, gfx_color_rgb(0x00, 0x00, 0xFF), gfx_color_rgb(0x00, 0x00, 0xBF));
	hudDrawAltimeter(inShip, gameLevel);

	char tempString[32];
	unsigned long int tempSeconds  = (gameTime / timer_resolution);
	unsigned long int tempFraction = ((gameTime % timer_resolution) * 10) / timer_resolution;
	int score_hud_width = 4 + gfx_font_width(gameFont, "Score: 00000000");

	gfx_font_print_center(4, gameFont, "Time");
	sprintf(tempString, "%lu.%01lu", tempSeconds, tempFraction);
	gfx_font_print_center(16, gameFont, tempString);

	sprintf(tempString, "Score: %lu", (unsigned long int)gameScore);
	gfx_font_print((gameDisplay->width - score_hud_width), 4, gameFont, tempString);
	sprintf(tempString, "Level: %lu", (unsigned long int)gameLevelNo);
	gfx_font_print((gameDisplay->width - score_hud_width), 16, gameFont, tempString);

	gfx_font_print_fromright((gameDisplay->width - 2), (gameDisplay->height - gfx_font_height(gameFont) - 2), gameFont, "[PAUSE]");
	gfx_font_print(2, (gameDisplay->height - gfx_font_height(gameFont) - 2), gameFont, "[SCREENSHOT]");

	/* print the current difficulty in the bottom-middle of the screen */
	gfx_font_print_center((gameDisplay->height - gfx_font_height(gameFont) - 2), gameFont, gameDifficultyStrings[gameDifficulty - 1]);
}
