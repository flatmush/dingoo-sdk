#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <dingoo/ucos2.h>
#include <dingoo/entry.h>
#include <dingoo/audio.h>


#include <sml/graphics.h>
#include <sml/display.h>
#include <sml/control.h>
#include <sml/timer.h>
#include <sml/fixmath.h>

#include <sml/mtaudio.h>
#include <modplay/modplay_a320.h>



display* main_display = NULL;
MODFILE mod;
signed short wav_sound_mod[1024];  // Sound buffer


void sound_thread() {
  MODFILE *modthread = &mod;

  modthread->mixingbuf = (void*) wav_sound_mod;
  modthread->mixingbuflen = 1024;
  MODFILE_Player(modthread);
      
	mtaudio_buffer_set(wav_sound_mod, 1024, 1, 70,22050);
}

void draw_sine_wave(fix16_t inFrequency) {
	uintptr_t i;
	fix16_t tempAngle;
	fix16_t tempY[2] = { 0, (main_display->height << 15) };
	for(i = 1; i < main_display->width; i++) {
		tempAngle = ((i * fix16_mul(inFrequency, (fix16_pi << 1))) / main_display->width);
		tempY[0] = fix16_sin(tempAngle);
		tempY[0] *= (main_display->height >> 2);
		tempY[0] += (main_display->height << 15);
		gfx_line_draw((i - 1), fix16_to_int(tempY[1]), i, fix16_to_int(tempY[0]), gfx_color_rgb(0xFF, 0xFF, 0xFF));
		tempY[1] = tempY[0];
	}
}



int main(int argc, char** argv) {
	int ref = EXIT_SUCCESS;
	srand(OSTimeGet());

	control_init();
	control_lock(timer_resolution / 4);
	main_display = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_BUFFER_STATIC), NULL, NULL);
	if(main_display == NULL)
		return ref;
	gfx_init(main_display);

  modplay_init(&mod);
  modplay_loadmod("test.mod", &mod);
  
	mtaudio_init(sound_thread,22050);

	gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	draw_sine_wave(0x00010000);
	gfx_render_target_swap();

	int sysref;
	while(true) {
		sysref = _sys_judge_event(NULL);
		if(sysref < 0) {
			ref = sysref;
			break;
		}

		control_poll();
		if(control_check(CONTROL_BUTTON_START).pressed)
			break;
	}

	mtaudio_term();

  modplay_term(&mod);

	gfx_term();
	display_delete(main_display);
	control_term();

	return ref;
}
