// ADD A TEST:
// 1. Add your function(s)
// 2. Update NMBR_OF_TESTS
// 3. Add it to the function pointer array in main
//
// Note: When using gfx and cleanup functions, if you use copy/paste, make sure
//       you change the pointers correctly to these two functions.

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

#include "font.h"
#include "testspt.h"

#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)

#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_REVISION 0

#define TEST_SUCCEEDED 1
#define TEST_FAILED    2

char* testResultText[] = {"not tested", "OK", "FAILED"};

#define NMBR_OF_TESTS 5

gfx_font*    gameFont            = NULL;
display*     gameDisplay         = NULL;
timer*       gameTimer           = NULL;
uint32_t     gameTickRate        = (timer_resolution / 30);
bool         gameRunning         = true;
uint16_t     curTest             = 0;
bool         curTestReqUserInput = false;
bool         testsDone           = false;
uint8_t      testResults[NMBR_OF_TESTS];
void*        testArg[5];

void (*testCleanup)();
void (*testDraw)();
bool (*test[NMBR_OF_TESTS])();

void do_cleanup() {
	testCleanup();
	uint8_t i;
	for (i = 0; i < 5; i++) {
		testArg[i] = NULL;
	}
}

// MEMSET
bool test1() {
	char str[] = "ABCDE";
	memset(&str[1], '-', 3);
	if (str[0] != 'A' || str[4] != 'E')
		return false;
	uint8_t i;
	for (i = 1; i < 4; i++) {
		if (str[i] != '-')
			return false;
	}

	return true;
}

// MEMCMP
bool test2() {
	char str[] = "ABCDEFGHIJKLMNOP";
	char str2[16];
	uint8_t i;
	char j;
	for (i = 0, j = 'A'; i < 16; i++, j++) {
		str2[i] = j;
	}

	if (memcmp(str, str2, 16) != 0)
		return false;

	if (memcmp(&str[5], &str2[5], 7) != 0)
		return false;

	return true;
}

// MEMCPY
bool test3() {
	char* str = "Hello this is a test";
	char temp1[16];
	char temp2[20];
	memcpy(temp1, str, 16);
	memcpy(temp2, str, 20);
	if (memcmp(str, temp1, 16) != 0)
		return false;
	if (memcmp(str, temp2, 20) != 0)
		return false;

	return true;
}

// SPRITE (buffer)
void test4gfx() {
	sprite* tempSprite = (sprite*)testArg[0];
	gfx_font_print_center(10, gameFont, "Does the image display correctly?");
	sprite_draw(tempSprite, 40, 40, 0);
}

void test4cleanup() {
	sprite_delete((sprite*)testArg[0]);
}

bool test4() {
	testDraw = test4gfx;
	testCleanup = test4cleanup;
	curTestReqUserInput = true;

	sprite* tempSprite = sprite_load_from_buffer(testspt, testsptSize);
	if (tempSprite == NULL)
		return false;

	testArg[0] = tempSprite;

	return true;
}

// GFX_TEXTURE (file)
void test5gfx() {
	gfx_texture* tempTex = (gfx_texture*)testArg[0];
	gfx_font_print_center(10, gameFont, "Does the image display correctly?");
	gfx_tex_draw(40, 40, tempTex);
}

void test5cleanup() {
	free(testArg[0]);
}

bool test5() {
	testDraw = test5gfx;
	testCleanup = test5cleanup;
	curTestReqUserInput = true;

	gfx_texture* tempTex = gfx_tex_load_tga("testimg.tga");
	if (tempTex == NULL)
		return false;

	testArg[0] = tempTex;

	return true;
}


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

	gameFont = gfx_font_load_from_buffer(tgaFont, tgaFontSize, COLOR_BLACK);

	gameTimer = timer_create();

	// Initialize testResults to 0 (do not use memset since it's part of the tests)
	uint8_t ti;
	for (ti = 0; ti < NMBR_OF_TESTS; ti++) {
		testResults[ti] = 0;
	}

	test[0] = test1;
	test[1] = test2;
	test[2] = test3;
	test[3] = test4;
	test[4] = test5;

	uint32_t tempTick    = 0;
	uint32_t tempPauseOS = 0;
	char     tempString[256];

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
		}

		if (testsDone) {
			if (control_just_pressed(CONTROL_BUTTON_A) || control_just_pressed(CONTROL_BUTTON_B)) {
				gameRunning = false;
			}
		} else if (curTest == NMBR_OF_TESTS) {
			testsDone = true;
		} else if (curTestReqUserInput) {
			if (control_just_pressed(CONTROL_BUTTON_A)) {
				// Advance to the next test
				testResults[curTest] = TEST_SUCCEEDED;
				curTestReqUserInput = false;
				do_cleanup();
				curTest++;
			} else if (control_just_pressed(CONTROL_BUTTON_B)) {
				// Fail current test
				testResults[curTest] = TEST_FAILED;
				do_cleanup();
				curTest++;
			}
		} else {
			bool tempResult = (*test[curTest])();
			if (tempResult == true) {
				if (!curTestReqUserInput) { // this could be set by the test function
					testResults[curTest] = TEST_SUCCEEDED;
					curTest++;
				}
			} else {
				testResults[curTest] = TEST_FAILED;
				curTest++;
			}
		}

		gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));

		if (testsDone) {
			// Print results
			uint16_t i;
			for (i = 0; i < NMBR_OF_TESTS; i++) {
				sprintf(tempString, "Test %u: %s", i + 1, testResultText[testResults[i]]);
				gfx_font_print(10, 10 + i*10, gameFont, tempString);
				gfx_font_print_center(220, gameFont, "Press A or B to quit");
			}
		} else {
			if (curTestReqUserInput) {
				testDraw();
			} else {
				sprintf(tempString, "Performing test %u of %u", curTest, NMBR_OF_TESTS);
				gfx_font_print_center(100, gameFont, tempString);
			}
		}
		display_flip(gameDisplay);
	}

	timer_delete(gameTimer);
	free(gameFont);

	gfx_term();
	display_delete(gameDisplay);
	control_term();

	return 0;
}
