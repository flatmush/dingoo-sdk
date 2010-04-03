#ifndef __background_h__
#define __background_h__

#include <stdbool.h>
#include <stdint.h>

#include <sml/graphics.h>



typedef struct {
	int32_t*  star_points;
	uint32_t  star_count;
	gfx_color sky_color;
} background;



extern background* backgroundCreate(uint16_t inStars, gfx_color inSkyColor);
extern void        backgroundDelete(background* inBackground);
extern void        backgroundDraw(background* inBackground);

#endif
