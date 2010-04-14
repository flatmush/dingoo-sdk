#include "fgl_image_tga.h"
#include "fgl_image.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



typedef struct {
	unsigned int alpha    : 4;
	unsigned int h_flip   : 1;
	unsigned int v_flip   : 1;
	unsigned int reserved : 2;
} __attribute__((__packed__)) _tga_flags;

typedef struct {
	uint8_t    ident_size;		  // size of ID field that follows 18 byte header (0 usually)
	uint8_t    clut_type;	  // type of colour map 0=none, 1=has palette
	uint8_t    image_type;		  // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed
	uint16_t   clut_start;	 // first colour map entry in palette
	uint16_t   clut_size;	// number of colours in palette
	uint8_t    clut_bits;	  // number of bits per palette entry 15,16,24,32
	int16_t    x_origin, y_origin;			 // image x origin
	int16_t    width, height;			  // image width in pixels
	uint8_t    bpp;			   // image bits per pixel 8,16,24,32
	_tga_flags flags;		 // image descriptor bits (vh flip bits)
} __attribute__((__packed__)) _tga_header;



inline uintptr_t _fread_rle(void* outData, uintptr_t inSize, uintptr_t inCount, FILE* inStream) {
	if((outData == NULL) || (inStream == NULL))
		return 0;

	uintptr_t i;
	uintptr_t tempPacket;
	uint8_t*  tempData = (uint8_t*)outData;
	for(i = 0; i < inCount; i += (tempPacket & 0x7F)) {
		fread(&tempPacket, 1, 1, inStream);
		if(tempPacket & 0x80) {
			fread(tempData, inSize, 1, inStream);
			memcpy(&tempData[inSize], tempData, ((tempPacket & 0x7F) * inSize));
		} else {
			fread(tempData, inSize, ((tempPacket & 0x7F) + 1), inStream);
		}
		tempData = &tempData[((tempPacket & 0x7F) + 1) * inSize];
	}

	return i;
}

fgl_image_complex* fgl_image_load_tga(FILE* inStream) {
	if(inStream == NULL)
		return NULL;

	_tga_header tempHeader;
	if(fread(&tempHeader, sizeof(_tga_header), 1, inStream) <= 0)
		return NULL;

	fseek(inStream, tempHeader.ident_size, SEEK_CUR);

	fgl_image_complex* tempImage = (fgl_image_complex*)malloc(sizeof(fgl_image_complex));
	if(tempImage == NULL)
		return NULL;

	tempImage->width  = abs(tempHeader.width);
	tempImage->height = abs(tempHeader.height);
	tempImage->stride = tempImage->width;

	uintptr_t tempArea = (tempImage->width * tempImage->height);

	tempImage->clut_static = false;
	tempImage->data_static = false;

	tempImage->encoding = fgl_pixel_encoding_invalid;
	if((tempHeader.image_type & 3) == 3) {
		if(tempHeader.bpp == 64)
			tempImage->encoding = fgl_gray64;
		else if(tempHeader.bpp == 32)
			tempImage->encoding = fgl_gray32;
		else if(tempHeader.bpp == 16)
			tempImage->encoding = fgl_gray16;
		else if(tempHeader.bpp == 8)
			tempImage->encoding = fgl_gray8;
		else if(tempHeader.bpp == 4)
			tempImage->encoding = fgl_gray4;
		else if(tempHeader.bpp == 2)
			tempImage->encoding = fgl_gray2;
		else if(tempHeader.bpp == 1)
			tempImage->encoding = fgl_gray1;
		if(tempHeader.flags.alpha != 0)
			tempImage->encoding = fgl_pixel_encoding_invalid;
	} else {
		if(tempHeader.bpp == 32) {
			if((tempHeader.flags.alpha == 0) || (tempHeader.flags.alpha == 8))
				tempImage->encoding = fgl_bgrx8888;
		} else if((tempHeader.bpp == 24) && (tempHeader.flags.alpha == 0)) {
			tempImage->encoding = fgl_bgr888;
		} else if(tempHeader.bpp == 16) {
			if(tempHeader.flags.alpha == 4)
				tempImage->encoding = fgl_bgrx4444;
			else if(tempHeader.flags.alpha == 1)
				tempImage->encoding = fgl_bgrx5551;
			else if(tempHeader.flags.alpha == 0)
				tempImage->encoding = fgl_bgr565;
		} else if(tempHeader.bpp == 8) {
			if(tempHeader.flags.alpha == 2)
				tempImage->encoding = fgl_bgrx2222;
			else if(tempHeader.flags.alpha == 0)
				tempImage->encoding = fgl_bgr332;
		} else if(tempHeader.bpp == 4) {
			if(tempHeader.flags.alpha == 1)
				tempImage->encoding = fgl_bgrx1111;
			else if(tempHeader.flags.alpha == 0)
				tempImage->encoding = fgl_bgr121;
		}
	}
	if(tempImage->encoding == fgl_pixel_encoding_invalid) {
		free(tempImage);
		return NULL;
	}

	if((tempHeader.image_type > 8) && (((tempHeader.clut_type != 0 ? tempHeader.clut_bits : tempHeader.bpp) & 0x03) != 0)) {
		free(tempImage);
		return NULL;
	}

	if(tempHeader.clut_type != 0) {
		fseek(inStream, ((tempHeader.bpp * tempHeader.clut_start) >> 3), SEEK_CUR);

		tempImage->clut_entries = tempHeader.clut_size;
		tempImage->clut_data    = malloc(tempHeader.bpp * tempHeader.clut_size);
		if(tempImage->clut_data == NULL) {
			free(tempImage);
			return NULL;
		}

		fread(tempImage->clut_data, (tempHeader.bpp * tempHeader.clut_size), 1, inStream);
	} else {
		tempImage->clut_entries = 0;
		tempImage->clut_data    = NULL;
	}

	uintptr_t tempDataBits = (tempHeader.clut_type != 0 ? tempHeader.clut_bits : tempHeader.bpp);
	tempDataBits *= tempArea;
	tempDataBits >>= 3;

	tempImage->data = malloc(tempDataBits);
	if(tempImage->data == NULL) {
		if(tempImage->clut_data != NULL)
			free(tempImage->clut_data);
		free(tempImage);
		return NULL;
	}

	if(tempHeader.image_type > 8)
		_fread_rle(tempImage->data, ((tempHeader.clut_type != 0 ? tempHeader.clut_bits : tempHeader.bpp) >> 3), tempArea, inStream);
	else
		fread(tempImage->data, tempDataBits, 1, inStream);

	tempImage->h_flip = (tempHeader.flags.h_flip != 0);
	if(tempHeader.width < 0)
		tempImage->h_flip = !tempImage->h_flip;


	tempImage->v_flip = (tempHeader.flags.v_flip == 0);
	if(tempHeader.height < 0)
		tempImage->v_flip = !tempImage->v_flip;

	return tempImage;
}
