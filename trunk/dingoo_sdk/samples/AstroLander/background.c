#include "background.h"

#include <stdlib.h>

#include <sml/graphics.h>

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
	return tempBackground;
}

void backgroundDelete(background* inBackground) {
	free(inBackground);
}

void backgroundDraw(background* inBackground) {
	if(inBackground == NULL)
		return;
	gfx_render_target_clear(inBackground->sky_color);
	uintptr_t i;
	for(i = 0; i < inBackground->star_count; i++)
		gfx_point_draw(inBackground->star_points[(i << 1) + 0], inBackground->star_points[(i << 1) + 1], gfx_color_rgb(0xFF, 0xFF, 0xFF));
}
