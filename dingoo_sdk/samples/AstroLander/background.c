#include "background.h"

#include <stdlib.h>

#include "game.h"



background* backgroundCreate(uint16_t inStars, gfx_color inSkyColor) {
	background* tempBackground  = (background*)malloc(sizeof(background) + (inStars << 3));
	if(tempBackground == NULL)
		return NULL;
	tempBackground->sky_color   = inSkyColor;
	tempBackground->star_count  = inStars;
	tempBackground->star_points = (int32_t*)((uintptr_t)tempBackground + sizeof(background));
	uintptr_t i;
	for(i = 0; i < tempBackground->star_count; i++) {
		tempBackground->star_points[(i << 1) + 0] = (rand() % gameDisplay->width);
		tempBackground->star_points[(i << 1) + 1] = (rand() % gameDisplay->height);
	}

	#ifdef FGL_BONUS_GRAPHICS
	tempBackground->planet_skin = fgl_texture_load_tga("earth.tga");
	if(tempBackground->planet_skin == NULL) {
		free(tempBackground);
		return NULL;
	}
	tempBackground->planet_spin  = 125;
	tempBackground->planet_angle = 0;
	#endif

	return tempBackground;
}

void backgroundDelete(background* inBackground) {
	if(inBackground == NULL)
		return;
	#ifdef FGL_BONUS_GRAPHICS
	fgl_texture_delete(inBackground->planet_skin);
	#endif
	free(inBackground);
}



void backgroundTick(background* inBackground) {
	if(inBackground == NULL)
		return;

	#ifdef FGL_BONUS_GRAPHICS
	inBackground->planet_angle += inBackground->planet_spin;
	#endif
}



void backgroundDraw(background* inBackground) {
	if(inBackground == NULL)
		return;

	#ifdef FGL_BONUS_GRAPHICS
	fgl_clear_color(inBackground->sky_color);
	fgl_clear(FGL_COLOR_BUFFER_BIT);
	#else
	gfx_render_target_clear(inBackground->sky_color);
	#endif

	uintptr_t i;
	for(i = 0; i < inBackground->star_count; i++)
		gfx_point_draw(inBackground->star_points[(i << 1) + 0], inBackground->star_points[(i << 1) + 1], gfx_color_rgb(0xFF, 0xFF, 0xFF));

	#ifdef FGL_BONUS_GRAPHICS
	fgl_matrix_push();
	fgl_rotate_y(inBackground->planet_angle);
	fgl_translate(fgl_fix16_from_int(0), fgl_fix16_from_int(0), fgl_fix16_from_int(10));
	fgl_scale(fgl_fix16_from_int(2), fgl_fix16_from_int(2), fgl_fix16_from_int(2));
	fgl_texture_bind(inBackground->planet_skin);
	fgl_draw_sphere(16);
	fgl_matrix_pop();
	#endif
}
