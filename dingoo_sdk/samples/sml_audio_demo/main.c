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
#include <sml/wav.h>



display* main_display = NULL;
sound_t* sound_wav = NULL;



void sound_thread() {
	mtaudio_buffer_set(sound_wav->sample_data, ((sound_wav->sample_count * sound_wav->channels * sound_wav->sample_bits) >> 3), sound_wav->channels, 100, 48000);
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

	sound_wav = wav_load("test.wav");

	mtaudio_init(sound_thread,48000);

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
	wav_delete(sound_wav);
	gfx_term();
	display_delete(main_display);
	control_term();

	return ref;
}
