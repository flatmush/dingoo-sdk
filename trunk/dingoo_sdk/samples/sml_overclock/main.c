#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <dingoo/ucos2.h>
#include <dingoo/entry.h>

#include <dingoo/jz4740.h>
#include <jz4740/emc.h>
#include <jz4740/cpm.h>
#include <jz4740/cpu.h>

#include <sml/graphics.h>
#include <sml/display.h>
#include <sml/control.h>
#include <sml/timer.h>



gfx_font* appFont    = NULL;

display* appDisplay  = NULL;
timer*   appTimer    = NULL;
uint32_t appTickRate = (timer_resolution / 30);
bool     appRunning  = true;

uintptr_t _presets_count = 4;
uintptr_t _presets[4] = { 336000000, 360000000, 400000000, 430000000 };


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

	appFont = gfx_font_load("font.tga", COLOR_BLACK);
	appTimer = timer_create();

	char tempString[256];

	uint32_t tempTick  = 0;
	uint32_t tempPauseOS = 0;

	uintptr_t tempCore;
	uintptr_t tempMemory;
	cpu_clock_get(&tempCore, &tempMemory);

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
				emc_sdram_ctrl_reg_dump("emc_sdram_ctrl_reg.txt");
				emc_sdram_timings_dump("emc_sdram_timings.txt", tempMemory);
				cpm_pll_ctrl_dump("cpm_pll_ctrl_reg.txt");
				cpm_freq_dump("cpm_freqs.txt");
			}

			if(control_check(CONTROL_DPAD_UP).repeat) {
				uintptr_t i;
				for(i = 0; (i < _presets_count); i++) {
					if(tempCore < _presets[i]) {
						tempCore = _presets[i];
						break;
					}
				}
			}
			if(control_check(CONTROL_DPAD_DOWN).repeat) {
				intptr_t i;
				for(i = (_presets_count - 1); i >= 0; i--) {
					if(tempCore > _presets[i]) {
						tempCore = _presets[i];
						break;
					}
				}
			}

			if(control_check(CONTROL_DPAD_LEFT).repeat)
				tempCore = (tempCore == 1000000 ? 1000000 : tempCore - 1000000);
			if(control_check(CONTROL_DPAD_RIGHT).repeat)
				tempCore   = (tempCore == 500000000 ? 500000000 : tempCore + 1000000);

			if(control_just_pressed(CONTROL_BUTTON_A)) {
				cpu_clock_set(tempCore);
				cpu_clock_get(&tempCore, &tempMemory);
			}
			if(control_just_pressed(CONTROL_BUTTON_B))
				cpu_clock_get(&tempCore, &tempMemory);
		}


		gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
		gfx_font_print_center(((appDisplay->height >> 1) - 16), appFont, "Dingoo A320 Clocking");

		uintptr_t tempY = (appDisplay->height >> 1) - 32;

		gfx_font_print_center(tempY, appFont, "A320 Clocks"); tempY += 12;
		tempY += 12;

		sprintf(tempString, "Core:   < % 3lu MHz >", (tempCore / 1000000));
		gfx_font_print_center(tempY, appFont, tempString); tempY += 12;
		sprintf(tempString, "Memory:   % 3lu MHz  ", (tempMemory / 1000000));
		gfx_font_print_center(tempY, appFont, tempString); tempY += 12;

		tempY += 12;
		gfx_font_print_center(tempY, appFont, "[A: Set] [B: Reset]");

		gfx_font_print_fromright(appDisplay->width, (appDisplay->height - gfx_font_height(appFont)), appFont, "[QUIT]");
		gfx_font_print(0, (appDisplay->height - gfx_font_height(appFont)), appFont, "[DUMP]");

		display_flip(appDisplay);
	}

	timer_delete(appTimer);
	gfx_font_delete(appFont);

	gfx_term();
	display_delete(appDisplay);
	control_term();

	return ref;
}
