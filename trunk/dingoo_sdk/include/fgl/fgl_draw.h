#ifndef __fgl_draw_h__
#define __fgl_draw_h__

#include <stdbool.h>
#include <stdint.h>

#include  <fgl/fgl_vertex.h>

#define FGL_POINTS         1
#define FGL_LINES          2
#define FGL_LINE_STRIP     3
#define FGL_LINE_LOOP      4
#define FGL_TRIANGLES      5
#define FGL_TRIANGLE_STRIP 6
#define FGL_TRIANGLE_FAN   7

extern void fgl_draw_array(uint8_t inType, fgl_vertex_t* inVerts, uintptr_t inVertCount);

#endif
