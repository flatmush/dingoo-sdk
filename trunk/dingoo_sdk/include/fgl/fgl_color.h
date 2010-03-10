#ifndef __fgl_color_h__
#define __fgl_color_h__

#include <stdbool.h>
#include <stdint.h>

typedef uint16_t fgl_color_t;
typedef uint32_t fgl_color_pair_t;

#define fgl_color_red(inColor)   ((inColor) >> 11)
#define fgl_color_green(inColor) (((inColor) & 0x07E0) >> 5)
#define fgl_color_blue(inColor)  ((inColor) & 0x001F)

#define fgl_color_rgb(inRed, inGreen, inBlue) (((inRed) << 11) | ((inGreen) << 5) | (inBlue))

extern fgl_color_t _fgl_color;

extern fgl_color_t fgl_color_halfblend(fgl_color_t inColor0, fgl_color_t inColor1);
extern fgl_color_t fgl_color_multiply(fgl_color_t inColor0, fgl_color_t inColor1);

#endif
