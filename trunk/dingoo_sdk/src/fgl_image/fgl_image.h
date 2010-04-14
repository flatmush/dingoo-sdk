#ifndef __fgl_image_h__
#define __fgl_image_h__

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	fgl_gray1, fgl_gray2, fgl_gray4, fgl_gray8, fgl_gray16, fgl_gray32, fgl_gray64,

	fgl_rgb121, fgl_rgb233, fgl_rgb565, fgl_rgb888,
	fgl_rgbx1111, fgl_rgbx2222, fgl_rgbx4444, fgl_rgbx5551, fgl_rgbx8888,

	fgl_bgr121, fgl_bgr332, fgl_bgr565, fgl_bgr888,
	fgl_bgrx1111, fgl_bgrx2222, fgl_bgrx4444, fgl_bgrx5551, fgl_bgrx8888,

	fgl_pixel_encoding_invalid
} fgl_pixel_encoding;

typedef struct {
	fgl_pixel_encoding encoding;
	uintptr_t width, height, stride;
	void*     clut_data;
	uintptr_t clut_entries;
	bool      clut_static;
	void*     data;
	bool      data_static;
	bool      v_flip, h_flip;
} fgl_image_complex;

typedef struct {
	uintptr_t width, height;
	uint32_t* data;
} fgl_image_basic;



extern fgl_image_basic*   fgl_image_basic_create(uintptr_t inWidth, uintptr_t inHeight);
extern void               fgl_image_basic_delete(fgl_image_basic* inImage);

extern void               fgl_image_complex_delete(fgl_image_complex* inImage);

extern fgl_image_basic*   fgl_image_decode(fgl_image_complex* inImage);
extern fgl_image_complex* fgl_image_encode(fgl_image_basic*   inImage, uint32_t inEncoding);

extern bool fgl_image_flip_horizontal(fgl_image_basic* inImage);
extern bool fgl_image_flip_vertical(fgl_image_basic* inImage);

extern bool fgl_image_scale_half_horizontal(fgl_image_basic* inImage);
extern bool fgl_image_scale_half_vertical(fgl_image_basic* inImage);
extern bool fgl_image_scale_half(fgl_image_basic* inImage);
extern bool fgl_image_scale(fgl_image_basic* inImage, uintptr_t inWidth, uintptr_t inHeight);

#endif
