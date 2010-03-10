#ifndef __fgl_matrix_h__
#define __fgl_matrix_h__

#include <stdint.h>
#include <fgl/fgl_fixmath.h>

#define FGL_MATRIX_MODEL      0
#define FGL_MATRIX_VIEW       1
#define FGL_MATRIX_PROJECTION 2
#define FGL_MATRIX_VIEWPORT   3
#define FGL_MATRIX_TEXTURE    4
#define FGL_MATRIX_COLOR      5
#define FGL_MATRIX_LAST FGL_MATRIX_COLOR

extern void    fgl_matrix_print(fgl_fix16_t* inMatrix);
extern void    fgl_matrix_copy(fgl_fix16_t* outMatrix, fgl_fix16_t* inMatrix);
extern void    fgl_matrix_transpose(fgl_fix16_t* outMatrix, fgl_fix16_t* inMatrix);
extern void    fgl_matrix_multiply(fgl_fix16_t* outMatrix, fgl_fix16_t* inMatrix0, fgl_fix16_t* inMatrix1);

extern void    fgl_matrix_mode_set(uint8_t inMode);
extern uint8_t fgl_matrix_mode_get();
extern void    fgl_matrix_identity();
extern void    fgl_matrix_push();
extern void    fgl_matrix_pop();

extern void    fgl_matrix_set(fgl_fix16_t* inMatrix);
extern void    fgl_matrix_get(fgl_fix16_t* outMatrix);

extern void    fgl_matrix_mult(fgl_fix16_t* inMatrix);

#endif
