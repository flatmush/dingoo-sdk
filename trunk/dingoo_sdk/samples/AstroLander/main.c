#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <dingoo/ucos2.h>
#include <dingoo/entry.h>

#include <sml/graphics.h>
#include <sml/display.h>
#include <sml/control.h>
#include <sml/timer.h>
#include <sml/fixmath.h>

#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)

#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_REVISION 0

typedef struct {
	uint32_t* points;
	uint32_t  points_count;
	uint32_t  target_point;
	uint32_t  target_width;
	uint32_t  width;
	uint32_t  start_time;
	int32_t   spawn_x, spawn_y;
	int32_t   gravity;
} level;

typedef struct {
	int32_t     x, y;
	int32_t     vel_x, vel_y;
	int32_t     angle;
	uint32_t    hp, hp_max;
	uint32_t    fuel, fuel_max;
	uint32_t    thrust_x, thrust_y;
	uint32_t    thrust_visible;
} ship;

typedef struct {
	int32_t*  star_points;
	int32_t   star_count;
	gfx_color sky_color;
} background;

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

typedef struct {
	int32_t height;
	bool    land;
} level_point;

typedef struct {
	uint32_t total;
	uint32_t level;
	uint32_t difficulty;
	char*    name;
} score;

typedef struct {
	score**   scores;
	uint32_t  count;
} score_list;



void  gameScreenshot();
void  gameDialog(char* inMessage);

background* backgroundCreate(uint16_t inStars, gfx_color inSkyColor);
void        backgroundDelete(background* inBackground);
void        backgroundDraw(background* inBackground);

level*      levelCreate(uint32_t inPoints, uint32_t inWidth, int32_t inGravity, uint8_t inSmooth);
void        levelDelete(level* inLevel);
void        levelDraw(level* inLevel, int32_t inOffX, int32_t inOffY);
level_point levelHeight(level* inLevel, int32_t inX);
level_point levelHeightMax(level* inLevel, int32_t inX, uint32_t inWidth);

ship*            shipCreate(int32_t inX, int32_t inY);
void             shipDelete(ship* inShip);
void             shipDraw(ship* inShip, int32_t inOffX, int32_t inOffY);
ship_level_point shipLevelCollision(ship* inShip, level* inLevel);
bool             shipDamage(ship* inShip, int32_t inVelocity);
bool             shipTick(ship* inShip, level* inLevel);
ship_land_score  shipLandScore(ship* inShip, level* inLevel, uint32_t inLevelNo);
void             shipLandScoreDraw(ship_land_score inScore);

score*      scoreCreate(uint32_t inTotal, uint32_t inLevel, uint32_t inDifficulty, const char* inName);
void        scoreDelete(score* inScore);
score_list* scoreListCreate();
void        scoreListDelete(score_list* inList);
bool        scoreListAdd(score_list* inList, score* inScore);
bool        scoreListSave(const char* inPath, score_list* inList);
void        scoreListDraw(score_list* inList, uint32_t inOffset);

void hudDrawGuage(uint32_t inCur, uint32_t inMax, int32_t inX, int32_t inY, uint32_t inWidth, uint32_t inHeight, gfx_color inBright, gfx_color inDark);
void hudDrawLives(uint32_t inLives, int32_t inX, int32_t inY, gfx_color inBright, gfx_color inDark);
void hudDraw(ship* inShip);

void gameRespawn();
void gameLevelNext();
void gameRestart();
void setShipDifficulty( ship *tempShip );

#define GAME_MODE_SPLASH 1
#define GAME_MODE_MENU   2
#define GAME_MODE_GAME   3

#define GAME_MODE_GAME_PLAY     1
#define GAME_MODE_GAME_PAUSE    2
#define GAME_MODE_GAME_LANDED   3
#define GAME_MODE_GAME_CRASHED  4
#define GAME_MODE_GAME_GAMEOVER 5

#define GAME_DIFFICULTY_EASY   1
#define GAME_DIFFICULTY_NORMAL 2
#define GAME_DIFFICULTY_HARD   3
#define GAME_DIFFICULTY_ULTRA  4

gfx_texture* gameFont    = NULL;
gfx_texture* gameSplash  = NULL;

uint8_t         gameMode     = GAME_MODE_SPLASH;
uint8_t         gameModeSub  = GAME_MODE_GAME_PLAY;
ship_land_score gameLandingScore = { 0, 0, 0, 0, 0 , 0 };

uint32_t    gameTime       = 0;
uint32_t    gameLives      = 5;
uint32_t    gameScore      = 0;
uint32_t    gameLevelNo    = 1;
uint32_t    gameDifficulty = GAME_DIFFICULTY_EASY;
char	stringDifficulty[7] = " EASY ";

level*      gameLevel      = NULL;
ship*       gameShip       = NULL;
background* gameBackground = NULL;
score_list* gameScores     = NULL;
uintptr_t   gameScoreListOffset = 0;

display* gameDisplay  = NULL;
timer*   gameTimer    = NULL;
uint32_t gameTickRate = (timer_resolution / 30);
bool     gameRunning  = true;


/* read the current difficulty and configure the ship as such -- don't worry
	about the point system rewarding easy players... a difficulty-based
	multiplier is applied to total points later. */
void setShipDifficulty( ship *tempShip )
{
	uint32_t hp_max = 0, fuel_max = 0;
	
	switch( gameDifficulty )
	{
		case GAME_DIFFICULTY_EASY:
			hp_max = 200; fuel_max = 2000;
			break;
		case GAME_DIFFICULTY_NORMAL:
			hp_max = 100; fuel_max = 1200;
			break;
		case GAME_DIFFICULTY_HARD:
			hp_max = 75; fuel_max = 800;
			break;
		case GAME_DIFFICULTY_ULTRA: /* it's called "ultra" for a reason. ;) */
			hp_max = 25; fuel_max = 400;
			break;
	}
	
	tempShip->hp_max = hp_max;
	tempShip->hp     = hp_max;

	tempShip->fuel_max = fuel_max;
	tempShip->fuel     = fuel_max;

	return;
}

void gameScreenshot() {
	char tempString[256];
	unsigned long int tempNumber = 0;
	FILE* tempFile;
	while(true) {
		sprintf(tempString, "screenshot%lu.tga", tempNumber);
		tempFile = fopen(tempString, "rb");
		if(tempFile == NULL)
			break;
		fclose(tempFile);
		tempNumber++;
	}
	gfx_tex_save_tga(tempString, gfx_render_target);
}

void gameDialog(char* inMessage) {
	int tempX, tempY;
	int tempWidth, tempHeight;
	tempWidth = (gfx_font_width(gameFont, inMessage) + 16);
	tempWidth = max(tempWidth, (gfx_font_width(gameFont, "Press [A/B]") + 16));
	tempHeight = ((gfx_font_height(gameFont) * 3) + 16);
	tempX = (gameDisplay->width - tempWidth) >> 1;
	tempY = (gameDisplay->height - tempHeight) >> 1;

	control_lock(timer_resolution / 2);

	while(gameRunning) {
		gfx_rect_fill_draw(tempX, tempY, tempWidth, tempHeight, gfx_color_rgb(0x7F, 0x7F, 0x7F));
		gfx_rect_draw(tempX, tempY, tempWidth, tempHeight, gfx_color_rgb(0xFF, 0xFF, 0xFF));

		gfx_font_print_center((tempY + 8), gameFont, inMessage);
		gfx_font_print_center((tempY + (2 * gfx_font_height(gameFont)) + 8), gameFont, "Press [A/B]");

		display_flip(gameDisplay);

		control_poll();
		if(control_check(CONTROL_BUTTON_B).pressed && control_check(CONTROL_BUTTON_B).changed)
			break;
		if(control_check(CONTROL_BUTTON_A).pressed && control_check(CONTROL_BUTTON_A).changed)
			break;
		if(control_check(CONTROL_BUTTON_SELECT).pressed && control_check(CONTROL_BUTTON_SELECT).changed) {
			gameScreenshot();
			gameDialog("Screenshot taken.");
		}
	}
}



background* backgroundCreate(uint16_t inStars, gfx_color inSkyColor) {
	background* tempBackground  = (background*)malloc(sizeof(background) + (inStars << 3));
	if(tempBackground == NULL)
		return NULL;
	tempBackground->sky_color   = inSkyColor;
	tempBackground->star_count  = inStars;
	tempBackground->star_points = (int32_t*)((uintptr_t)tempBackground + sizeof(background));
	uintptr_t i;
	for(i = 0; i < tempBackground->star_count; i++) {
		tempBackground->star_points[(i << 1) + 0] = (rand() % gameDisplay->width);
		tempBackground->star_points[(i << 1) + 1] = (rand() % gameDisplay->height);
	}
	return tempBackground;
}

void backgroundDelete(background* inBackground) {
	free(inBackground);
}

void backgroundDraw(background* inBackground) {
	if(inBackground == NULL)
		return;
	gfx_render_target_clear(inBackground->sky_color);
	uintptr_t i;
	for(i = 0; i < inBackground->star_count; i++)
		gfx_point_draw(inBackground->star_points[(i << 1) + 0], inBackground->star_points[(i << 1) + 1], gfx_color_rgb(0xFF, 0xFF, 0xFF));
}



uint8_t _rand8() {
	uint32_t tempRand = rand();
	return ((tempRand >> 24) ^ (tempRand >> 16) ^ (tempRand >> 8) ^ tempRand);
}

level* levelCreate(uint32_t inPoints, uint32_t inWidth, int32_t inGravity, uint8_t inSmooth) {
	if(inWidth < (gameDisplay->width << 16))
		inWidth = (gameDisplay->width << 16);

	level* tempLevel = (level*)malloc(sizeof(level) + ((inPoints + 1) << 2));
	if(tempLevel == NULL)
		return NULL;
	tempLevel->points = (uint32_t*)((uintptr_t)tempLevel + sizeof(level));
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
	if((inX < 0) || (inX >= inLevel->width))
		return tempOut;
	uint32_t tempSegWidth = (inLevel->width / (inLevel->points_count - 1));
	uint32_t tempIndex = (inX / tempSegWidth);
	tempOut.land = ((tempIndex >= inLevel->target_point) && (tempIndex < (inLevel->target_point + inLevel->target_width)));
	fract_t tempFract = fract_create((inX % tempSegWidth), tempSegWidth);
	tempOut.height = fract_smul(inLevel->points[tempIndex], fract_invert(tempFract));
	tempOut.height += fract_smul(inLevel->points[tempIndex + 1], tempFract);
	return tempOut;
}

level_point levelHeightMax(level* inLevel, int32_t inX, uint32_t inWidth) {
	level_point tempOut = { 0, false };
	if(inLevel == NULL)
		return tempOut;
	if(inX < 0) {
		if(inWidth < -inX)
			return tempOut;
		inWidth += inX;
		inX = 0;
	}
	if((inX + inWidth) >= inLevel->width) {
		if(inX > inLevel->width)
			return tempOut;
		inWidth = (inLevel->width - (inX + 1));
	}

	uint32_t tempSegWidth = (inLevel->width / (inLevel->points_count - 1));
	uint32_t tempIndex = (inX / tempSegWidth);
	if((tempIndex * tempSegWidth) < inX)
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

	setShipDifficulty( tempShip );

	tempShip->thrust_y = (1 << 13);
	tempShip->thrust_x = (tempShip->thrust_y >> 1);

	tempShip->thrust_visible = 0;

	return tempShip;
}

void shipDelete(ship* inShip) {
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
	ship_level_point tempOut = { 0, -1, 0, -1, -1 };
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

	if((inShip->fuel >= 256) && control_check(CONTROL_BUTTON_B).pressed && control_check(CONTROL_BUTTON_B).changed) {
		inShip->fuel -= 256;
		inShip->vel_x += fix16_mul(tempSin, inShip->thrust_y) << 6;
		inShip->vel_y += fix16_mul(tempCos, inShip->thrust_y) << 6;
		inShip->thrust_visible = 32;
	} else if((inShip->fuel >= 4) && (control_check(CONTROL_BUTTON_A).pressed)) {
		inShip->fuel -= 4;
		inShip->vel_x += fix16_mul(tempSin, inShip->thrust_y);
		inShip->vel_y += fix16_mul(tempCos, inShip->thrust_y);
		if(inShip->thrust_visible < 8)
			inShip->thrust_visible++;
		else if(inShip->thrust_visible > 8)
			inShip->thrust_visible--;
	} else {
		if(inShip->thrust_visible > 0)
			inShip->thrust_visible--;
	}

	inShip->vel_x += tempWind;
	inShip->vel_y -= tempGravity;
	inShip->vel_x  = fract_smul(inShip->vel_x, tempFriction);
	inShip->vel_y  = fract_smul(inShip->vel_y, tempFriction);

	inShip->x += inShip->vel_x;
	if((inShip->x - (4 << 16)) < 0) {
		inShip->x = (4 << 16);
		inShip->vel_x = 0;
	} if(inShip->x + (4 << 16) >= inLevel->width) {
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

	tempOut.total >>= (2 / gameDifficulty);
	
	return tempOut;
}

void shipLandScoreDraw(ship_land_score inScore) {
	char tempString[32];

	uint32_t tempWidth  = 106;
	uint32_t tempHeight = 100;
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
	sprintf(tempString, "---------------");
	gfx_font_print_center(tempY, gameFont, tempString);

	tempY += 12;
	sprintf(tempString, "Total: +%lu", inScore.total);
	gfx_font_print_center(tempY, gameFont, tempString);

}



score* scoreCreate(uint32_t inTotal, uint32_t inLevel, uint32_t inDifficulty, const char* inName) {
	if(inName == NULL)
		return NULL;
	score* tempScore = (score*)malloc(sizeof(score) + 16);
	if(tempScore == NULL)
		return NULL;
	tempScore->total = inTotal;
	tempScore->level = inLevel;
	tempScore->difficulty = inDifficulty;
	tempScore->name = (char*)((uintptr_t)tempScore + sizeof(score));
	strncpy(tempScore->name, inName, 15);
	return tempScore;
}

void scoreDelete(score* inScore) {
	free(inScore);
}

score_list* scoreListCreate() {
	score_list* tempList = (score_list*)malloc(sizeof(score_list));
	if(tempList == NULL)
		return NULL;
	tempList->count = 0;
	tempList->scores = NULL;
	return tempList;
}

void scoreListDelete(score_list* inList) {
	if(inList == NULL)
		return;
	uint32_t i;
	for(i = 0; i < inList->count; i++)
		scoreDelete(inList->scores[i]);
	free(inList);
}

bool scoreListAdd(score_list* inList, score* inScore) {
	if((inList == NULL) || (inScore == NULL))
		return false;
	score** tempList;
	if(inList->scores == NULL)
		tempList = (score**)malloc((inList->count + 1) * sizeof(score*));
	else
		tempList = (score**)realloc(inList->scores, ((inList->count + 1) * sizeof(score*)));
	if(tempList == NULL)
		return false;
	inList->scores = tempList;

	uintptr_t i, j;
	for(i = 0; (i < inList->count) && (inList->scores[i]->total >= inScore->total); i++);
	for(j = inList->count; j > i; j--)
		inList->scores[j] = inList->scores[j - 1];
	inList->scores[i] = inScore;
	inList->count++;
	return true;
}

score_list* scoreListLoad(const char* inPath) {
	score_list* tempList = scoreListCreate();
	if(tempList == NULL)
		return NULL;

	if(inPath == NULL)
		return tempList;

	FILE* tempFile = fopen(inPath, "rb");
	if(tempFile == NULL)
		return tempList;

	uint32_t tempTotal;
	uint32_t tempDifficulty;
	uint32_t tempLevel;
	char     tempName[16];
	while(!feof(tempFile)) {
		fread(&tempTotal, 4, 1, tempFile);
		fread(&tempDifficulty, 4, 1, tempFile);
		fread(&tempLevel, 4, 1, tempFile);
		fread(tempName, 4, 4, tempFile);
		scoreListAdd(tempList, scoreCreate(tempTotal, tempLevel, tempDifficulty, tempName));
	}
	fclose(tempFile);
	return tempList;
}

bool scoreListSave(const char* inPath, score_list* inList) {
	if((inPath == NULL) || (inList == NULL))
		return false;

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	uintptr_t i;
	for(i = 0; i < inList->count; i++) {
		fwrite(&inList->scores[i]->total, 4, 1, tempFile);
		fwrite(&inList->scores[i]->difficulty, 4, 1, tempFile);
		fwrite(&inList->scores[i]->level, 4, 1, tempFile);
		fwrite(inList->scores[i]->name, 4, 4, tempFile);
	}
	fclose(tempFile);
	return true;
}

void scoreListDraw(score_list* inList, uint32_t inOffset) {
	if(inList == NULL)
		return;

	uint32_t tempWidth = (gfx_font_width(gameFont, "000. 0123456789ABCDE NORMAL 100 00000000") + 16);
	uint32_t tempHeight = ((gfx_font_height(gameFont) + 2) * 12) + 14;
	uint32_t tempX = (gameDisplay->width - tempWidth) >> 1;
	uint32_t tempY = (gameDisplay->height - tempHeight) >> 1;

	gfx_rect_fill_draw(tempX, tempY, tempWidth, tempHeight, gfx_color_rgb(0x7F, 0x7F, 0x7F));
	gfx_rect_draw(tempX, tempY, tempWidth, tempHeight, gfx_color_rgb(0xFF, 0xFF, 0xFF));

	tempY += 8;
	gfx_font_print_center(tempY, gameFont, "High Scores");
	tempY += (gfx_font_height(gameFont) + 2) << 1;

	uintptr_t i;
	char tempString[64];
	for(i = inOffset; (i < (inOffset + 10)) && (i < inList->count) ; i++) {
		switch(inList->scores[i]->difficulty) {
			case GAME_DIFFICULTY_EASY:
				sprintf(tempString, "% 3lu. %- 15s EASY   % 3lu % 8lu", (i + 1), inList->scores[i]->name, inList->scores[i]->level, inList->scores[i]->total);
				break;
			case GAME_DIFFICULTY_NORMAL:
				sprintf(tempString, "% 3lu. %- 15s NORMAL % 3lu % 8lu", (i + 1), inList->scores[i]->name, inList->scores[i]->level, inList->scores[i]->total);
				break;
			case GAME_DIFFICULTY_HARD:
				sprintf(tempString, "% 3lu. %- 15s HARD   % 3lu % 8lu", (i + 1), inList->scores[i]->name, inList->scores[i]->level, inList->scores[i]->total);
				break;
			case GAME_DIFFICULTY_ULTRA:
				sprintf(tempString, "% 3lu. %- 15s ULTRA  % 3lu % 8lu", (i + 1), inList->scores[i]->name, inList->scores[i]->level, inList->scores[i]->total);
				break;
		}
		gfx_font_print_center(tempY, gameFont, tempString);
		tempY += gfx_font_height(gameFont) + 2;
	}
}



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

void hudDrawTargetDir(ship* inShip, level* inLevel, int32_t inX, int32_t inY, gfx_color inBright, gfx_color inDark) {
	if((inShip == NULL) || (inLevel == NULL))
		return;

	inX -= 4;
	uint32_t tempSegWidth = inLevel->width / (inLevel->points_count - 1);
	uint32_t tempTargetStart = inLevel->target_point * tempSegWidth;
	uint32_t tempTargetEnd   = tempTargetStart + (inLevel->target_width * tempSegWidth);
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
	hudDrawTargetDir(gameShip, gameLevel, (gameDisplay->width >> 1), 32, gfx_color_rgb(0x00, 0x00, 0xFF), gfx_color_rgb(0x00, 0x00, 0xBF));

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
	gfx_font_print( ((gameDisplay->width / 2) - 3),
		(gameDisplay->height - gfx_font_height(gameFont) - 2), gameFont,
		stringDifficulty);
		
	/* print the current altitude in the top-middle of the screen */
	sprintf( tempString, "Alt:   %d", (inShip->y + 20000)/1000 ); /* adjust alt to be more realistic */
	gfx_font_print((gameDisplay->width - score_hud_width), 28, gameFont, tempString);
}



void gameRespawn() {
	shipDelete(gameShip);

	int32_t tempSpawnX = (gameDisplay->width << 15);
	int32_t tempSpawnY = (gameDisplay->height << 15);
	if(gameLevel != NULL) {
		tempSpawnX = gameLevel->spawn_x;
		tempSpawnY = gameLevel->spawn_y;
	}
	gameShip = shipCreate(tempSpawnX, tempSpawnY);
	setShipDifficulty( gameShip );
	gameTime = 0;
}

void gameLevelNext() {
	srand(rand() + OSTimeGet());
	gameLevelNo++;
	levelDelete(gameLevel);
	gameLevel = levelCreate((64 + (gameLevelNo << 1)), (gameDisplay->width << 17), ((1 << 12) + (gameLevelNo << 4)), (224 - gameLevelNo));
	backgroundDelete(gameBackground);
	gameBackground = backgroundCreate(((gameDisplay->width * gameDisplay->height) / 900), gfx_color_rgb(0x00, 0x00, 0x1F));
	gameRespawn();
}

void gameRestart() {
	gameLives   = 5;
	gameScore   = 0;
	gameLevelNo = 0;
	gameLevelNext();
}

/* returns the next highest difficult than the currently selected difficulty,
	or EASY if the current difficulty is set to ULTRA. */
void changeDifficulty( void )
{
	/* determine the next difficulty and convert to a string */
	switch( gameDifficulty )
	{
		case GAME_DIFFICULTY_EASY:
			gameDifficulty = GAME_DIFFICULTY_NORMAL;
			strcpy( stringDifficulty, "NORMAL" );
			break;
		case GAME_DIFFICULTY_NORMAL:
			gameDifficulty = GAME_DIFFICULTY_HARD;
			strcpy( stringDifficulty, " HARD " );
			break;
		case GAME_DIFFICULTY_HARD:
			gameDifficulty = GAME_DIFFICULTY_ULTRA;
			strcpy( stringDifficulty, " ULTRA" );
			break;
		case GAME_DIFFICULTY_ULTRA:
			gameDifficulty = GAME_DIFFICULTY_EASY;
			strcpy( stringDifficulty, " EASY " );
			break;
	}
	
	/* force a restart of the game so the high score board makes sense */
	gameRestart();
	
	return;
}

int main(int argc, char** argv) {
	int ref = EXIT_SUCCESS;

	srand(OSTimeGet());

	control_init();
	gameDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_BUFFER_STATIC), NULL, NULL);
	//gameDisplay = display_create(640, 480, 640, (DISPLAY_FORMAT_RGB565 | DISPLAY_STRETCH | DISPLAY_FILTER_LINEAR), NULL, NULL);
	if(gameDisplay == NULL) {
		control_term();
		return ref;
	}
	gfx_init(gameDisplay);

	gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	display_flip(gameDisplay);
	control_lock(timer_resolution / 4);

	char tempString[256];

	gameFont   = gfx_tex_load_tga("font.tga");
	gameSplash = gfx_tex_load_tga("splash.tga");
	gameScores = scoreListLoad("scores.dat");

	gameTimer   = timer_create();

	int32_t tempOffX = 0;
	int32_t tempOffY = 0;

	uint32_t tempTick  = 0;
	uint32_t tempPauseOS = 0;

	int sysref;
	while(gameRunning) {
		sysref = _sys_judge_event(NULL);
		if(sysref < 0) {
			ref = sysref;
			break;
		}

		tempTick += timer_delta(gameTimer);
		if(tempTick < gameTickRate) {
			tempPauseOS = (((gameTickRate - tempTick) * OS_TICKS_PER_SEC) / timer_resolution);
			if(tempPauseOS > 0) OSTimeDly(tempPauseOS);
			while(tempTick < gameTickRate)
				tempTick += timer_delta(gameTimer);
		}


		if(tempTick > (gameTickRate << 2))
			tempTick = 0; // HACK - Fixes timing glitch of unknown origin.
		while(tempTick >= gameTickRate) {
			tempTick -= gameTickRate;

			control_poll();
			if(control_check(CONTROL_BUTTON_START).pressed && control_check(CONTROL_BUTTON_SELECT).pressed) {
				gameRunning = false;
				break;
			}
			if(control_check(CONTROL_BUTTON_SELECT).pressed && control_check(CONTROL_BUTTON_SELECT).changed) {
				gameScreenshot();
				gameDialog("Screenshot taken.");
			}

			switch(gameMode) {
				case GAME_MODE_SPLASH:
					if((control_check(CONTROL_BUTTON_A).pressed && control_check(CONTROL_BUTTON_A).changed))
						gameMode = GAME_MODE_MENU;
					break;
				case GAME_MODE_MENU:
					gameRestart();
					gameMode = GAME_MODE_GAME;
					break;
				case GAME_MODE_GAME:
					switch(gameModeSub) {
						case GAME_MODE_GAME_PLAY:
							if(control_check(CONTROL_BUTTON_START).pressed && control_check(CONTROL_BUTTON_START).changed) {
								gameModeSub = GAME_MODE_GAME_PAUSE;
								break;
							}
							gameTime += gameTickRate;
							if(shipTick(gameShip, gameLevel)) {
								gameLandingScore = shipLandScore(gameShip, gameLevel, gameLevelNo);
								gameScore += gameLandingScore.total;
								gameModeSub = GAME_MODE_GAME_LANDED;
								control_lock(timer_resolution / 4);
								break;
							}
							if((gameShip == NULL) || (gameShip->hp == 0) || ((gameShip->fuel <= 1) && (gameShip->vel_y < (1 << 14)))) {
								gameModeSub = GAME_MODE_GAME_CRASHED;
							}
							break;
						case GAME_MODE_GAME_PAUSE:
							if(control_check(CONTROL_BUTTON_START).pressed && control_check(CONTROL_BUTTON_START).changed)
								gameModeSub = GAME_MODE_GAME_PLAY;
							else if(control_check(CONTROL_TRIGGER_LEFT).pressed && control_check(CONTROL_TRIGGER_LEFT).changed)
								/* quit if left shoulder button is pressed while paused */
								return 0;
							else if(control_check(CONTROL_TRIGGER_RIGHT).pressed
								&& control_check(CONTROL_TRIGGER_RIGHT).changed)
								/* cycle through difficulties if the right 
								shoulder button is pressed while paused */
								changeDifficulty();
								
							break;
						case GAME_MODE_GAME_LANDED:
							if(control_check(CONTROL_BUTTON_A).pressed && control_check(CONTROL_BUTTON_A).changed) {
								gameLevelNext();
								gameModeSub = GAME_MODE_GAME_PLAY;
							}
							break;
						case GAME_MODE_GAME_CRASHED:
							if(gameLives == 0) {
								shipDelete(gameShip);
								gameShip = NULL;
								scoreListAdd(gameScores, scoreCreate(gameScore, gameLevelNo, gameDifficulty, "Player"));
								gameModeSub = GAME_MODE_GAME_GAMEOVER;
								break;
							}
							gameLives--;
							gameRespawn();
							gameModeSub = GAME_MODE_GAME_PLAY;
							break;
						case GAME_MODE_GAME_GAMEOVER:
							if(gameScores != NULL) {
								if(control_check(CONTROL_DPAD_UP).pressed && control_check(CONTROL_DPAD_UP).changed) {
									if(gameScoreListOffset > 0)
										gameScoreListOffset--;
								}
								if(control_check(CONTROL_DPAD_DOWN).pressed && control_check(CONTROL_DPAD_DOWN).changed) {
									if((gameScoreListOffset + 9) <= gameScores->count)
										gameScoreListOffset++;
								}
							}
							if(control_check(CONTROL_BUTTON_A).pressed && control_check(CONTROL_BUTTON_A).changed) {
								gameRestart();
								gameModeSub = GAME_MODE_GAME_PLAY;
							}
							break;
					}
			}
		}

		switch(gameMode) {
			case GAME_MODE_SPLASH:
				gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
				gfx_tex_draw(((gameDisplay->width - gameSplash->width) >> 1), ((gameDisplay->height - gameSplash->height) >> 1), gameSplash);
				gfx_font_print_center(((gameDisplay->height + gameSplash->height) >> 1) + (gfx_font_height(gameFont) << 1), gameFont, "A Flatmush Game");
				sprintf(tempString, "Ver: %lu.%lu.%lu", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
				gfx_font_print_fromright((gameDisplay->width - 4), (gameDisplay->height - (gfx_font_height(gameFont) + 4)), gameFont, tempString);
				display_flip(gameDisplay);
				break;
			case GAME_MODE_GAME:
				if((gameShip != NULL) && (gameLevel != NULL)) {
					tempOffX = ((gameDisplay->width << 15) - gameShip->x);
					if(tempOffX > 0)
						tempOffX = 0;
					if(((gameDisplay->width << 16) - tempOffX) >= gameLevel->width)
						tempOffX = ((gameDisplay->width << 16) - gameLevel->width) + 1;
					tempOffY = ((gameDisplay->height << 15) - gameShip->y);
				}
				backgroundDraw(gameBackground);
				levelDraw(gameLevel, tempOffX, tempOffY);
				shipDraw(gameShip, tempOffX, tempOffY);
				hudDraw(gameShip);
				switch(gameModeSub) {
					case GAME_MODE_GAME_PLAY:
						break;
					case GAME_MODE_GAME_PAUSE:
						gfx_font_print_center(((gameDisplay->height - gfx_font_height(gameFont)) >> 1), gameFont, "- PAUSED -");
						break;
					case GAME_MODE_GAME_LANDED:
						shipLandScoreDraw(gameLandingScore);
						break;
					case GAME_MODE_GAME_CRASHED:
						break;
					case GAME_MODE_GAME_GAMEOVER:
						scoreListDraw(gameScores, gameScoreListOffset);
						break;
				}
				display_flip(gameDisplay);
				break;
		}
	}


	scoreListSave("scores.dat", gameScores);
	scoreListDelete(gameScores);

	levelDelete(gameLevel);
	timer_delete(gameTimer);
	free(gameSplash);
	free(gameFont);

	gfx_term();
	display_delete(gameDisplay);
	control_term();

	return 0;
}
