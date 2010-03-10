#ifndef __fgl_transform_h__
#define __fgl_transform_h__

#include <stdbool.h>
#include <stdint.h>

#include <fgl/fgl_fixmath.h>
#include <fgl/fgl_matrix.h>
#include <fgl/fgl_vertex.h>

extern fgl_vertex_t fgl_transform_vertex(fgl_vertex_t inVertex, fgl_fix16_t* inMatrix);

extern void fgl_translate(fgl_fix16_t inX, fgl_fix16_t inY, fgl_fix16_t inZ);
extern void fgl_scale(fgl_fix16_t inX, fgl_fix16_t inY, fgl_fix16_t inZ);
extern void fgl_rotate_x(fgl_fix16_t inAngle);
extern void fgl_rotate_y(fgl_fix16_t inAngle);
extern void fgl_rotate_z(fgl_fix16_t inAngle);

extern void fgl_ortho(fgl_fix16_t inLeft, fgl_fix16_t inRight, fgl_fix16_t inTop, fgl_fix16_t inBottom, fgl_fix16_t inNear, fgl_fix16_t inFar);
extern void fgl_ortho_2d(fgl_fix16_t inLeft, fgl_fix16_t inRight, fgl_fix16_t inTop, fgl_fix16_t inBottom);
extern void fgl_frustum(fgl_fix16_t inLeft, fgl_fix16_t inRight, fgl_fix16_t inTop, fgl_fix16_t inBottom, fgl_fix16_t inNear, fgl_fix16_t inFar);
extern void fgl_perspective(fgl_fix16_t inFovY, fgl_fix16_t inAspect, fgl_fix16_t inNear, fgl_fix16_t inFar);
extern void fgl_viewport(fgl_fix16_t inX, fgl_fix16_t inY, fgl_fix16_t inWidth, fgl_fix16_t inHeight);

#endif
