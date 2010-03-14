#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <dingoo/fsys.h>
#include <dingoo/ucos2.h>
#include <dingoo/entry.h>

#include <sml/graphics.h>
#include <sml/sprite.h>
#include <sml/display.h>
#include <sml/control.h>
#include <sml/timer.h>
#include <sml/fixmath.h>

#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)

#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_REVISION 0


char* gamePathInit(const char* inPath);

gfx_texture* gameFont     = NULL;
display*     gameDisplay  = NULL;
timer*       gameTimer    = NULL;
uint32_t     gameTickRate = (timer_resolution / 30);
bool         gameRunning  = true;
char         gamePath[256];



char* gamePathInit(const char* inPath) {
	uintptr_t i, j;
	for(i = 0, j = 0; inPath[i] != '\0'; i++) {
		if((inPath[i] == '\\') || (inPath[i] == '/'))
			j = i + 1;
	}
	strncpy(gamePath, inPath, j);
	return gamePath;
}



int main(int argc, char** argv) {
	gamePathInit(argv[0]);
	int ref = EXIT_SUCCESS;

	srand(OSTimeGet());

	control_init();
	gameDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_BUFFER_STATIC), NULL, NULL);
	if(gameDisplay == NULL) {
		control_term();
		return ref;
	}
	gfx_init(gameDisplay);

	gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	display_flip(gameDisplay);
	control_lock(timer_resolution / 4);

	char tempString[256];
	sprintf(tempString, "%sfont.tga", gamePath);
	gameFont = gfx_tex_load_tga(tempString);

	gfx_texture* tempSpriteBmp[4];
	sprite* tempSprite;
	uintptr_t i;

	sprintf(tempString, "%ssprite.spt", gamePath);
	FSYS_FILE* tempFile = fsys_fopen(tempString, "rb");
	if(tempFile == NULL) {
		for(i = 0; i < 4; i++) {
			sprintf(tempString, "%ssprite%i.tga", gamePath, i);
			tempSpriteBmp[i] = gfx_tex_load_tga(tempString);
		}
		tempSprite = sprite_create(tempSpriteBmp[0]->width, tempSpriteBmp[0]->height);
		for(i = 0; i < 4; i++) {
			tempSprite = sprite_frame_add_bitmap(tempSprite, tempSpriteBmp[i]->address, gfx_color_rgb(0xFF, 0x00, 0xFF));
			free(tempSpriteBmp[i]);
		}
		sprintf(tempString, "%ssprite.spt", gamePath);
		sprite_save(tempSprite, tempString);
	} else {
		fsys_fclose(tempFile);
		tempSprite = sprite_load(tempString);
	}

	gameTimer = timer_create();

	uint32_t tempTick    = 0;
	uint32_t tempPauseOS = 0;
	uint8_t  tempFrame   = 0;

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
			if(control_check(CONTROL_BUTTON_A).pressed && control_check(CONTROL_BUTTON_A).changed) {
				tempFrame++;
				tempFrame &= 0x3;
			}

			gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
			sprite_draw(tempSprite, ((gameDisplay->width - tempSprite->width) >> 1), ((gameDisplay->height - tempSprite->height) >> 1), tempFrame);
			gfx_font_print_center(((gameDisplay->height + tempSprite->height) >> 1) + (gfx_font_height(gameFont) << 1), gameFont, "Sprite Test");
			sprintf(tempString, "Ver: %lu.%lu.%lu", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
			gfx_font_print_fromright((gameDisplay->width - 4), (gameDisplay->height - (gfx_font_height(gameFont) + 4)), gameFont, tempString);
			display_flip(gameDisplay);
		}
	}

	timer_delete(gameTimer);
	free(gameFont);

	gfx_term();
	display_delete(gameDisplay);
	control_term();

	return 0;
}
