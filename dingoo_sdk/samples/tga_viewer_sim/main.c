#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <dirent.h>

#include <dingoo/ucos2.h>
#include <dingoo/entry.h>

#include <sml/graphics.h>
#include <sml/display.h>
#include <sml/control.h>
#include <sml/timer.h>

#include "font.h"

#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)

#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_REVISION 0

#define SCROLL_SPEED 4

#define SWITCH_PREVIOUS 0
#define SWITCH_NEXT     1



display*     appDisplay  = NULL;
bool         appRunning  = true;
gfx_texture* appImage    = NULL;
char		 appImagePath[256];
gfx_font*    appFont     = NULL;
uint32_t     appTickRate = (timer_resolution / 30);
timer*       appTimer;
char         errorString[256];
bool         error = false;
int16_t      offsetX = 0;
int16_t      offsetY = 0;

void splitPath(char* outPath, char* outFilename, char* inPath) {

	uint16_t i, j;
	for(i = 0, j = 0; inPath[i] != '\0'; i++) {
		if((inPath[i] == '/') || (inPath[i] == '\\'))
			j = i + 1;
	}

	memcpy(outPath, inPath, j);
	outPath[j] = '\0';
	
	strcpy(outFilename, &inPath[j]);
}

void unloadCurrentImage() {
	if (appImage == NULL)
		return;

	free(appImage);
	appImage = NULL;
}

void loadImage(char* path) {
	unloadCurrentImage();

	appImage = gfx_tex_load_tga(path);
	if (appImage == NULL) {
		error = true;
		sprintf(errorString, "Unable to load %s", path);
	} else {
		offsetX = ((appDisplay->width - appImage->width) >> 1);
		offsetY = ((appDisplay->height - appImage->height) >> 1);
	}
}

void switchImage(uint8_t direction) {
	unloadCurrentImage();

	char tempString[256];
	char path[256];
	char filename[256];
	splitPath(path, filename, appImagePath);

	fsys_file_info_t fData;

	sprintf(tempString, "%s*.tga", path);
	int ret = fsys_findfirst(tempString, FSYS_FIND_FILE, &fData);

	if (ret != 0)
	{
		sprintf(errorString, "No files found");
		error = true;
		return;
	}

	char firstFile[256];
	char prevFile[256];
	sprintf(firstFile, "%s", fData.name);
	// Special case, if the current image is the first one, and user wants to go to the previous,
	// use the last in the folder.
	bool useLast = false;
	if (strcmp(firstFile, filename) == 0 && direction == SWITCH_PREVIOUS) {
		useLast = true;
	}

	do {
		// If we found the current file while going to the previous, we're done
		if (!useLast && direction == SWITCH_PREVIOUS && strcmp(fData.name, filename) == 0) {
			sprintf(appImagePath, "%s%s", path, prevFile);
			loadImage(appImagePath);
			fsys_findclose(&fData);
			return;
		}

		// If prevFile was the match while we're going to the next, we're done
		if (direction == SWITCH_NEXT && strcmp(prevFile, filename) == 0) {
			sprintf(appImagePath, "%s%s", path, fData.name);
			loadImage(appImagePath);
			fsys_findclose(&fData);
			return;
		}

		sprintf(prevFile, "%s", fData.name);

	} while (fsys_findnext(&fData) == 0);

	fsys_findclose(&fData);

	if (useLast) {
		sprintf(appImagePath, "%s%s", path, prevFile);
		loadImage(appImagePath);
	} else { // Match is last file, direction next. Load first.
		sprintf(appImagePath, "%s%s", path, firstFile);
		loadImage(appImagePath);
	}	
}

int main(int argc, char** argv) {
	int ref = EXIT_SUCCESS;

	srand(OSTimeGet());

	control_init();
	appDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_BUFFER_STATIC), NULL, NULL);
	if(appDisplay == NULL) {
		control_term();
		return ref;
	}
	gfx_init(appDisplay);

	gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	display_flip(appDisplay);
	control_lock(timer_resolution / 4);

	
	appFont = gfx_font_load_from_buffer(font, fontSize, COLOR_BLACK);
	sprintf(appImagePath, "%s", argv[0]); // In SIM files argv[0] contains the target to be loaded.
	loadImage(appImagePath);

	appTimer = timer_create();

	uint32_t tempTick  = 0;
	uint32_t tempPauseOS = 0;

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
			if((control_check(CONTROL_BUTTON_START).pressed && control_check(CONTROL_BUTTON_SELECT).pressed) ||
				control_just_pressed(CONTROL_BUTTON_B)) {
				appRunning = false;
				break;
			}

			if (control_just_pressed(CONTROL_TRIGGER_LEFT)) {
				switchImage(SWITCH_PREVIOUS);
			}

			if (control_just_pressed(CONTROL_TRIGGER_RIGHT)) {
				switchImage(SWITCH_NEXT);
			}

			if (!error && appImage != NULL) {
				if (appImage->width > appDisplay->width) {
					if (control_check(CONTROL_DPAD_LEFT).repeat)
						offsetX += SCROLL_SPEED;

					if (control_check(CONTROL_DPAD_RIGHT).repeat)
						offsetX -= SCROLL_SPEED;

					if (offsetX > 0)
						offsetX = 0;
					if (offsetX < appDisplay->width - appImage->width)
						offsetX = appDisplay->width - appImage->width;
				}

				if (appImage->height > appDisplay->height) {
					if (control_check(CONTROL_DPAD_UP).repeat)
						offsetY += SCROLL_SPEED;

					if (control_check(CONTROL_DPAD_DOWN).repeat)
						offsetY -= SCROLL_SPEED;

					if (offsetY > 0)
						offsetY = 0;

					if (offsetY < appDisplay->height - appImage->height)
						offsetY = appDisplay->height - appImage->height;
				}
			}
		}
		
		gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));

		if (error)
			gfx_font_print_center(100, appFont, errorString);
		else if (appImage != NULL)
			gfx_tex_draw(offsetX, offsetY, appImage);

		display_flip(appDisplay);
	}

	timer_delete(appTimer); 

	gfx_font_delete(appFont);
	unloadCurrentImage();

	gfx_term();
	display_delete(appDisplay);
	control_term();

	return 0;
}
