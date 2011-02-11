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

gfx_color _shipParticleColor[64];

void _shipParticleColorInit() {
	uintptr_t i, j;

	for(i = 0, j = 0; i < 16; i++, j++) // Blue->White
		_shipParticleColor[i] = gfx_color_rgb((j << 4), (j << 4), 0xFF);
	for(i = 16, j = 15; i < 32; i++, j--) // White->Yellow
		_shipParticleColor[i] = gfx_color_rgb(0xFF, 0xFF, (j << 4));
	for(i = 32, j = 15; i < 48; i++, j--) // Yellow->Red
		_shipParticleColor[i] = gfx_color_rgb(0xFF, (j << 4), 0x00);
	for(i = 48, j = 15; i < 64; i++, j--) // Red->Black
		_shipParticleColor[i] = (j << 12);
}


void _shipParticleDraw(particle* inParticle, int32_t inOffX, int32_t inOffY) {
	if(inParticle == NULL)
		return;

	int32_t tempX = fix16_to_int(inParticle->x + inOffX);
	int32_t tempY = fix16_to_int(fix16_from_int(gameDisplay->height) - (inParticle->y + inOffY));

	gfx_point_draw(tempX, tempY, _shipParticleColor[inParticle->age]);
}

bool _shipParticleTick(particle* inParticle) {
	if(inParticle == NULL)
		return false;

	inParticle->age++;
	fract32_t tempFriction = fract32_create(127, 128);
	inParticle->vel_y -= gameLevel->gravity;
	inParticle->vel_x = fract32_smul(inParticle->vel_x, tempFriction);
	inParticle->vel_y = fract32_smul(inParticle->vel_y, tempFriction);
	inParticle->x += inParticle->vel_x;
	inParticle->y += inParticle->vel_y;

	return (inParticle->age < 64);
}



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
	tempShip->avg_vel = 0;
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

	tempShip->thrust_power = 0;

	tempShip->exploding = false;
	tempShip->dead = false;

	if((_shipThrustRefCount == 0) || (_shipThrustSound == NULL)) {
		_shipThrustSound = wav_load("thrust.wav");
		_shipBoostSound  = wav_load("boost.wav");
	}
	_shipThrustRefCount++;

	tempShip->partsys = particle_system_create();
	if(tempShip->partsys != NULL) {
		tempShip->partsys->tick_func = _shipParticleTick;
		tempShip->partsys->draw_func = _shipParticleDraw;
	}

	_shipParticleColorInit();

	return tempShip;
}

void shipDelete(ship* inShip) {
	if(inShip == NULL)
		return;
	_shipThrustRefCount--;
	if(_shipThrustRefCount == 0) {
		sound_stop(0);
		_shipThrustLoop = 0;
		wav_delete(_shipThrustSound);
		_shipThrustSound = NULL;
		wav_delete(_shipBoostSound);
		_shipBoostSound = NULL;
	}
	if(inShip->partsys != NULL)
		particle_system_delete(inShip->partsys);
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

	if(inShip->partsys != NULL)
		particle_system_draw(inShip->partsys, inOffX, inOffY);

	if(!inShip->exploding) {
		tempPointsX[0] = (tempX - (tempCos * 4));
		tempPointsY[0] = (fix16_from_int(gameDisplay->height) - (tempY + (tempSin * 4)));
		tempPointsX[1] = (tempX + (tempCos * 4));
		tempPointsY[1] = (fix16_from_int(gameDisplay->height) - (tempY - (tempSin * 4)));
		tempPointsX[2] = (tempX + (tempSin * 16));
		tempPointsY[2] = (fix16_from_int(gameDisplay->height) - (tempY + (tempCos * 16)));
		gfx_tri_fill_draw(fix16_to_int(tempPointsX[0]), fix16_to_int(tempPointsY[0]), fix16_to_int(tempPointsX[1]), fix16_to_int(tempPointsY[1]), fix16_to_int(tempPointsX[2]), fix16_to_int(tempPointsY[2]), gfx_color_rgb(0x7F, 0x7F, 0x7F));
		gfx_tri_draw(fix16_to_int(tempPointsX[0]), fix16_to_int(tempPointsY[0]), fix16_to_int(tempPointsX[1]), fix16_to_int(tempPointsY[1]), fix16_to_int(tempPointsX[2]), fix16_to_int(tempPointsY[2]), gfx_color_rgb(0xFF, 0xFF, 0xFF));
	}
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
		fix16_t tempMagnitude = fix16_one;
		tempMagnitude += fix16_div(inShip->fuel, inShip->fuel_max);
		tempMagnitude += fix16_div((tempDamage - inShip->hp), inShip->hp_max);

		uintptr_t i;
		for(i = 0; i < (uint32_t)fix16_mul(1024, tempMagnitude); i++) {
			fix16_t tempAngle = fix16_mul((rand() & 0xFFFF), (1 << fix16_pi));
			fix16_t tempVel   = fix16_mul((rand() & 0xFFFF), tempMagnitude);

			fix16_t tempVelX = fix16_mul(tempVel, fix16_sin(tempAngle));
			fix16_t tempVelY = fix16_mul(tempVel, fix16_cos(tempAngle));

			particle_system_add(inShip->partsys, particle_create(0xFFFF, inShip->x, inShip->y, tempVelX, tempVelY));
		}

		inShip->exploding = true;
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

	if(!inShip->exploding) {
		int32_t  tempGravity  = (inLevel == NULL ? (1 << 12) : inLevel->gravity);
		int32_t  tempWind     = 0;
		fract32_t  tempFriction = fract32_create(127, 128);

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



		uint32_t tempFuelCost = fix16_mul(4, inShip->thrust_power);
		if(tempFuelCost > inShip->fuel) {
			inShip->thrust_power = fix16_div(inShip->fuel, 4);
			inShip->fuel = 0;
		} else
			inShip->fuel -= tempFuelCost;

		fix16_t tempThrustX = fix16_mul(fix16_mul(tempSin, inShip->thrust_y), inShip->thrust_power);
		fix16_t tempThrustY = fix16_mul(fix16_mul(tempCos, inShip->thrust_y), inShip->thrust_power);

		if((inShip->partsys != NULL) && (inShip->thrust_power > 0)) {
			uintptr_t tempPAge = 16;
			uintptr_t tempPCnt = ((inShip->thrust_power <= (fix16_one >> 1)) ? 4 : fix16_mul(8, inShip->thrust_power));
			fix16_t tempPVelX = inShip->vel_x - tempThrustX;
			fix16_t tempPVelY = inShip->vel_y - tempThrustY;

			uintptr_t i;
			for(i = 0; i < tempPCnt; i++) {
				fix16_t tempVelH = (0x7FFF - (rand() & 0xFFFF));
				fix16_t tempVelV = (0x3FFF - (rand() & 0x7FFF));
				fix16_t tempVelX = tempPVelX + tempVelH;
				fix16_t tempVelY = tempPVelY + tempVelV;
				particle_system_add(inShip->partsys, particle_create(tempPAge, inShip->x, inShip->y, tempVelX, tempVelY));
			}
		}

		inShip->vel_x += tempThrustX;
		inShip->vel_y += tempThrustY;

		if((inShip->thrust_power > 0) && (_shipThrustLoop == 0))
			_shipThrustLoop = sound_play(*_shipThrustSound, true);
		else if((inShip->thrust_power == 0) && (_shipThrustLoop != 0)){
			sound_stop(_shipThrustLoop);
			_shipThrustLoop = 0;
		}

		if(control_just_pressed(CONTROL_BUTTON_B))
			inShip->thrust_power = fix16_one << 5;
		else if(control_check(CONTROL_BUTTON_A).pressed && (inShip->thrust_power <= fix16_one))
			inShip->thrust_power = fix16_one;
		else if(inShip->thrust_power > 0)
			inShip->thrust_power = 0;



		inShip->vel_x += tempWind;
		inShip->vel_y -= tempGravity;
		inShip->vel_x  = fract32_smul(inShip->vel_x, tempFriction);
		inShip->vel_y  = fract32_smul(inShip->vel_y, tempFriction);

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
				inShip->vel_x = (0 - fract32_smul(inShip->vel_x, fract32_create(1, 4)));
			}
		} else {
			if(tempHit.left_y > 0) {
				shipDamage(inShip, inShip->vel_x);
				inShip->x = (tempHit.left_x + ((4 << 16) + 1));
				inShip->vel_x = (0 - fract32_smul(inShip->vel_x, fract32_create(1, 4)));
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
			inShip->vel_y = (0 - fract32_smul(inShip->vel_y, fract32_create(1, 4)));
		}

		uint32_t tempAvgVel = ((inShip->vel_x < 0 ? -inShip->vel_x : inShip->vel_x) + (inShip->vel_y < 0 ? -inShip->vel_y : inShip->vel_y));//fix16_sqrt((inShip->vel_x * inShip->vel_x) + (inShip->vel_y * inShip->vel_y));
		inShip->avg_vel = ((inShip->avg_vel * 15) + tempAvgVel) >> 4;
	} else {
		if((inShip->partsys == NULL) || (inShip->partsys->list == NULL))
			inShip->dead = true;
	}

	if(inShip->partsys != NULL)
		particle_system_tick(inShip->partsys);

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
