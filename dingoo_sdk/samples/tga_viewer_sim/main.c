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

#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)

#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_REVISION 0



display*     appDisplay  = NULL;
bool         appRunning  = true;
gfx_texture* appImage    = NULL;



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

	appImage = gfx_tex_load_tga(argv[0]); // In SIM files argv[0] contains the target to be loaded.

	int sysref;
	while(appRunning) {
		sysref = _sys_judge_event(NULL);
		if(sysref < 0) {
			ref = sysref;
			break;
		}

		control_poll();
		if(control_check(CONTROL_BUTTON_START).pressed && control_check(CONTROL_BUTTON_SELECT).pressed) {
			appRunning = false;
			break;
		}

		gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
		gfx_tex_draw(((appDisplay->width - appImage->width) >> 1), ((appDisplay->height - appImage->height) >> 1), appImage);
		display_flip(appDisplay);
	}

	gfx_term();
	display_delete(appDisplay);
	control_term();

	return 0;
}
