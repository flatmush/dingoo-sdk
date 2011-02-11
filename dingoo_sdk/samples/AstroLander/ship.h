#ifndef __ship_h__
#define __ship_h__

#include <stdbool.h>
#include <stdint.h>

#include "particle.h"


typedef struct {
	int32_t     x, y;
	int32_t     vel_x, vel_y;
	int32_t     angle;

	uint32_t    avg_vel;

	uint32_t    hp, hp_max;
	uint32_t    fuel, fuel_max;

	uint32_t    thrust_x, thrust_y;
	int32_t     thrust_power;

	bool        exploding;
	bool        dead;

	particle_system* partsys;
} ship;

typedef struct {
	int32_t left_x, left_y, right_x, right_y, high;
	bool    land;
} ship_level_point;

typedef struct {
	uint32_t velocity;
	uint32_t position;
	uint32_t fuel;
	uint32_t time;
	uint32_t level;
	uint32_t total;
} ship_land_score;



#include "level.h"



extern ship*            shipCreate(int32_t inX, int32_t inY);
extern void             shipDelete(ship* inShip);
extern void             shipDraw(ship* inShip, int32_t inOffX, int32_t inOffY);
extern ship_level_point shipLevelCollision(ship* inShip, level* inLevel);
extern bool             shipDamage(ship* inShip, int32_t inVelocity);
extern bool             shipTick(ship* inShip, level* inLevel);
extern ship_land_score  shipLandScore(ship* inShip, level* inLevel, uint32_t inLevelNo);
extern void             shipLandScoreDraw(ship_land_score inScore);

#endif
