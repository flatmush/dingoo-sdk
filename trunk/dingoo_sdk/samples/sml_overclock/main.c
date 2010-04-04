#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <dingoo/ucos2.h>
#include <dingoo/entry.h>

#include <jz4740/jz4740.h>
#include <jz4740/emc.h>
#include <jz4740/cpm.h>

#include <sml/cpu.h>
#include <sml/graphics.h>
#include <sml/display.h>
#include <sml/control.h>
#include <sml/timer.h>



gfx_texture* appFont    = NULL;

display* appDisplay  = NULL;
timer*   appTimer    = NULL;
uint32_t appTickRate = (timer_resolution / 30);
bool     appRunning  = true;



int main(int argc, char** argv) {
	int ref = EXIT_SUCCESS;

	srand(OSTimeGet());

	control_init();
	appDisplay = display_create_fastest();
	if(appDisplay == NULL) {
		control_term();
		return ref;
	}
	gfx_init(appDisplay);

	gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	display_flip(appDisplay);
	control_lock(timer_resolution / 4);

	appFont = gfx_tex_load_tga("font.tga");
	appTimer = timer_create();

	char tempString[256];

	uint32_t tempTick  = 0;
	uint32_t tempPauseOS = 0;

	uintptr_t tempCore;
	uintptr_t tempMemory;
	cpu_clock_get(&tempCore, &tempMemory);
	tempCore   /= 1000000;
	tempMemory /= 1000000;

	uintptr_t tempMenu = 0;

	int sysref;
	while(appRunning) {
		sysref = _sys_judge_event(NULL);
		if(sysref < 0) {
			ref = sysref;
			break;
		}

		tempTick += timer_delta(appTimer);
		if(tempTick < appTickRate) {
			tempPauseOS = (((appTickRate - tempTick) * OS_TICKS_PER_SEC) / timer_resolution);
			if(tempPauseOS > 0) OSTimeDly(tempPauseOS);
			while(tempTick < appTickRate)
				tempTick += timer_delta(appTimer);
		}

		while(tempTick >= appTickRate) {
			tempTick -= appTickRate;

			control_poll();
			if(control_just_pressed(CONTROL_BUTTON_START)) {
				appRunning = false;
				break;
			}
			if(control_just_pressed(CONTROL_BUTTON_SELECT)) {
				emc_sdram_timings_dump("emc_sdram.txt");
				cpm_pll_ctrl_dump("cpm_pll.txt");
				cpm_freq_dump("cpm_freqs.txt");
				break;
			}

			if(control_check(CONTROL_DPAD_UP).repeat) {
				tempMenu = (tempMenu - 1) & 1;
				break;
			}
			if(control_check(CONTROL_DPAD_DOWN).repeat) {
				tempMenu = (tempMenu + 1) & 1;
				break;
			}

			if(control_check(CONTROL_DPAD_LEFT).repeat) {
				if(tempMenu == 0) tempCore   = (tempCore == 1 ? 1 : tempCore - 1);
				else              tempMemory = (tempMemory == 1 ? 1 : tempMemory - 1);
				break;
			}
			if(control_check(CONTROL_DPAD_RIGHT).repeat) {
				if(tempMenu == 0) tempCore   = (tempCore == 500 ? 500 : tempCore + 1);
				else              tempMemory = (tempMemory == 166 ? 166 : tempMemory + 1);
				break;
			}

			if(control_just_pressed(CONTROL_BUTTON_A)) {
				cpu_clock_set((tempCore * 1000000), (tempMemory * 1000000));
				cpu_clock_get(&tempCore, &tempMemory);
				tempCore   /= 1000000;
				tempMemory /= 1000000;
			}
			if(control_just_pressed(CONTROL_BUTTON_B)) {
				cpu_clock_get(&tempCore, &tempMemory);
				tempCore   /= 1000000;
				tempMemory /= 1000000;
			}
		}


		gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
		gfx_font_print_center(((appDisplay->height >> 1) - 16), appFont, "Dingoo A320 Clocking");

		uintptr_t tempY = (appDisplay->height >> 1) - 32;

		gfx_font_print_center(tempY, appFont, "A320 Clocks"); tempY += 12;
		tempY += 12;

		gfx_font_print_center((tempY + (tempMenu * 12)), appFont, "        <         >");

		sprintf(tempString, "Core:     % 3lu MHz  ", tempCore);
		gfx_font_print_center(tempY, appFont, tempString); tempY += 12;
		sprintf(tempString, "Memory:   % 3lu MHz  ", tempMemory);
		gfx_font_print_center(tempY, appFont, tempString); tempY += 12;

		tempY += 12;
		gfx_font_print_center(tempY, appFont, "[A: Set] [B: Reset]");

		gfx_font_print_fromright(appDisplay->width, (appDisplay->height - gfx_font_height(appFont)), appFont, "[QUIT]");
		gfx_font_print(0, (appDisplay->height - gfx_font_height(appFont)), appFont, "[DUMP]");

		display_flip(appDisplay);
	}

	timer_delete(appTimer);
	free(appFont);

	gfx_term();
	display_delete(appDisplay);
	control_term();

	return ref;
}
