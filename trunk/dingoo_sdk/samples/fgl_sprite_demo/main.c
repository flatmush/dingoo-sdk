#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dingoo/ucos2.h>
#include <dingoo/entry.h>

#include <fgl/fgl.h>

#include <sml/display.h>
#include <sml/timer.h>
#include <sml/control.h>



extern fgl_texture* _fgl_texture;



display* appDisplay  = NULL;
timer*   appTimer    = NULL;
uint32_t appTickRate = (timer_resolution / 30);
bool     appRunning  = true;



void appScreenshot() {
	char tempString[256];
	unsigned long int tempNumber = 0;
	FILE* tempFile;
	while(true) {
		sprintf(tempString, "screenshot%lu.tga", tempNumber);
		tempFile = fopen(tempString, "rb");
		if(tempFile == NULL)
			break;
		fclose(tempFile);
		tempNumber++;
	}
	fgl_texture_save_tga(tempString, fgl_draw_buffer_get());
}

int main(int argc, char** argv) {
	int ref = EXIT_SUCCESS;
	srand(OSTimeGet());
	control_init();
	appTimer = timer_create();

	fgl_texture* tempDispTex = fgl_texture_create(320, 240);
	display* appDisplay = display_create(320, 240, 320, (DISPLAY_FILTER_NEAREST | DISPLAY_FORMAT_RGB565), tempDispTex->data, NULL);
	if(appDisplay == NULL)
		return EXIT_FAILURE;

	fgl_draw_buffer_set(tempDispTex);
	fgl_texture* tempRenderTarget = fgl_draw_buffer_get();
	uint16_t* tempDepth = (uint16_t*)malloc(tempRenderTarget->width * tempRenderTarget->height * 2);
	if(tempDepth != NULL)
		fgl_depth_buffer_set(tempDepth);
	fgl_enable(FGL_DEPTH_TEST);

	fgl_matrix_mode_set(FGL_MATRIX_VIEWPORT);
	fgl_matrix_identity();
	fgl_viewport(0, 0, fgl_fix16_from_int(320), fgl_fix16_from_int(240));
	fgl_matrix_mode_set(FGL_MATRIX_PROJECTION);
	fgl_matrix_identity();
	fgl_perspective((fgl_fix16_pi >> 3), fgl_fix16_div(fgl_fix16_from_int(320), fgl_fix16_from_int(240)), fgl_fix16_one, fgl_fix16_from_int(1024));
	fgl_matrix_mode_set(FGL_MATRIX_VIEW);
	fgl_matrix_identity();
	fgl_translate(fgl_fix16_from_int(0), fgl_fix16_from_int(0), fgl_fix16_from_int(10));
	fgl_matrix_mode_set(FGL_MATRIX_MODEL);
	fgl_matrix_identity();

	fgl_scale(fgl_fix16_from_int(1), fgl_fix16_from_int(1), fgl_fix16_from_int(1));

	fgl_texture* tempTexture = fgl_texture_load_tga("splash.tga");
	fgl_texture_bind(tempTexture);
	fgl_enable(FGL_TEXTURE_2D);

	//fgl_enable(FGL_CULL_FACE);

	fgl_vertex_t tempVerts[2] = {
		{ fgl_fix16_from_int(-1), fgl_fix16_from_int(+1), 0, 0, 0, FGL_COLOR_WHITE },
		{ fgl_fix16_from_int(+1), fgl_fix16_from_int(-1), 0, (_fgl_texture->width - 1), (_fgl_texture->height - 1), FGL_COLOR_WHITE },
		};

	/*fgl_vertex_t tempVerts[4] = {
		{ fgl_fix16_from_int(-1), fgl_fix16_from_int(-1), 0, 0, 0, FGL_COLOR_WHITE },
		{ fgl_fix16_from_int(+1), fgl_fix16_from_int(-1), 0, (_fgl_texture->width - 1), 0, FGL_COLOR_WHITE },
		{ fgl_fix16_from_int(-1), fgl_fix16_from_int(+1), 0, 0, (_fgl_texture->height - 1), FGL_COLOR_WHITE },
		{ fgl_fix16_from_int(+1), fgl_fix16_from_int(+1), 0, (_fgl_texture->width - 1), (_fgl_texture->height - 1), FGL_COLOR_WHITE },
		};*/

	int sysref;
	while(appRunning) {
		sysref = _sys_judge_event(NULL);
		if(sysref < 0) {
			ref = sysref;
			break;
		}

		// Control
		control_poll();
		if(control_check(CONTROL_BUTTON_START).pressed && control_check(CONTROL_BUTTON_SELECT).pressed)
			appRunning = false;
		else if(control_check(CONTROL_BUTTON_SELECT).pressed && control_check(CONTROL_BUTTON_SELECT).changed)
			appScreenshot();

		// Draw
		fgl_clear(FGL_COLOR_BUFFER_BIT | FGL_DEPTH_BUFFER_BIT);
		fgl_draw_array(FGL_SPRITES, tempVerts, 2);
		//fgl_draw_array(FGL_TRIANGLE_STRIP, tempVerts, 4);
		fgl_rotate_y(500);
		display_flip(appDisplay);
	}

	fgl_texture_delete(tempTexture);
	if(tempDepth != NULL)
		free(tempDepth);

	timer_delete(appTimer);
	control_term();
	display_delete(appDisplay);
	return EXIT_SUCCESS;
}
