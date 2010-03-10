#ifndef __fgl_vertex_h__
#define __fgl_vertex_h__

#include <stdbool.h>
#include <stdint.h>

#include <fgl/fgl_fixmath.h>
#include <fgl/fgl_color.h>

typedef struct {
	fgl_fix16_t x, y, z;
	fgl_fix16_t u, v;
	fgl_color_t c;
} fgl_vertex_t;

extern void fgl_vertex_print(fgl_vertex_t inVertex);

#endif
