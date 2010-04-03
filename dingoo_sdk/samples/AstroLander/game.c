#include "game.h"

#include <stdlib.h>
#include <stdio.h>

#include <sml/fixmath.h>
#include <sml/timer.h>
#include <sml/control.h>
#include <sml/graphics.h>
#include <sml/sound.h>



#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif



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
char        gameDifficultyStrings[4][32] = { "EASY", "MEDIUM", "HARD", "ULTRA" };

level*      gameLevel      = NULL;
ship*       gameShip       = NULL;
background* gameBackground = NULL;
score_list* gameScores     = NULL;
uintptr_t   gameScoreListOffset = 0;

display* gameDisplay  = NULL;
timer*   gameTimer    = NULL;
uint32_t gameTickRate = (timer_resolution / 30);
bool     gameRunning  = true;



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
		if(control_just_pressed(CONTROL_BUTTON_B))
			break;
		if(control_just_pressed(CONTROL_BUTTON_A))
			break;
		if(control_just_pressed(CONTROL_BUTTON_SELECT)) {
			gameScreenshot();
			gameDialog("Screenshot taken.");
		}
	}
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
	gameTime = 0;
}

/* returns the next highest difficult than the currently selected difficulty,
	or EASY if the current difficulty is set to ULTRA. */
void gameDifficultyNext() {
	/* determine the next difficulty */
	gameDifficulty = (gameDifficulty & 3) + 1;
	/* force a restart of the game so the high score board makes sense */
	gameRestart();
	return;
}

void gameDifficultyPrev() {
	gameDifficulty = ((gameDifficulty - 1) & 3);
	if(gameDifficulty == 0)
		gameDifficulty = 4;
	gameRestart();
	return;
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
