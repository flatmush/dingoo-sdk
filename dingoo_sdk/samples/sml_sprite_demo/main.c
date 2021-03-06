#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

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



gfx_font*    gameFont     = NULL;
display*     gameDisplay  = NULL;
timer*       gameTimer    = NULL;
uint32_t     gameTickRate = (timer_resolution / 30);
bool         gameRunning  = true;



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

	gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	display_flip(gameDisplay);
	control_lock(timer_resolution / 4);

	gameFont = gfx_font_load("font.tga", COLOR_BLACK);

	gfx_texture* tempSpriteBmp;
	sprite* tempSprite;
	uintptr_t i;

	char tempString[FILENAME_MAX];

	FILE* tempFile = fopen("sprite.spt", "rb");
	if(tempFile == NULL) {
		tempSprite = sprite_load_from_tga("sprite0.tga", gfx_color_rgb(0xFF, 0x00, 0xFF));
		if(tempSprite == NULL)
			return EXIT_SUCCESS;
		for(i = 1; true; i++) {
			sprintf(tempString, "sprite%i.tga", i);
			tempSpriteBmp = gfx_tex_load_tga(tempString);
			if(tempSpriteBmp == NULL)
				break;
			tempSprite = sprite_frame_add_bitmap(tempSprite, tempSpriteBmp->address, gfx_color_rgb(0xFF, 0x00, 0xFF));
			gfx_tex_delete(tempSpriteBmp);
		}
		sprite_save(tempSprite, "sprite.spt");
	} else {
		fclose(tempFile);
		tempSprite = sprite_load("sprite.spt");
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
				tempFrame %= tempSprite->frame_count;
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
	gfx_font_delete(gameFont);

	gfx_term();
	display_delete(gameDisplay);
	control_term();

	return 0;
}
