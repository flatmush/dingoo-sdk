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
#include <strings.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

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

#define TEST_SUCCEEDED 0
#define TEST_FAILED    255

#define NMBR_OF_TESTS 5
uint16_t curAddTest = 0;

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
uint8_t (*test[NMBR_OF_TESTS])();

void do_cleanup() {
	testCleanup();
	uint8_t i;
	for (i = 0; i < 5; i++) {
		testArg[i] = NULL;
	}
}

// STRING.H (Untested: strcoll, strxfrm, strpbrk, strstr, strtok, strerror)
uint8_t testStringH()
{
	char* testStr1 = "Hello this is a test string";
	char* testStr2 = "yet another";
	char testBuf[256];

	// memchr
	char* pch;
	pch = (char*) memchr(testStr1, 't', strlen(testStr1));
	if (pch == NULL)
		return 1;

	if (pch - testStr1 != 6)
		return 2;

	pch = (char*) memchr(testStr2, 'r', strlen(testStr2));
	if (pch == NULL)
		return 3;

	if (pch - testStr2 != 10)
		return 4;

	pch = (char*) memchr(testStr2, 'w', strlen(testStr2));
	if (pch != NULL)
		return 5;

	// strchr
	int count = -1;
	pch = testStr1 - 1;
	do {
		count++;
		pch = strchr(pch + 1, 's');
		if (pch != NULL)
		{
			if (pch - testStr1 != 9 && pch - testStr1 != 12 && 
				pch - testStr1 != 18 && pch - testStr1 != 21)
			{
				return 6;
			}
		}
	} while(pch != NULL);

	if (count != 4)
		return 7;

	// strrchr
	pch = strrchr(testStr1, 's');
	if (pch - testStr1 != 21)
		return 8;
	pch = strrchr(testStr1, 'h');
	if (pch - testStr1 != 7)
		return 9;
	pch = strrchr(testStr1, '?');
	if (pch != NULL)
		return 10;

	// strcpy, strcmp, strlen, strncmp
	char* testPtr = strcpy(testBuf, testStr1);
	if (testPtr != testBuf)
		return 11;

	if (strlen(testBuf) != 27)
		return 12;

	if (strcmp(testBuf, testStr1) != 0)
		return 13;

	testBuf[strlen(testBuf) - 1] = '?';

	if (strcmp(testBuf, testStr1) == 0)
		return 14;

	testBuf[strlen(testBuf) - 1] = '\0';

	if (strcmp(testBuf, testStr1) == 0)
		return 15;

	if (strcmp(testStr1, testBuf) == 0)
		return 16;

	if (strncmp(testBuf, testStr1, strlen(testBuf)) != 0)
		return 17;

	strncpy(&testBuf[6], &testStr2[4], 7);
	
	if (strncmp(testBuf, testStr1, strlen(testBuf)) == 0)
		return 18;

	if (strncmp(testBuf, "Hello another a", 15) != 0)
		return 19;

	strncpy(testBuf, &testStr2[10], 3);
	if (testBuf[0] != 'r' || testBuf[1] != '\0' || testBuf[2] != '\0' || testBuf[3] != 'l')
		return 20;

	// memmove
	char memMoveStr[] = "memmove can be very useful......";
	memmove (memMoveStr + 20, memMoveStr + 15, 11);
	if (strcmp(memMoveStr, "memmove can be very very useful.") != 0)
		return 21;

	// memcpy, memcmp
	char* strMemcpy = "Hello this is a test";
	char* strMemcpy2 = "abc";
	char tempMemcpy1[16];
	char tempMemcpy2[20];
	memcpy(tempMemcpy1, strMemcpy, 16);
	memcpy(tempMemcpy2, strMemcpy, 20);
	if (memcmp(strMemcpy, tempMemcpy1, 16) != 0)
		return 22;
	if (memcmp(strMemcpy, tempMemcpy2, 20) != 0)
		return 23;
	tempMemcpy1[1] = '?';
	if (memcmp(strMemcpy, tempMemcpy1, 16) == 0)
		return 24;
	memcpy(&tempMemcpy1[1], strMemcpy2, 3);
	if (memcmp(tempMemcpy1, "Habco", 5) != 0)
		return 25;

	// memset
	char strMemset[] = "ABCDE";
	memset(&strMemset[1], '-', 3);
	if (strMemset[0] != 'A' || strMemset[4] != 'E')
		return 26;
	uint8_t i;
	for (i = 1; i < 4; i++) {
		if (strMemset[i] != '-')
			return 27;
	}

	// strcat
	strcpy(testBuf,"these ");
	strcat(testBuf,"strings ");
	strcat(testBuf,"are ");
	strcat(testBuf,"concatenated.");
	if (strcmp(testBuf, "these strings are concatenated.") != 0)
		return 28;

	// strncat
	memset(testBuf, '-', 20);
	strcpy(testBuf,"a ");
	strncat(testBuf,"s", 3);
	if (strcmp(testBuf, "a s") != 0 || testBuf[3] != '\0' || testBuf[4] != '-')
		return 29;

	// strcspn
	if (strcspn("fcba73", "1234567890") != 4)
		return 29;
	if (strcspn("fcba73", "xyz") != 6)
		return 30;

	// strspn
	if (strspn("129th", "1234567890") != 3)
		return 31;
	if (strspn("a129th", "1234567890") != 0)
		return 32;
	

	return 0;
}

uint8_t testStringsH()
{
	char* testStr1 = "Hello this is a test string";
	char* testStr2 = "hello this is a test string";
	char* testStr3 = "HELLO this is a test STRING";
	char* testStr4 = "helloo yet another";
	char* testStr5 = "yet another";
	char testBuf[256];

	if (ffs(0x00) != 0 || ffs(0x01) != 1 || ffs(0x02) != 2 || ffs(0xFF) != 1 ||
		ffs(0x800) != 12 || ffs(0xFFFFFFFF) != 1 || ffs(0x80000000) != 32)
	{
		return 1;
	}

	testBuf[0] = '\0';

	if (strcasecmp(testStr1, testStr2) != 0 || strcasecmp(testStr1, testStr3) != 0 ||
		strcasecmp(testStr3, testStr1) != 0 || strcasecmp(testStr1, testStr1) != 0 ||
		strcasecmp(testStr1, testBuf) != 104 || strcasecmp(testBuf, testStr1) != -104 ||
		strcasecmp(testStr4, testStr1) != 79 || strcasecmp(testStr1, testStr4) != -79 ||
		strcasecmp(testStr5, testStr1) != 17 || strcasecmp(testStr1, testStr5) != -17)
	{
		return 2;
	}

	if (strncasecmp(testStr1, testStr2, 100) != 0 || strncasecmp(testStr2, testStr1, 2) != 0 ||
		strncasecmp(testStr1, testStr4, 5) != 0 || strncasecmp(testStr1, testStr4, 6) != -79 ||
		strncasecmp(testStr1, testStr4, 0) != 0 || strncasecmp(testStr1, testStr5, 0) != 0 ||
		strncasecmp(testStr1, testStr5, 10) != -17 || strncasecmp(testStr5, testStr1, 10) != 17)
	{
		return 3;
	}

	return 0;
}

void testMathPrint() {
//	char buf[256];
//	double e = exp(1.0);
//	sprintf(buf, "Math test: exp(1.0) = %lf", e);
//	double p = pow(7.0, 3.0);
//	sprintf(buf, "Math test: pow(7.0, 3.0) = %lf", p);
//	gfx_font_print(10, 10, gameFont, buf);
}
void testMathCleanup() {
}

#define EPSILON 0.00001
bool IsEqual(double v1, double v2)
{
	return v1 > v2 - EPSILON && v1 < v2 + EPSILON;
}
uint8_t testMath() {
//	testDraw = testMathPrint;
//	testCleanup = testMathCleanup;
//	curTestReqUserInput = true;
	if(sin(M_PI_2) != 1.0)
		return 1;
	if(cos(0.0) != 1.0)
		return 2;
	if(tan(0.0) != 0.0)
		return 3;
	if(!IsEqual(exp(5.0), 148.413159))
		return 4;
	if(!IsEqual(pow(9.21, 3.5), 2370.875264))
		return 5;
	if(!IsEqual(log(5.5), 1.704748))
		return 6;
	if(!IsEqual(log10(1000.0), 3.0))
		return 7;
	if(!IsEqual(acos(0.5), M_PI/3))
		return 8;

	// lrint
	double dvals[] = {1.5, 0.50, 499999.0, 0.51, -0.5, -0.9, -0.1, 3.49999, 2.5};
	long dres[] = {2, 0, 499999, 1, 0, -1, 0, 3, 2};
	int i;
	for (i = 0; i < 9; i++)
	{
		if (lrint(dvals[i]) != dres[i])
			return 9 + i;
	}

	return 0;
}

// SPRITE (buffer)
void testSprite_gfx() {
	sprite* tempSprite = (sprite*)testArg[0];
	gfx_font_print_center(10, gameFont, "Does the image display correctly?");
	sprite_draw(tempSprite, 40, 40, 0);
}

void testSprite_cleanup() {
	sprite_delete((sprite*)testArg[0]);
}

uint8_t testSprite() {
	testDraw = testSprite_gfx;
	testCleanup = testSprite_cleanup;
	curTestReqUserInput = true;

	sprite* tempSprite = sprite_load_from_buffer(testspt, testsptSize);
	if (tempSprite == NULL)
		return 1;

	testArg[0] = tempSprite;

	return 0;
}

// GFX_TEXTURE (file)
void testTexture_gfx() {
	gfx_texture* tempTex = (gfx_texture*)testArg[0];
	gfx_font_print_center(10, gameFont, "Does the image display correctly?");
	gfx_tex_draw(40, 40, tempTex);
}

void testTexture_cleanup() {
	free(testArg[0]);
}

uint8_t testTexture() {
	testDraw = testTexture_gfx;
	testCleanup = testTexture_cleanup;
	curTestReqUserInput = true;

	gfx_texture* tempTex = gfx_tex_load_tga("testimg.tga");
	if (tempTex == NULL)
		return 1;

	testArg[0] = tempTex;

	return 0;
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

	test[0] = testStringH;
	test[1] = testStringsH;
	test[2] = testMath;
	test[3] = testSprite;
	test[4] = testTexture;

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
				curTestReqUserInput = false;
				do_cleanup();
				curTest++;
			}
		} else {
			uint8_t tempResult = (*test[curTest])();
			if (tempResult == 0) {
				if (!curTestReqUserInput) { // this could be set by the test function
					testResults[curTest] = TEST_SUCCEEDED;
					curTest++;
				}
			} else {
				testResults[curTest] = tempResult;
				curTest++;
			}
		}

		gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));

		if (testsDone) {
			// Print results
			uint16_t i;
			for (i = 0; i < NMBR_OF_TESTS; i++) {
				sprintf(tempString, "Test %u: %s (code %u)", i + 1, testResults[i] == 0 ? "OK" : "FAILED", testResults[i]);
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
