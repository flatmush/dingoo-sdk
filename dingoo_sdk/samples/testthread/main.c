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

#include "font.h"

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_REVISION 0

gfx_font* gameFont     = NULL;
display*  gameDisplay  = NULL;
timer*    gameTimer    = NULL;
uint32_t  gameTickRate = (timer_resolution / 30);
bool      gameRunning  = true;

int temppos = 10;
int tempdir = 1;

// Thread stuff
#define TASK_PRIO 5
#define TASK_STK_SIZE 4096
#define NO_TASKS 1
OS_STK   TaskStk[NO_TASKS][TASK_STK_SIZE];      // Tasks stacks 
OS_STK   TaskStartStk[TASK_STK_SIZE];
char     TaskData[NO_TASKS];                    // Parameters to pass to each task

static volatile int threadExit = 0;
static volatile int threadRunning = 0;
static volatile unsigned char tempChar = 0;
static volatile int threadReturnVal = -1;

void _thread(void *none)
{
	threadRunning = 1;

	do
	{
		tempChar++;
		OSTimeDly(1);
	} while (threadExit == 0); // Until the end of the thread

	threadRunning = 0;
	threadExit = 0;

	OSTaskDel(TASK_PRIO);
}

int main(int argc, char** argv) {

	int ref = EXIT_SUCCESS;

	control_init();
	gameDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_BUFFER_STATIC), NULL, NULL);
	if(gameDisplay == NULL) {
		control_term();
		return ref;
	}
	gfx_init(gameDisplay);
	gfx_render_target_clear(COLOR_BLACK);



	display_flip(gameDisplay);
	control_lock(timer_resolution / 4);

	char tempString[256];

	gameFont   = gfx_font_load_from_buffer(font, fontSize, COLOR_BLACK);

	gameTimer   = timer_create();

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
			if(control_just_pressed(CONTROL_BUTTON_A)) {
				threadExit = 0;
				threadReturnVal = OSTaskCreate(_thread, (void *) 0, (void *)&TaskStartStk[TASK_STK_SIZE - 1], TASK_PRIO);
			}

			if(control_just_pressed(CONTROL_BUTTON_B)) {
				threadExit = 1;
			}

			if (temppos <= 10)
				tempdir = 1;
			if (temppos >= 150)
				tempdir = -1;

			temppos += tempdir;
		}

		gfx_render_target_clear(COLOR_BLACK);

		if (threadRunning == 1)
			sprintf(tempString, "thread!! %u - threadret=%d", tempChar, threadReturnVal);
		else
			sprintf(tempString, "no thread %u - threadret=%d", tempChar, threadReturnVal);

		gfx_font_print(temppos, 4, gameFont, tempString);

		display_flip(gameDisplay);
	}

	// Exit thread if it's running
	if (threadRunning == 1)
		threadExit = 1;

	while (threadExit == 1); // Wait for thread to exit


	timer_delete(gameTimer);
	gfx_font_delete(gameFont);

	gfx_term();
	display_delete(gameDisplay);
	control_term();

	return 0;
}
