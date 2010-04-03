#include "ship.h"

#include <stdlib.h>
#include <stdio.h>

#include <sml/fixmath.h>
#include <sml/control.h>
#include <sml/graphics.h>
#include <sml/sound.h>
#include <sml/wav.h>

#include "game.h"
#include "hud.h"



sound_t*  _shipThrustSound    = NULL;
sound_t*  _shipBoostSound     = NULL;
uintptr_t _shipThrustRefCount = 0;
uintptr_t _shipThrustLoop     = 0;



ship* shipCreate(int32_t inX, int32_t inY) {
	if(inX < 0)
		inX = -inX;
	if(inY < 0)
		inY = -inY;

	ship* tempShip = (ship*)malloc(sizeof(ship));
	if(tempShip == NULL)
		return NULL;

	tempShip->x = inX;
	tempShip->y = inY;
	tempShip->vel_x = 0;
	tempShip->vel_y = 0;
	tempShip->angle = 0;

	/* read the current difficulty and configure the ship as such -- don't worry
		about the point system rewarding easy players... a difficulty-based
		multiplier is applied to total points later. */

	tempShip->hp_max   = (50  * (5 - gameDifficulty));
	tempShip->fuel_max = (400 * (5 - gameDifficulty));

	tempShip->hp   = tempShip->hp_max;
	tempShip->fuel = tempShip->fuel_max;

	tempShip->thrust_y = (1 << 13);
	tempShip->thrust_x = (tempShip->thrust_y >> 1);

	tempShip->thrust_visible = 0;

	if((_shipThrustRefCount == 0) || (_shipThrustSound == NULL)) {
		_shipThrustSound = wav_load("thrust.wav");
		_shipBoostSound  = wav_load("boost.wav");
	}
	_shipThrustRefCount++;

	return tempShip;
}

void shipDelete(ship* inShip) {
	_shipThrustRefCount--;
	if(_shipThrustRefCount == 0) {
		sound_stop(0);
		_shipThrustLoop = 0;
		wav_delete(_shipThrustSound);
		_shipThrustSound = NULL;
		wav_delete(_shipBoostSound);
		_shipBoostSound = NULL;
	}
	free(inShip);
}

void shipDraw(ship* inShip, int32_t inOffX, int32_t inOffY) {
	if(inShip == NULL)
		return;

	fix16_t tempX = (inShip->x + inOffX);
	fix16_t tempY = (inShip->y + inOffY);

	fix16_t tempPointsX[3];
	fix16_t tempPointsY[3];
	fix16_t tempSin = fix16_sin(inShip->angle);
	fix16_t tempCos = fix16_cos(inShip->angle);

	if(inShip->thrust_visible > 0) {
		gfx_color tempThrustColor = gfx_color_rgb(0xFF, (0x1F + (rand() & 0x3F)), 0x00);
		tempPointsX[0] = (tempX - (tempCos * 2));
		tempPointsY[0] = (fix16_from_int(gameDisplay->height) - (tempY + (tempSin * 2)));
		tempPointsX[1] = (tempX + (tempCos * 2));
		tempPointsY[1] = (fix16_from_int(gameDisplay->height) - (tempY - (tempSin * 2)));
		tempPointsX[2] = (tempX - (tempSin * inShip->thrust_visible));
		tempPointsY[2] = (fix16_from_int(gameDisplay->height) - (tempY - (tempCos * inShip->thrust_visible)));
		gfx_tri_fill_draw(fix16_to_int(tempPointsX[0]), fix16_to_int(tempPointsY[0]), fix16_to_int(tempPointsX[1]), fix16_to_int(tempPointsY[1]), fix16_to_int(tempPointsX[2]), fix16_to_int(tempPointsY[2]), tempThrustColor);
	}

	tempPointsX[0] = (tempX - (tempCos * 4));
	tempPointsY[0] = (fix16_from_int(gameDisplay->height) - (tempY + (tempSin * 4)));
	tempPointsX[1] = (tempX + (tempCos * 4));
	tempPointsY[1] = (fix16_from_int(gameDisplay->height) - (tempY - (tempSin * 4)));
	tempPointsX[2] = (tempX + (tempSin * 16));
	tempPointsY[2] = (fix16_from_int(gameDisplay->height) - (tempY + (tempCos * 16)));
	gfx_tri_fill_draw(fix16_to_int(tempPointsX[0]), fix16_to_int(tempPointsY[0]), fix16_to_int(tempPointsX[1]), fix16_to_int(tempPointsY[1]), fix16_to_int(tempPointsX[2]), fix16_to_int(tempPointsY[2]), gfx_color_rgb(0x7F, 0x7F, 0x7F));
	gfx_tri_draw(fix16_to_int(tempPointsX[0]), fix16_to_int(tempPointsY[0]), fix16_to_int(tempPointsX[1]), fix16_to_int(tempPointsY[1]), fix16_to_int(tempPointsX[2]), fix16_to_int(tempPointsY[2]), gfx_color_rgb(0xFF, 0xFF, 0xFF));
}

ship_level_point shipLevelCollision(ship* inShip, level* inLevel) {
	ship_level_point tempOut = { 0, -1, 0, -1, -1, false };
	if((inShip == NULL) || (inLevel == NULL))
		return tempOut;
	int32_t i;
	level_point tempPoint;
	for(i = (inShip->x - (4 << 16)); i <= (inShip->x + (4 << 16)); i += (1 << 16)) {
		tempPoint = levelHeight(inLevel, i);
		if(tempPoint.height > inShip->y) {
			if(tempOut.left_y < 0) {
				tempOut.left_x = i;
				tempOut.left_y = tempPoint.height;
			}
			tempOut.right_x = i;
			tempOut.right_y = tempPoint.height;
		}
	}
	tempPoint = levelHeightMax(inLevel, (inShip->x - (4 << 16)), (8 << 16));
	tempOut.high = tempPoint.height;
	tempOut.land = tempPoint.land;
	return tempOut;
}

bool shipDamage(ship* inShip, int32_t inVelocity) {
	if(inShip == NULL)
		return 0;

	if(inVelocity < 0)
		inVelocity = -inVelocity;

	uint32_t tempDamage = (inVelocity >> 12);
	if(inShip->hp <= tempDamage) {
		inShip->hp = 0;
		return false;
	}
	inShip->hp -= tempDamage;
	return true;
}

bool shipTick(ship* inShip, level* inLevel) {
	if(inShip == NULL)
		return false;

	bool tempOut = false;

	int32_t  tempGravity  = (inLevel == NULL ? (1 << 12) : inLevel->gravity);
	int32_t  tempWind     = 0;
	fract_t  tempFriction = fract_create(127, 128);

	if(control_check(CONTROL_DPAD_LEFT).pressed)
		inShip->angle -= (fix16_pi >> 6);
	if(control_check(CONTROL_DPAD_RIGHT).pressed)
		inShip->angle += (fix16_pi >> 6);

	fix16_t tempSin = fix16_sin(inShip->angle);
	fix16_t tempCos = fix16_cos(inShip->angle);

	if(control_check(CONTROL_TRIGGER_LEFT).pressed) {
		if(inShip->fuel >= 1) {
			inShip->fuel--;
			inShip->vel_x -= fix16_mul(tempCos, inShip->thrust_x);
			inShip->vel_y += fix16_mul(tempSin, inShip->thrust_x);
		}
	}
	if(control_check(CONTROL_TRIGGER_RIGHT).pressed) {
		if(inShip->fuel >= 1) {
			inShip->fuel--;
			inShip->vel_x += fix16_mul(tempCos, inShip->thrust_x);
			inShip->vel_y -= fix16_mul(tempSin, inShip->thrust_x);
		}
	}

	if((inShip->fuel >= 256) && control_just_pressed(CONTROL_BUTTON_B)) {
		inShip->fuel -= 256;
		inShip->vel_x += fix16_mul(tempSin, inShip->thrust_y) << 6;
		inShip->vel_y += fix16_mul(tempCos, inShip->thrust_y) << 6;
		inShip->thrust_visible = 32;
		sound_play(*_shipBoostSound, false);
	} else if((inShip->fuel >= 4) && (control_check(CONTROL_BUTTON_A).pressed)) {
		inShip->fuel -= 4;
		inShip->vel_x += fix16_mul(tempSin, inShip->thrust_y);
		inShip->vel_y += fix16_mul(tempCos, inShip->thrust_y);
		if(inShip->thrust_visible < 8)
			inShip->thrust_visible++;
		else if(inShip->thrust_visible > 8)
			inShip->thrust_visible--;
		if(_shipThrustLoop == 0)
			_shipThrustLoop = sound_play(*_shipThrustSound, true);
	} else {
		if(inShip->thrust_visible > 0)
			inShip->thrust_visible--;
		if(_shipThrustLoop != 0) {
			sound_stop(_shipThrustLoop);
			_shipThrustLoop = 0;
		}
	}

	inShip->vel_x += tempWind;
	inShip->vel_y -= tempGravity;
	inShip->vel_x  = fract_smul(inShip->vel_x, tempFriction);
	inShip->vel_y  = fract_smul(inShip->vel_y, tempFriction);

	inShip->x += inShip->vel_x;
	if((inShip->x - (4 << 16)) < 0) {
		inShip->x = (4 << 16);
		inShip->vel_x = 0;
	} if(inShip->x + (4 << 16) >= (int32_t)inLevel->width) {
		inShip->x = (inLevel->width - ((4 << 16) + 1));
		inShip->vel_x = 0;
	}
	ship_level_point tempHit = shipLevelCollision(inShip, inLevel);
	if(inShip->vel_x > 0) {
		if(tempHit.right_y > 0) {
			inShip->x = (tempHit.right_x - ((4 << 16) + 1));
			inShip->vel_x = (0 - fract_smul(inShip->vel_x, fract_create(1, 4)));
		}
	} else {
		if(tempHit.left_y > 0) {
			shipDamage(inShip, inShip->vel_x);
			inShip->x = (tempHit.left_x + ((4 << 16) + 1));
			inShip->vel_x = (0 - fract_smul(inShip->vel_x, fract_create(1, 4)));
		}
	}

	inShip->y += inShip->vel_y;
	tempHit = shipLevelCollision(inShip, inLevel);
	if(inShip->y < tempHit.high) {
		if(tempHit.land) {
			tempOut = true;
		} else {
			shipDamage(inShip, inShip->vel_y);
		}
		inShip->y = tempHit.high;
		inShip->vel_y = (0 - fract_smul(inShip->vel_y, fract_create(1, 4)));
	}
	return tempOut;
}

ship_land_score shipLandScore(ship* inShip, level* inLevel, uint32_t inLevelNo) {
	ship_land_score	tempOut = { 0, 0, 0, 0, 0, 0 };
	if((inShip == NULL) || (inLevel == NULL))
		return tempOut;

	sound_stop(0);
	_shipThrustLoop = 0;

	uint32_t tempSegWidth = (inLevel->width / (inLevel->points_count - 1));
	int32_t  tempTarget   = (inLevel->target_point * tempSegWidth) + ((inLevel->target_width * tempSegWidth) >> 1);
	tempOut.position  = (tempTarget > inShip->x ? tempTarget - inShip->x : inShip->x - tempTarget);
	tempOut.position *= 1000;
	tempOut.position /= (((inLevel->target_width * tempSegWidth) >> 1) - (4 << 16));
	tempOut.position = (tempOut.position >= 1000 ? 0 : 1000 - tempOut.position);

	tempOut.velocity = ((inShip->vel_y * 1000) >> 17);
	tempOut.velocity = (tempOut.velocity >= 1000 ? 0 : 1000 - tempOut.velocity);

	tempOut.fuel     = (((inShip->fuel_max - inShip->fuel) * 1000) / inShip->fuel_max);

	tempOut.time     = ((gameTime << 5) / timer_resolution);

	tempOut.level    = (25 + inLevelNo);
	if((tempOut.fuel + tempOut.time) >= (tempOut.position + tempOut.velocity))
		tempOut.total = 0;
	else
		tempOut.total = (uint32_t)(((tempOut.position + tempOut.velocity) - (tempOut.fuel + tempOut.time)) * tempOut.level);

	tempOut.total  *= gameDifficulty;
	tempOut.total >>= 1;

	return tempOut;
}

void shipLandScoreDraw(ship_land_score inScore) {
	char tempString[32];

	uint32_t tempWidth  = 106;
	uint32_t tempHeight = 112;
	uint32_t tempX = ((gameDisplay->width - tempWidth) >> 1);
	uint32_t tempY = ((gameDisplay->height - tempHeight) >> 1);

	gfx_rect_fill_draw(tempX, tempY, tempWidth, tempHeight, gfx_color_rgb(0x7F, 0x7F, 0x7F));
	gfx_rect_draw(tempX, tempY, tempWidth, tempHeight, gfx_color_rgb(0xFF, 0xFF, 0xFF));

	tempY += 8;
	gfx_font_print_center(tempY, gameFont, "Position:      ");
	hudDrawGuage(inScore.position, 1000, ((gameDisplay->width + gfx_font_width(gameFont, "Position: ") - gfx_font_width(gameFont, "+1000")) >> 1), tempY, gfx_font_width(gameFont, "+1000"), 8, gfx_color_rgb(0xFF, 0x00, 0x00), gfx_color_rgb(0xBF, 0x00, 0x00));

	tempY += 12;
	gfx_font_print_center(tempY, gameFont, "Velocity:      ");
	hudDrawGuage(inScore.velocity, 1000, ((gameDisplay->width + gfx_font_width(gameFont, "Position: ") - gfx_font_width(gameFont, "+1000")) >> 1), tempY, gfx_font_width(gameFont, "+1000"), 8, gfx_color_rgb(0xFF, 0x00, 0x00), gfx_color_rgb(0xBF, 0x00, 0x00));

	tempY += 12;
	gfx_font_print_center(tempY, gameFont, "Fuel:          ");
	hudDrawGuage((1000 - inScore.fuel), 1000, ((gameDisplay->width + gfx_font_width(gameFont, "Position: ") - gfx_font_width(gameFont, "+1000")) >> 1), tempY, gfx_font_width(gameFont, "+1000"), 8, gfx_color_rgb(0xFF, 0x00, 0x00), gfx_color_rgb(0xBF, 0x00, 0x00));

	tempY += 12;
	gfx_font_print_center(tempY, gameFont, "Time:          ");
	hudDrawGuage((1000 - inScore.time), 1000, ((gameDisplay->width + gfx_font_width(gameFont, "Position: ") - gfx_font_width(gameFont, "+1000")) >> 1), tempY, gfx_font_width(gameFont, "+1000"), 8, gfx_color_rgb(0xFF, 0x00, 0x00), gfx_color_rgb(0xBF, 0x00, 0x00));

	tempY += 12;
	sprintf(tempString, "Level:    x% 4lu", inScore.level);
	gfx_font_print_center(tempY, gameFont, tempString);

	tempY += 12;
	sprintf(tempString, "Diff:     x% 3lu%%", (50 * gameDifficulty));
	gfx_font_print_center(tempY, gameFont, tempString);

	tempY += 12;
	sprintf(tempString, "---------------");
	gfx_font_print_center(tempY, gameFont, tempString);

	tempY += 12;
	sprintf(tempString, "Total: +%lu", inScore.total);
	gfx_font_print_center(tempY, gameFont, tempString);

}
