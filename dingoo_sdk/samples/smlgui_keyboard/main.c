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

#include <smlgui/vrtkey.h>

#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)

void  gameScreenshot();

gfx_font* fontDefault = NULL;

display* gameDisplay  = NULL;
timer*   gameTimer    = NULL;
uint32_t gameTime     = 0;
uint32_t gameTickRate = (timer_resolution / 50);
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



int main(int argc, char** argv) {
	int ref = EXIT_SUCCESS;

	srand(OSTimeGet());

	control_init();

	gameDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_FILTER_NEAREST | DISPLAY_BUFFER_STATIC), NULL, NULL);
	if(gameDisplay == NULL)
		return EXIT_FAILURE;

	gfx_init(gameDisplay);
	gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	gfx_render_target_swap();

	fontDefault = gfx_font_load("font.tga", COLOR_BLACK);

	gameTimer = timer_create();

	vrtkey_init(VRTKEY_MODE_WITHTEXTFIELD);
	//vrtkey_init(0, VRTKEY_MODE_BASIC);

	//vrtkey_config(VRTKEY_OPT_ENABLECANCEL, 0);
	//vrtkey_config(VRTKEY_OPT_SHOWTEXTFIELD, 0);

	char inputStr[20];
	inputStr[0] = '\0';

	uint32_t tempTick = 0;
	int sysref;
	while(gameRunning) {
		if(tempTick < gameTickRate) {
			OSTimeDly(((gameTickRate - tempTick) * OS_TICKS_PER_SEC) / timer_resolution);
			while(tempTick < gameTickRate)
				tempTick += timer_delta(gameTimer);
		}
		tempTick -= gameTickRate;
		gameTime += gameTickRate;

		sysref = _sys_judge_event(NULL);
		if(sysref < 0) {
			ref = sysref;
			break;
		}

		gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
		vrtkey_draw();
		gfx_font_print(5, 5, fontDefault, inputStr);
		gfx_render_target_swap();


	    control_poll();
		uint8_t vrtkey_ret = vrtkey_update();
		switch (vrtkey_ret)
		{
		case VRTKEY_ACCEPT:
			vrtkey_close(false);
			break;
		case VRTKEY_CANCEL:
			vrtkey_close(false);
			break;
		}

		if(!vrtkey_isopen() && control_check(CONTROL_BUTTON_Y).pressed && control_check(CONTROL_BUTTON_Y).changed)
		{
			vrtkey_open(inputStr, 20, false);
		}

		if(control_check(CONTROL_BUTTON_START).pressed && control_check(CONTROL_BUTTON_START).changed)
			gameRunning = false;
		if(control_check(CONTROL_BUTTON_SELECT).pressed && control_check(CONTROL_BUTTON_SELECT).changed) {
			gameScreenshot();
		}
	}

	vrtkey_term();
	timer_delete(gameTimer);
	gfx_font_delete(fontDefault);
	gfx_term();
	display_delete(gameDisplay);
	control_term();

	return 0;
}
