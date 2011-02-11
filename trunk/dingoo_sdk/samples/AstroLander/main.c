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
#include <sml/sound.h>

#ifdef FGL_BONUS_GRAPHICS
#include <fgl/fgl.h>
#endif

#include "game.h"
#include "ship.h"
#include "level.h"
#include "hud.h"
#include "background.h"
#include "scoreList.h"

//#include "font.h"

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_REVISION 0



int main(int argc, char** argv) {
	int ref = EXIT_SUCCESS;

	srand(OSTimeGet());

	control_init();
	gameDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_BUFFER_STATIC), NULL, NULL);
	if(gameDisplay == NULL) {
		control_term();
		return ref;
	}
	gfx_init(gameDisplay);
	sound_init();
	gameVolume = sound_volume_set(gameVolume);


	#ifdef FGL_BONUS_GRAPHICS
	// Initialize fgl
	gameDisplayTex = (fgl_texture*)malloc(sizeof(fgl_texture));
	if(gameDisplayTex == NULL) {
		control_term();
		display_delete(gameDisplay);
		gfx_term();
		return ref;
	}
	gameDisplayTex->width    = gameDisplay->stride;
	gameDisplayTex->height   = gameDisplay->height;
	gameDisplayTex->reserved = 0;
	gameDisplayTex->data     = gameDisplay->buffer;
	gameDisplayTex->mipmap   = NULL;
	fgl_draw_buffer_set(gameDisplayTex);

	fgl_matrix_mode_set(FGL_MATRIX_VIEWPORT);
	fgl_matrix_identity();
	fgl_viewport(0, 0, fgl_fix16_from_int(gameDisplay->width), fgl_fix16_from_int(gameDisplay->height));
	fgl_matrix_mode_set(FGL_MATRIX_PROJECTION);
	fgl_matrix_identity();
	fgl_perspective((fgl_fix16_pi >> 3), fgl_fix16_div(fgl_fix16_from_int(gameDisplay->width), fgl_fix16_from_int(gameDisplay->height)), fgl_fix16_one, fgl_fix16_from_int(1024));
	fgl_matrix_mode_set(FGL_MATRIX_VIEW);
	fgl_matrix_identity();
	fgl_matrix_mode_set(FGL_MATRIX_MODEL);
	fgl_matrix_identity();

	fgl_disable(FGL_DEPTH_TEST);
	fgl_disable(FGL_DEPTH_WRITE);
	fgl_enable(FGL_CULL_FACE);
	fgl_enable(FGL_TEXTURE_2D);
	fgl_clear(FGL_COLOR_BUFFER_BIT);
	#else
	gfx_render_target_clear(COLOR_BLACK);
	#endif



	display_flip(gameDisplay);
	control_lock(timer_resolution / 4);

	char tempString[256];

	gameFont   = gfx_font_load("font.tga", COLOR_BLACK);
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

		while(tempTick >= gameTickRate) {
			tempTick -= gameTickRate;

			control_poll();
			if(control_check(CONTROL_BUTTON_START).pressed && control_check(CONTROL_BUTTON_SELECT).pressed) {
				gameRunning = false;
				break;
			}
			if(control_just_pressed(CONTROL_BUTTON_SELECT)) {
				gameScreenshot();
				gameDialog("Screenshot taken.");
			}

			if(gameVolumeShow > 0)
				gameVolumeShow--;

			switch(gameMode) {
				case GAME_MODE_SPLASH:
					if(control_just_pressed(CONTROL_BUTTON_A))
						gameMode = GAME_MODE_MENU;
					break;
				case GAME_MODE_MENU:
					gameRestart();
					gameMode = GAME_MODE_GAME;
					break;
				case GAME_MODE_GAME:
					switch(gameModeSub) {
						case GAME_MODE_GAME_PLAY:
							if(control_just_pressed(CONTROL_BUTTON_START)) {
								gameModeSub = GAME_MODE_GAME_PAUSE;
								break;
							}
							gameTime += gameTickRate;
							backgroundTick(gameBackground);
							if(shipTick(gameShip, gameLevel)) {
								gameLandingScore = shipLandScore(gameShip, gameLevel, gameLevelNo);
								gameScore += gameLandingScore.total;
								gameModeSub = GAME_MODE_GAME_LANDED;
								control_lock(timer_resolution / 4);
								break;
							}
							if((gameShip == NULL) || gameShip->dead || ((gameShip->fuel <= 1) && (gameShip->avg_vel < (1 << 14)))) {
								gameModeSub = GAME_MODE_GAME_CRASHED;
							}
							break;
						case GAME_MODE_GAME_PAUSE:
							if(control_just_pressed(CONTROL_BUTTON_START))
								gameModeSub = GAME_MODE_GAME_PLAY;
							else {
								if(control_just_pressed(CONTROL_TRIGGER_LEFT))
									gameDifficultyPrev(); // cycle through difficulties if the right shoulder button is pressed while paused
								if(control_just_pressed(CONTROL_TRIGGER_RIGHT))
									gameDifficultyNext(); // cycle through difficulties if the right shoulder button is pressed while paused
								if(control_just_pressed(CONTROL_DPAD_UP)) {
									gameVolume = sound_volume_set(gameVolume + 3);
									gameVolumeShow = 15;
								}
								if(control_just_pressed(CONTROL_DPAD_DOWN)) {
									gameVolume = sound_volume_set(gameVolume >= 3 ? (gameVolume - 3) : 0);
									gameVolumeShow = 15;
								}
							}
							break;
						case GAME_MODE_GAME_LANDED:
							if(control_just_pressed(CONTROL_BUTTON_A)) {
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
				#ifdef FGL_BONUS_GRAPHICS
				fgl_clear(FGL_COLOR_BUFFER_BIT);
				#else
				gfx_render_target_clear(COLOR_BLACK);
				#endif
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
					if((((uint32_t)gameDisplay->width << 16) - tempOffX) >= gameLevel->width)
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
						if(gameVolumeShow > 0) {
							sprintf(tempString, "Volume: % 3lu", gameVolume);
							gfx_font_print_center((((gameDisplay->height - gfx_font_height(gameFont)) >> 1) + 12), gameFont, tempString);
						}
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
	gfx_tex_delete(gameSplash);
	gfx_font_delete(gameFont);

	sound_term();
	gfx_term();
	display_delete(gameDisplay);
	control_term();

	return 0;
}
