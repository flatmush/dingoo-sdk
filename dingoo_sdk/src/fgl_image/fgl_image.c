#include "fgl_image.h"

#include <stdlib.h>
#include <stdio.h>



#ifndef swap
#define swap(x, y) { typeof(y) __swap = (x); (x) = (y); (y) = __swap; }
#endif

typedef struct {
	uint8_t r, g, b, x;
} _fgl_pixel_t;

typedef union {
	uint64_t* d;
	uint32_t* w;
	uint16_t* h;
	uint8_t*  q;
} _mptr;



inline _fgl_pixel_t _fgl_pixel_blend2(_fgl_pixel_t inPix0, _fgl_pixel_t inPix1) {
	uintptr_t tempBlend;
	_fgl_pixel_t tempOut;

	tempBlend = inPix0.r + inPix1.r;
	tempOut.r = (tempBlend >> 1);

	tempBlend = inPix0.g + inPix1.g;
	tempOut.g = (tempBlend >> 1);

	tempBlend = inPix0.b + inPix1.b;
	tempOut.b = (tempBlend >> 1);

	return tempOut;
}

inline _fgl_pixel_t _fgl_pixel_blend4(_fgl_pixel_t inPix0, _fgl_pixel_t inPix1, _fgl_pixel_t inPix2, _fgl_pixel_t inPix3) {
	uintptr_t tempBlend;
	_fgl_pixel_t tempOut;

	tempBlend = inPix0.r + inPix1.r + inPix2.r + inPix3.r;
	tempOut.r = (tempBlend >> 2);

	tempBlend = inPix0.g + inPix1.g + inPix2.g + inPix3.g;
	tempOut.g = (tempBlend >> 2);

	tempBlend = inPix0.b + inPix1.b + inPix2.b + inPix3.b;
	tempOut.b = (tempBlend >> 2);

	return tempOut;
}

inline uintptr_t _fgl_image_row_size(uintptr_t inWidth, uintptr_t inAlign, fgl_pixel_encoding inEncoding) {
	uintptr_t tempSize = inWidth;
	switch(inEncoding) {
		case fgl_gray1:
			tempSize  += 7;
			tempSize >>= 3;
			break;
		case fgl_gray2:
			tempSize  *= 2;
			tempSize  += 6;
			tempSize >>= 1;
			break;
		case fgl_gray4:
		case fgl_rgb121:
		case fgl_rgbx1111:
		case fgl_bgr121:
		case fgl_bgrx1111:
			tempSize  *= 4;
			tempSize  += 4;
			tempSize >>= 2;
			break;
		case fgl_gray8:
		case fgl_rgb233:
		case fgl_rgbx2222:
		case fgl_bgr332:
		case fgl_bgrx2222:
			break;
		case fgl_gray16:
		case fgl_rgb565:
		case fgl_rgbx5551:
		case fgl_rgbx4444:
		case fgl_bgr565:
		case fgl_bgrx5551:
		case fgl_bgrx4444:
			tempSize *= 2;
			break;
		case fgl_rgb888:
		case fgl_bgr888:
			tempSize *= 3;
			break;
		case fgl_gray32:
		case fgl_rgbx8888:
		case fgl_bgrx8888:
			tempSize *= 4;
			break;
		case fgl_gray64:
			tempSize *= 8;
			break;
	}
	tempSize += (inAlign - 1);
	tempSize /= inAlign;
	return tempSize;
}

inline _fgl_pixel_t _fgl_image_decode_pixel(void* inLine, uintptr_t inX, fgl_pixel_encoding inEncoding) {
	_mptr tempPtr; tempPtr.w = (uint32_t*)inLine;
	_fgl_pixel_t tempPixel; tempPixel.x = 0;
	switch(inEncoding) {
		case fgl_gray1:
			tempPtr.q += (inX >> 3);
			tempPixel.r = ((*tempPtr.q) >> (inX & 0x07)) & 0x01;
			tempPixel.r = (tempPixel.r ? 0xFF : 0x00);
			tempPixel.g = tempPixel.r;
			tempPixel.b = tempPixel.r;
			return tempPixel;
		case fgl_gray2:
			tempPtr.q += (inX >> 2);
			tempPixel.r = ((*tempPtr.q) >> (inX & 0x03)) & 0x03;
			tempPixel.r |= (tempPixel.r << 2);
			tempPixel.r |= (tempPixel.r << 4);
			tempPixel.g = tempPixel.r;
			tempPixel.b = tempPixel.r;
			return tempPixel;
		case fgl_gray4:
			tempPtr.q += (inX >> 1);
			tempPixel.r = ((*tempPtr.q) >> (inX & 0x01)) & 0x07;
			tempPixel.r |= (tempPixel.r << 4);
			tempPixel.g = tempPixel.r;
			tempPixel.b = tempPixel.r;
			return tempPixel;
		case fgl_gray8:
			tempPixel.r = tempPtr.q[inX];
			tempPixel.g = tempPixel.r;
			tempPixel.b = tempPixel.r;
			return tempPixel;
		case fgl_gray16:
			tempPixel.r = (tempPtr.h[inX] >> 8);
			tempPixel.g = tempPixel.r;
			tempPixel.b = tempPixel.r;
			return tempPixel;
		case fgl_gray32:
			tempPixel.r = (tempPtr.w[inX] >> 24);
			tempPixel.g = tempPixel.r;
			tempPixel.b = tempPixel.r;
			return tempPixel;
		case fgl_gray64:
			tempPixel.r = (tempPtr.d[inX] >> 56);
			tempPixel.g = tempPixel.r;
			tempPixel.b = tempPixel.r;
			return tempPixel;
		case fgl_rgb121:
		case fgl_bgr121:
			tempPtr.q += (inX >> 1);
			tempPixel.b = ((*tempPtr.q) >> (inX & 0x01)) & 0x07;
			tempPixel.r = ((tempPixel.b >> 0) & 0x01);
			tempPixel.g = ((tempPixel.b >> 1) & 0x03);
			tempPixel.b = ((tempPixel.b >> 3) & 0x01);
			tempPixel.r |= (tempPixel.r << 1);
			tempPixel.r |= (tempPixel.r << 2);
			tempPixel.r |= (tempPixel.r << 4);
			tempPixel.g |= (tempPixel.g << 2);
			tempPixel.g |= (tempPixel.g << 4);
			tempPixel.b |= (tempPixel.b << 1);
			tempPixel.b |= (tempPixel.b << 2);
			tempPixel.b |= (tempPixel.b << 4);
			if(inEncoding == fgl_bgr121)
				swap(tempPixel.b, tempPixel.r);
			return tempPixel;
		case fgl_rgb233:
			tempPixel.b = tempPtr.q[inX];
			tempPixel.r = ((tempPixel.b >> 0) & 0x03);
			tempPixel.g = ((tempPixel.b >> 2) & 0x07);
			tempPixel.b = ((tempPixel.b >> 5) & 0x07);
			tempPixel.r |= (tempPixel.r << 2);
			tempPixel.r |= (tempPixel.r << 4);
			tempPixel.g |= (tempPixel.g << 3);
			tempPixel.g  = ((tempPixel.g << 2) | (tempPixel.g >> 4));
			tempPixel.b |= (tempPixel.b << 3);
			tempPixel.b  = ((tempPixel.b << 2) | (tempPixel.b >> 4));
			return tempPixel;
		case fgl_bgr332:
			tempPixel.b = tempPtr.q[inX];
			tempPixel.r = ((tempPixel.b >> 0) & 0x07);
			tempPixel.g = ((tempPixel.b >> 3) & 0x07);
			tempPixel.b = ((tempPixel.b >> 6) & 0x03);
			tempPixel.r |= (tempPixel.r << 3);
			tempPixel.r  = ((tempPixel.r << 2) | (tempPixel.r >> 4));
			tempPixel.g |= (tempPixel.g << 3);
			tempPixel.g  = ((tempPixel.g << 2) | (tempPixel.g >> 4));
			tempPixel.b |= (tempPixel.b << 2);
			tempPixel.b |= (tempPixel.b << 4);
			return tempPixel;
		case fgl_rgb565:
		case fgl_bgr565:
			tempPtr.h += inX;
			tempPixel.r = ((*tempPtr.h >>  0) & 0x1F);
			tempPixel.g = ((*tempPtr.h >>  5) & 0x3F);
			tempPixel.b = ((*tempPtr.h >> 11) & 0x1F);
			tempPixel.r = ((tempPixel.r << 3) | (tempPixel.r >> 2));
			tempPixel.g = ((tempPixel.g << 2) | (tempPixel.g >> 2));
			tempPixel.b = ((tempPixel.b << 3) | (tempPixel.b >> 2));
			if(inEncoding == fgl_bgr565)
				swap(tempPixel.b, tempPixel.r);
			return tempPixel;
		case fgl_rgb888:
			tempPtr.q += (inX * 3);
			tempPixel.r = tempPtr.q[0];
			tempPixel.g = tempPtr.q[1];
			tempPixel.b = tempPtr.q[2];
			return tempPixel;
		case fgl_bgr888:
			tempPtr.q += (inX * 3);
			tempPixel.r = tempPtr.q[2];
			tempPixel.g = tempPtr.q[1];
			tempPixel.b = tempPtr.q[0];
			return tempPixel;
		case fgl_rgbx1111:
		case fgl_bgrx1111:
			tempPtr.q += (inX >> 1);
			tempPixel.b = ((*tempPtr.q) >> (inX & 0x01)) & 0x07;
			tempPixel.r = ((tempPixel.b >> 0) & 0x01);
			tempPixel.g = ((tempPixel.b >> 1) & 0x01);
			tempPixel.b = ((tempPixel.b >> 3) & 0x01);
			tempPixel.r |= (tempPixel.r << 1);
			tempPixel.r |= (tempPixel.r << 2);
			tempPixel.r |= (tempPixel.r << 4);
			tempPixel.g |= (tempPixel.g << 1);
			tempPixel.g |= (tempPixel.g << 2);
			tempPixel.g |= (tempPixel.g << 4);
			tempPixel.b |= (tempPixel.b << 1);
			tempPixel.b |= (tempPixel.b << 2);
			tempPixel.b |= (tempPixel.b << 4);
			if(inEncoding == fgl_bgrx1111)
				swap(tempPixel.b, tempPixel.r);
			return tempPixel;
		case fgl_rgbx2222:
		case fgl_bgrx2222:
			tempPixel.b = tempPtr.q[inX];
			tempPixel.r = ((tempPixel.b >> 0) & 0x03);
			tempPixel.g = ((tempPixel.b >> 2) & 0x03);
			tempPixel.b = ((tempPixel.b >> 4) & 0x03);
			tempPixel.r |= (tempPixel.r << 2);
			tempPixel.r |= (tempPixel.r << 4);
			tempPixel.g |= (tempPixel.g << 2);
			tempPixel.g |= (tempPixel.g << 4);
			tempPixel.b |= (tempPixel.b << 2);
			tempPixel.b |= (tempPixel.b << 4);
			if(inEncoding == fgl_bgrx2222)
				swap(tempPixel.b, tempPixel.r);
			return tempPixel;
		case fgl_rgbx4444:
		case fgl_bgrx4444:
			tempPtr.h += inX;
			tempPixel.r = ((*tempPtr.h >> 0) & 0x0F);
			tempPixel.g = ((*tempPtr.h >> 4) & 0x0F);
			tempPixel.b = ((*tempPtr.h >> 8) & 0x0F);
			tempPixel.r |= (tempPixel.r << 4);
			tempPixel.g |= (tempPixel.g << 4);
			tempPixel.b |= (tempPixel.b << 4);
			if(inEncoding == fgl_bgrx4444)
				swap(tempPixel.b, tempPixel.r);
			return tempPixel;
		case fgl_rgbx5551:
		case fgl_bgrx5551:
			tempPtr.h += inX;
			tempPixel.r = ((*tempPtr.h >>  0) & 0x1F);
			tempPixel.g = ((*tempPtr.h >>  5) & 0x1F);
			tempPixel.b = ((*tempPtr.h >> 10) & 0x1F);
			tempPixel.r = ((tempPixel.r << 3) | (tempPixel.r >> 2));
			tempPixel.g = ((tempPixel.g << 3) | (tempPixel.g >> 2));
			tempPixel.b = ((tempPixel.b << 3) | (tempPixel.b >> 2));
			if(inEncoding == fgl_bgrx5551)
				swap(tempPixel.b, tempPixel.r);
			return tempPixel;
		case fgl_rgbx8888:
			tempPtr.w += inX;
			tempPixel.r = tempPtr.q[0];
			tempPixel.g = tempPtr.q[1];
			tempPixel.b = tempPtr.q[2];
			return tempPixel;
		case fgl_bgrx8888:
			tempPtr.w += inX;
			tempPixel.r = tempPtr.q[2];
			tempPixel.g = tempPtr.q[1];
			tempPixel.b = tempPtr.q[0];
			return tempPixel;
	}

	tempPixel.r = 0;
	tempPixel.g = 0;
	tempPixel.b = 0;
	return tempPixel;
}

void _fgl_image_decode_row(_fgl_pixel_t* outData, void* inLine, uintptr_t inWidth, fgl_pixel_encoding inEncoding) {
	_fgl_pixel_t* tempPtr = outData;
	uintptr_t i;
	for(i = 0; i < inWidth; tempPtr++, i++)
		*tempPtr = _fgl_image_decode_pixel(inLine, i, inEncoding);
}

void _fgl_image_decode_row_clut(_fgl_pixel_t* outData, void* inLine, uintptr_t inWidth, _fgl_pixel_t* inCLUT, uintptr_t inEntries) {
	_fgl_pixel_t* tempPtr = outData;
	uintptr_t i;

	_mptr tempLine; tempLine.q = (uint8_t*)inLine;
	if(inEntries == 0) {
		for(i = 0; i < inWidth; tempPtr++, i++)
			*tempPtr = *inCLUT;
	} else if(inEntries <= 2) {
		for(i = 0; i < inWidth; tempPtr += 8, tempLine.q++, i++) {
			tempPtr[0] = inCLUT[(*tempLine.q >> 0) & 0x01];
			tempPtr[1] = inCLUT[(*tempLine.q >> 1) & 0x01];
			tempPtr[2] = inCLUT[(*tempLine.q >> 2) & 0x01];
			tempPtr[3] = inCLUT[(*tempLine.q >> 3) & 0x01];
			tempPtr[4] = inCLUT[(*tempLine.q >> 4) & 0x01];
			tempPtr[5] = inCLUT[(*tempLine.q >> 5) & 0x01];
			tempPtr[6] = inCLUT[(*tempLine.q >> 6) & 0x01];
			tempPtr[7] = inCLUT[(*tempLine.q >> 7) & 0x01];
		}
	} else if(inEntries <= 4) {
		for(i = 0; i < inWidth; tempPtr += 4, tempLine.q++, i++) {
			tempPtr[0] = inCLUT[(*tempLine.q >> 0) & 0x03];
			tempPtr[1] = inCLUT[(*tempLine.q >> 2) & 0x03];
			tempPtr[2] = inCLUT[(*tempLine.q >> 4) & 0x03];
			tempPtr[3] = inCLUT[(*tempLine.q >> 6) & 0x03];
		}
	} else if(inEntries <= 16) {
		for(i = 0; i < inWidth; tempPtr += 2, tempLine.q++, i++) {
			tempPtr[0] = inCLUT[(*tempLine.q >> 0) & 0x0F];
			tempPtr[1] = inCLUT[(*tempLine.q >> 4) & 0x0F];
		}
	} else if(inEntries <= 256) {
		for(i = 0; i < inWidth; tempPtr++, tempLine.q++, i++)
			*tempPtr = inCLUT[*tempLine.q];
	} else if(inEntries <= 65536) {
		for(i = 0; i < inWidth; tempPtr++, tempLine.h++, i++)
			*tempPtr = inCLUT[*tempLine.h];
	}
}



fgl_image_basic* fgl_image_basic_create(uintptr_t inWidth, uintptr_t inHeight) {
	if((inWidth == 0) || (inHeight == 0))
		return NULL;

	fgl_image_basic* tempImage = (fgl_image_basic*)malloc(sizeof(fgl_image_basic));
	if(tempImage == NULL)
		return NULL;
	tempImage->data = (uint32_t*)malloc(inWidth * inHeight << 2);
	if(tempImage->data == NULL) {
		free(tempImage);
		return NULL;
	}

	tempImage->width  = inWidth;
	tempImage->height = inHeight;

	return tempImage;
}

void fgl_image_basic_delete(fgl_image_basic* inImage) {
	if(inImage == NULL)
		return;
	if(inImage->data != NULL)
		free(inImage->data);
	free(inImage);
}



void fgl_image_complex_delete(fgl_image_complex* inImage) {
	if(inImage == NULL)
		return;
	if(!inImage->clut_static && (inImage->clut_data != NULL))
		free(inImage->clut_data);
	if(!inImage->data_static && (inImage->data != NULL))
		free(inImage->data);
	free(inImage);
}



fgl_image_basic* fgl_image_decode(fgl_image_complex* inImage) {
	if(inImage == NULL)
		return NULL;

	fgl_image_basic* tempImage = fgl_image_basic_create(inImage->width, inImage->height);
	if(tempImage == NULL)
		return NULL;

	if(inImage->clut_entries > 0) {
		_fgl_pixel_t* tempCLUT = (_fgl_pixel_t*)malloc(sizeof(_fgl_pixel_t) * inImage->clut_entries);
		if(tempCLUT == NULL) {
			fgl_image_basic_delete(tempImage);
			return NULL;
		}
		_fgl_image_decode_row(tempCLUT, inImage->clut_data, inImage->clut_entries, inImage->encoding);

		_fgl_image_decode_row_clut((_fgl_pixel_t*)tempImage->data, inImage->data, (inImage->width * inImage->height), tempCLUT, inImage->clut_entries);

		free(tempCLUT);
	} else {
		_fgl_image_decode_row((_fgl_pixel_t*)tempImage->data, inImage->data, (inImage->width * inImage->height), inImage->encoding);
	}

	if(inImage->h_flip && !fgl_image_flip_horizontal(tempImage)) {
		fgl_image_basic_delete(tempImage);
		return NULL;
	}

	if(inImage->v_flip && !fgl_image_flip_vertical(tempImage)) {
		fgl_image_basic_delete(tempImage);
		return NULL;
	}

	return tempImage;
}

fgl_image_complex* fgl_image_encode(fgl_image_basic* inImage, uint32_t inEncoding) {
	return NULL;
}



bool fgl_image_flip_horizontal(fgl_image_basic* inImage) {
	if(inImage == NULL)
		return false;

	uintptr_t i, j;
	uint32_t* tempRow = inImage->data;
	for(j = 0; j < inImage->height; j++, tempRow += inImage->width) {
		for(i = 0; i < (inImage->width >> 1); i++)
			swap(tempRow[i], tempRow[inImage->width - i]);
	}

	return true;
}

bool fgl_image_flip_vertical(fgl_image_basic* inImage) {
	if(inImage == NULL)
		return false;

	uintptr_t i, j;
	for(j = 0; j < (inImage->height >> 1); j++) {
		for(i = 0; i < inImage->width; i++)
			swap(inImage->data[(j * inImage->width) + i], inImage->data[((inImage->height - j) * inImage->width) + i]);
	}

	return true;
}



bool fgl_image_scale_half_horizontal(fgl_image_basic* inImage) {
	if(inImage == NULL)
		return false;

	uintptr_t i, j;
	_fgl_pixel_t* tempRow = (_fgl_pixel_t*)inImage->data;
	for(j = 0; j < inImage->height; j++, tempRow += inImage->width) {
		for(i = 0; i < ((inImage->width + 1) >> 1); i++)
			tempRow[i] = _fgl_pixel_blend2(tempRow[i], tempRow[inImage->width - i]);
	}

	inImage->width = (inImage->width + 1) >> 1;

	uint32_t* tempRealloc = (uint32_t*)realloc(inImage->data, (inImage->width * inImage->height << 2));
	if(tempRealloc)
		inImage->data = tempRealloc;

	return true;
}

bool fgl_image_scale_half_vertical(fgl_image_basic* inImage) {
	if(inImage == NULL)
		return false;

	uintptr_t i, j;
	_fgl_pixel_t* tempRow[2] = { (_fgl_pixel_t*)inImage->data, (_fgl_pixel_t*)&inImage->data[(inImage->height - 1) * inImage->width] };
	for(j = 0; j < ((inImage->height + 1) >> 1); j++, tempRow[0] += inImage->width, tempRow[1] -= inImage->width) {
		for(i = 0; i < inImage->width; i++)
			tempRow[0][i] = _fgl_pixel_blend2(tempRow[0][i], tempRow[1][i]);
	}

	inImage->height = (inImage->height + 1) >> 1;

	uint32_t* tempRealloc = (uint32_t*)realloc(inImage->data, (inImage->width * inImage->height << 2));
	if(tempRealloc)
		inImage->data = tempRealloc;

	return true;
}

bool fgl_image_scale_half(fgl_image_basic* inImage) {
	if(inImage == NULL)
		return false;

	uintptr_t i, j;
	_fgl_pixel_t* tempRow[2] = { (_fgl_pixel_t*)inImage->data, (_fgl_pixel_t*)&inImage->data[(inImage->height - 1) * inImage->width] };
	for(j = 0; j < ((inImage->height + 1) >> 1); j++, tempRow[0] += inImage->width, tempRow[1] -= inImage->width) {
		for(i = 0; i < ((inImage->width + 1) >> 1); i++)
			tempRow[0][i] = _fgl_pixel_blend4(tempRow[0][i], tempRow[1][i], tempRow[0][inImage->width - i], tempRow[1][inImage->width - i]);
	}

	inImage->width  = (inImage->width + 1) >> 1;
	inImage->height = (inImage->height + 1) >> 1;

	uint32_t* tempRealloc = (uint32_t*)realloc(inImage->data, (inImage->width * inImage->height << 2));
	if(tempRealloc)
		inImage->data = tempRealloc;

	return true;
}

bool fgl_image_scale(fgl_image_basic* inImage, uintptr_t inWidth, uintptr_t inHeight) {
	if(inImage == NULL)
		return false;
	if((inWidth == 0) || (inHeight == 0)) {
		fgl_image_basic_delete(inImage);
		return true;
	}

	while((((inImage->width + 1) >> 1) >= inWidth) && (((inImage->height + 1) >> 1) >= inHeight))
		fgl_image_scale_half(inImage);
	while(((inImage->width + 1) >> 1) >= inWidth)
		fgl_image_scale_half_horizontal(inImage);
	while(((inImage->height + 1) >> 1) >= inHeight)
		fgl_image_scale_half_vertical(inImage);

	// TODO - Finer grain scaling.

	return false; // Currently only scales to by closest power.
}
