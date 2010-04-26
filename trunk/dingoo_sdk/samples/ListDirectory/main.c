#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <dingoo/fsys.h>
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

gfx_font* gameFont = NULL;

display* gameDisplay  = NULL;
timer*   gameTimer    = NULL;
uint32_t gameTickRate = (timer_resolution / 30);
bool     gameRunning  = true;
char     gamePath[256];

char* gamePathInit(const char* inPath) {
	uintptr_t i, j;
	for(i = 0, j = 0; inPath[i] != '\0'; i++) {
		if((inPath[i] == '\\') || (inPath[i] == '/'))
			j = i + 1;
	}
	strncpy(gamePath, inPath, j);
	gamePath[j] = '\0';
	return gamePath;
}

// If you are going to draw a file list during a loop,
// it might be better to load them into an array first.
void drawFiles()
{
	char tempString[256];
	fsys_file_info_t fData;

	sprintf(tempString, "%s*", gamePath);
	int ret = fsys_findfirst(tempString, FSYS_FIND_FILE, &fData); // change FSYS_FIND_FILE to FSYS_FIND_DIR to list folders

	if (ret != 0)
	{
		gfx_font_print(50, 20, gameFont, "No files found.");
		return;
	}

	int i = 0;
	do {
		gfx_font_print(50, 20 + i * 10, gameFont, fData.name);
		i++;
	} while (fsys_findnext(&fData) == 0);

	fsys_findclose(&fData);

	gfx_font_print(50, 220, gameFont, "Press A to quit");
}



uint8_t _rand8() {
	uint32_t tempRand = rand();
	return ((tempRand >> 24) ^ (tempRand >> 16) ^ (tempRand >> 8) ^ tempRand);
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

	gameFont = gfx_font_load("font.tga", COLOR_BLACK);

	drawFiles();
	display_flip(gameDisplay);

	int sysref;
	while(gameRunning) {
		sysref = _sys_judge_event(NULL);
		if(sysref < 0) {
			ref = sysref;
			break;
		}
		control_poll();

		if (control_just_pressed(CONTROL_BUTTON_A) || control_just_pressed(CONTROL_BUTTON_B) ||
			control_just_pressed(CONTROL_BUTTON_START) || control_just_pressed(CONTROL_BUTTON_SELECT))
		{
			gameRunning = false;
		}

		OSTimeDly(1);
	}

	free(gameFont);

	gfx_term();
	display_delete(gameDisplay);
	control_term();

	return 0;
}
