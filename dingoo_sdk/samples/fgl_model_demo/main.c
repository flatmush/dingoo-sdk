#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dingoo/fsys.h>
#include <dingoo/ucos2.h>
#include <dingoo/entry.h>

#include <fgl/fgl.h>

#include <sml/display.h>
#include <sml/timer.h>
#include <sml/control.h>




display* appDisplay  = NULL;
timer*   appTimer    = NULL;
uint32_t appTickRate = (timer_resolution / 30);
bool     appRunning  = true;
char     appPath[256];

char* appPathInit(const char* inPath) {
	uintptr_t i, j;
	for(i = 0, j = 0; inPath[i] != '\0'; i++) {
		if((inPath[i] == '\\') || (inPath[i] == '/'))
			j = i + 1;
	}
	strncpy(appPath, inPath, j);
	return appPath;
}

void appScreenshot() {
	char tempString[256];
	unsigned long int tempNumber = 0;
	FILE* tempFile;
	while(true) {
		sprintf(tempString, "%sscreenshot%lu.tga", appPath, tempNumber);
		tempFile = fsys_fopen(tempString, "rb");
		if(tempFile == NULL)
			break;
		fsys_fclose(tempFile);
		tempNumber++;
	}
	fgl_texture_save_tga(tempString, fgl_draw_buffer_get());
}

int main(int argc, char** argv) {
	appPathInit(argv[0]);
	int ref = EXIT_SUCCESS;
	srand(OSTimeGet());
	control_init();
	appTimer = timer_create();

	fgl_texture* tempDispTex = fgl_texture_create(9, 8);
	display* appDisplay = display_create(320, 240, 512, (DISPLAY_FILTER_NEAREST | DISPLAY_FORMAT_RGB565), tempDispTex->data, NULL);
	if(appDisplay == NULL)
		return EXIT_FAILURE;

	fgl_draw_buffer_set(tempDispTex);
	fgl_texture* tempRenderTarget = fgl_draw_buffer_get();
	uint16_t* tempDepth = (uint16_t*)malloc(2 << (tempRenderTarget->width + tempRenderTarget->height));
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
	fgl_scale(fgl_fix16_from_int(1) >> 4, fgl_fix16_from_int(1) >> 4, fgl_fix16_from_int(1) >> 4);
	fgl_rotate_x(-fgl_fix16_pi >> 1);
	fgl_rotate_y(-fgl_fix16_pi >> 1);

	char tempString[256];
	sprintf(tempString, "%smodel.md2", appPath);
	fgl_model* tempModel = fgl_model_load_md2(tempString);

	sprintf(tempString, "%sskin.tga", appPath);
	fgl_texture* tempTexture = fgl_texture_load_tga(tempString);

	fgl_texture_bind(tempTexture);
	fgl_enable(FGL_TEXTURE_2D);
	fgl_clear_color(fgl_color_rgb(0 , 31, 31));

	fgl_fix16_t tempFrame = 0;

	//fgl_enable(FGL_CULL_FACE);

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

		if(control_just_pressed(CONTROL_DPAD_UP)) {
			fgl_matrix_mode_set(FGL_MATRIX_VIEW);
			fgl_translate(fgl_fix16_from_int(0), fgl_fix16_from_int(0), fgl_fix16_from_int(-10));
			fgl_matrix_mode_set(FGL_MATRIX_MODEL);
		}
		if(control_just_pressed(CONTROL_DPAD_DOWN)) {
			fgl_matrix_mode_set(FGL_MATRIX_VIEW);
			fgl_translate(fgl_fix16_from_int(0), fgl_fix16_from_int(0), fgl_fix16_from_int(+10));
			fgl_matrix_mode_set(FGL_MATRIX_MODEL);
		}

		// Draw
		fgl_clear(FGL_COLOR_BUFFER_BIT | FGL_DEPTH_BUFFER_BIT);

		fgl_model_draw(tempModel, tempFrame);

		tempFrame += (1 << 14);
		if(tempFrame >= fgl_fix16_from_int(tempModel->frame_count + 1))
			tempFrame -= fgl_fix16_from_int(tempModel->frame_count);

		display_flip(appDisplay);
	}

	fgl_texture_delete(tempTexture);
	fgl_model_delete(tempModel);
	if(tempDepth != NULL)
		free(tempDepth);

	timer_delete(appTimer);
	control_term();
	display_delete(appDisplay);
	return EXIT_SUCCESS;
}
