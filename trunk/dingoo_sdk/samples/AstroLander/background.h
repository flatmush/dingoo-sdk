#ifndef __background_h__
#define __background_h__

#include <stdbool.h>
#include <stdint.h>

#include <sml/graphics.h>

#ifdef FGL_BONUS_GRAPHICS
#include <fgl/fgl.h>
#endif



typedef struct {
	int32_t*     star_points;
	uint32_t     star_count;
	gfx_color    sky_color;
	#ifdef FGL_BONUS_GRAPHICS
	fgl_texture* planet_skin;
	fgl_fix16_t  planet_spin;
	fgl_fix16_t  planet_angle;
	#endif
} background;



extern background* backgroundCreate(uint16_t inStars, gfx_color inSkyColor);
extern void        backgroundDelete(background* inBackground);

extern void        backgroundTick(background* inBackground);

extern void        backgroundDraw(background* inBackground);

#endif
