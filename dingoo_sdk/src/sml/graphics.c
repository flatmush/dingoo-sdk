#include <stdlib.h>
#include <stdio.h>
#include <sml/graphics.h>
#include <sml/display.h>
#include <sml/fixmath.h>

#define _GFX_CLIP_NONE 0
#define _GFX_CLIP_PART 1
#define _GFX_CLIP_ALL  2

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define ABS(x)    ((x) < 0 ? -(x) : (x))



inline uint16_t _abs_s16(int16_t inVal) { return (inVal >= 0 ? inVal : -inVal); }

inline gfx_color gfx_color_rgb(uint8_t inRed, uint8_t inGreen, uint8_t inBlue) {
		return ((inRed & 0xF8) << 8) | ((inGreen & 0xFC) << 3) | ((inBlue & 0xF8) >> 3);
}

inline gfx_color gfx_color_rand(uint8_t inRed, uint8_t inGreen, uint8_t inBlue) {
	return gfx_color_rgb((inRed + (rand() % (256 - inRed))), (inGreen + (rand() % (256 - inGreen))), (inBlue + (rand() % (256 - inBlue))));
}



gfx_texture  gfx_frame_buffer = { NULL, 0, 0 };
gfx_texture* gfx_render_target = &gfx_frame_buffer;
display*     _gfx_display = NULL;

bool gfx_init(display* inDisplay) {
	_gfx_display = inDisplay;
	gfx_frame_buffer.address = inDisplay->buffer;
	gfx_frame_buffer.width   = inDisplay->width;
	gfx_frame_buffer.height  = inDisplay->height;
	return true;
}

void gfx_term() {
	_gfx_display = NULL;
	gfx_frame_buffer.address = NULL;
}



bool gfx_render_target_set(gfx_texture* inTexture) {
	if(inTexture == NULL) {
		gfx_render_target = &gfx_frame_buffer;
		return true;
	}

	if((inTexture->width & 0xF) != 0)
		return false;
	gfx_render_target = inTexture;
	return true;
}

void gfx_render_target_clear(gfx_color inColor) {
	uint32_t  tempPixelColor = (inColor << 16) | inColor;
	uint32_t* tempLineStart = (uint32_t*)gfx_render_target->address;
	uint32_t* tempBlockEnd;
	tempBlockEnd = (uint32_t*)((uintptr_t)gfx_render_target->address + (gfx_render_target->width * gfx_render_target->height * 2));
	while(tempLineStart < tempBlockEnd) {
		*(tempLineStart++) = tempPixelColor;
		*(tempLineStart++) = tempPixelColor;
		*(tempLineStart++) = tempPixelColor;
		*(tempLineStart++) = tempPixelColor;
	}
}

void gfx_render_target_swap() {
	if(gfx_render_target != &gfx_frame_buffer)
		return;
	gfx_frame_buffer.address = display_flip(_gfx_display);
}


gfx_texture* gfx_tex_allocate(uint32_t width, uint32_t height) {
	gfx_texture* tempTexture = NULL;
	tempTexture = (gfx_texture*)malloc(sizeof(gfx_texture) + (width * height * 2));
	if (tempTexture == NULL) {
		return NULL;
	}
	tempTexture->address = (void*)((uintptr_t)tempTexture + sizeof(gfx_texture));
	tempTexture->width = width;
	tempTexture->height = height;

	return tempTexture;
}

gfx_texture* _gfx_tex_load_tga(FILE* tempFile) {
	if(tempFile == NULL)
		return NULL;

	uint8_t  tga_ident_size;
	uint8_t  tga_color_map_type;
	uint8_t  tga_image_type;
	uint16_t tga_color_map_start;
	uint16_t tga_color_map_length;
	uint8_t  tga_color_map_bpp;
	uint16_t tga_origin_x;
	uint16_t tga_origin_y;
	uint16_t tga_width;
	uint16_t tga_height;
	uint8_t  tga_bpp;
	uint8_t  tga_descriptor;

	fread(&tga_ident_size, 1, 1, tempFile);
	fread(&tga_color_map_type, 1, 1, tempFile);
	fread(&tga_image_type, 1, 1, tempFile);
	fread(&tga_color_map_start, 2, 1, tempFile);
	fread(&tga_color_map_length, 2, 1, tempFile);
	fread(&tga_color_map_bpp, 1, 1, tempFile);
	fread(&tga_origin_x, 2, 1, tempFile);
	fread(&tga_origin_y, 2, 1, tempFile);
	fread(&tga_width, 2, 1, tempFile);
	fread(&tga_height, 2, 1, tempFile);
	fread(&tga_bpp, 1, 1, tempFile);
	fread(&tga_descriptor, 1, 1, tempFile);

	bool upside_down = (tga_descriptor & 0x20) > 0;

	gfx_texture* tempTexture = (gfx_texture*)malloc(sizeof(gfx_texture) + (tga_width * tga_height * 2));
	if(tempTexture == NULL) {
		fclose(tempFile);
		return NULL;
	}
	tempTexture->address = (void*)((uintptr_t)tempTexture + sizeof(gfx_texture));
	tempTexture->width = tga_width;
	tempTexture->height = tga_height;

	uintptr_t i;
	uint8_t tempColor[3];
	uint16_t* tempTexPtr = tempTexture->address;
	for(i = 0; i < (tga_width * tga_height); i++) {
		fread(&tempColor[2], 1, 1, tempFile);
		fread(&tempColor[1], 1, 1, tempFile);
		fread(&tempColor[0], 1, 1, tempFile);
		tempTexPtr[upside_down ? i : ((tga_height - 1 - (i / tga_width)) * tga_width + i % tga_width)] = gfx_color_rgb(tempColor[0], tempColor[1], tempColor[2]);
	}
	fclose(tempFile);

	return tempTexture;
}

gfx_texture* gfx_tex_load_tga(const char* inPath) {
	if(inPath == NULL)
		return NULL;

	FILE* tempFile = fopen(inPath, "rb");
	return _gfx_tex_load_tga(tempFile);
}

gfx_texture* gfx_tex_load_tga_from_buffer(uint8_t* buffer, size_t size) {
	if(buffer == NULL)
		return NULL;

	FILE* tempFile = fmemopen(buffer, size, "rb");
	return _gfx_tex_load_tga(tempFile);
}

bool gfx_tex_save_tga(const char* inPath, gfx_texture* inTexture) {
	if((inPath == NULL) || (inTexture == NULL) || (inTexture->address == NULL))
		return false;

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	uint8_t  tga_ident_size = 0;
	uint8_t  tga_color_map_type = 0;
	uint8_t  tga_image_type = 2;
	uint16_t tga_color_map_start = 0;
	uint16_t tga_color_map_length = 0;
	uint8_t  tga_color_map_bpp = 0;
	uint16_t tga_origin_x = 0;
	uint16_t tga_origin_y = 0;
	int16_t  tga_width = inTexture->width;
	int16_t  tga_height = inTexture->height;
	uint8_t  tga_bpp = 24;
	uint8_t  tga_descriptor = 0x20;

	fwrite(&tga_ident_size, 1, 1, tempFile);
	fwrite(&tga_color_map_type, 1, 1, tempFile);
	fwrite(&tga_image_type, 1, 1, tempFile);
	fwrite(&tga_color_map_start, 2, 1, tempFile);
	fwrite(&tga_color_map_length, 2, 1, tempFile);
	fwrite(&tga_color_map_bpp, 1, 1, tempFile);
	fwrite(&tga_origin_x, 2, 1, tempFile);
	fwrite(&tga_origin_y, 2, 1, tempFile);
	fwrite(&tga_width, 2, 1, tempFile);
	fwrite(&tga_height, 2, 1, tempFile);
	fwrite(&tga_bpp, 1, 1, tempFile);
	fwrite(&tga_descriptor, 1, 1, tempFile);

	uintptr_t i;
	uint8_t tempColor[3];
	uint16_t* tempTexPtr = inTexture->address;
	for(i = 0; i < (tga_width * tga_height); i++) {
		tempColor[0]  = ((tempTexPtr[i] >> 8) & 0xF8);
		tempColor[0] |= (tempColor[0] >> 5);
		tempColor[1]  = ((tempTexPtr[i] >> 3) & 0xFC);
		tempColor[1] |= (tempColor[1] >> 6);
		tempColor[2]  = ((tempTexPtr[i] << 3) & 0xF8);
		tempColor[2] |= (tempColor[2] >> 5);
		fwrite(&tempColor[2], 1, 1, tempFile);
		fwrite(&tempColor[1], 1, 1, tempFile);
		fwrite(&tempColor[0], 1, 1, tempFile);
	}
	fclose(tempFile);

	return true;
}



inline void gfx_point_draw(int16_t inX, int16_t inY, gfx_color inColor) {
	if((inX < 0) || (inX >= gfx_render_target->width) || (inY < 0) || (inY >= gfx_render_target->height))
		return;
	uint16_t* tempPtr = (uint16_t*)(gfx_render_target->address);
	tempPtr[(inY * gfx_render_target->width) + inX] = inColor;
}



void _gfx_line_draw_h(int16_t inX0, int16_t inX1, int16_t inY, gfx_color inColor) {
	if((inY < 0) || (inY >= gfx_render_target->height))
		return;
	if(inX0 == inX1)
		return;

	if(inX0 > inX1) {
		inX0 = inX0 ^ inX1;
		inX1 = inX0 ^ inX1;
		inX0 = inX0 ^ inX1;
	}

	if(inX0 >= gfx_render_target->width)
		return;
	if(inX1 < 0)
		return;

	if(inX0 < 0)
		inX0 = 0;
	if(inX1 > gfx_render_target->width)
		inX1 = gfx_render_target->width;
	inX1 -= inX0;

	if(((inX0 | inX1) & 1) == 0) {
		uint32_t* tempStart32 = (uint32_t*)((uintptr_t)gfx_render_target->address + (((inY * gfx_render_target->width) + inX0) << 1));
		uint32_t* tempEnd32   = (uint32_t*)((uintptr_t)tempStart32 + (inX1 << 1));
		uint32_t  tempColor32 = (inColor << 16) | inColor;
		while(tempStart32 < tempEnd32)
			*(tempStart32++) = tempColor32;
		return;
	}

	uint16_t* tempStart16 = (uint16_t*)((uintptr_t)gfx_render_target->address + (((inY * gfx_render_target->width) + inX0) << 1));
	uint16_t* tempEnd16   = (uint16_t*)((uintptr_t)tempStart16 + (inX1 << 1));
	while(tempStart16 < tempEnd16)
		*(tempStart16++) = inColor;
}

void _gfx_line_draw_v(int16_t inX, int16_t inY0, int16_t inY1, gfx_color inColor) {
	if((inX < 0) || (inX >= gfx_render_target->width))
		return;
	if(inY0 == inY1)
		return;

	if(inY0 > inY1) {
		inY0 = inY0 ^ inY1;
		inY1 = inY0 ^ inY1;
		inY0 = inY0 ^ inY1;
	}

	if(inY0 >= gfx_render_target->height)
		return;
	if(inY1 < 0)
		return;

	if(inY0 < 0)
		inY0 = 0;
	if(inY1 > gfx_render_target->height)
		inY1 = gfx_render_target->height;
	inY1 -= inY0;

	uint16_t* tempStart = (uint16_t*)((uintptr_t)gfx_render_target->address + (((inY0 * gfx_render_target->width) + inX) << 1));
	uint16_t* tempEnd   = (uint16_t*)((uintptr_t)tempStart + ((inY1 * gfx_render_target->width) << 1));
	while(tempStart < tempEnd) {
		*tempStart = inColor;
		tempStart += gfx_render_target->width;
	}
}

void _gfx_line_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, gfx_color inColor) {
	uint16_t* tempBuffer = (uint16_t*)gfx_render_target->address;
	int32_t tempDrawX, tempDrawY;
	if(ABS(inY1 - inY0) >= ABS(inX1 - inX0)) {
		int32_t tempX[2] = { inX0, inX1 };
		if(inY0 > inY1) {
			tempX[0] = inX1;
			tempX[1] = inX0;
			inY0 = inY0 ^ inY1;
			inY1 = inY0 ^ inY1;
			inY0 = inY0 ^ inY1;
		} else if(inY0 == inY1)
			return;
		tempX[0] <<= 16;
		tempX[1] <<= 16;
		tempX[0] += 0x00008000;
		tempX[1] += 0x00008000;
		int32_t tempDx = (tempX[1] - tempX[0]) / (inY1 - inY0);
		tempDrawY = inY0;
		tempDrawX = tempX[0];
		tempBuffer = &tempBuffer[tempDrawY * gfx_render_target->width];
		for(; tempDrawY < inY1; tempDrawY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempDrawX += tempDx)
			tempBuffer[tempDrawX >> 16] = inColor;
	} else {
		int32_t tempY[2] = { inY0, inY1 };
		if(inX0 > inX1) {
			inX0 = inX0 ^ inX1;
			inX1 = inX0 ^ inX1;
			inX0 = inX0 ^ inX1;
			tempY[0] = inY1;
			tempY[1] = inY0;
		} else if(inX0 == inX1)
			return;
		tempY[0] <<= 16;
		tempY[1] <<= 16;
		tempY[0] += 0x00008000;
		tempY[1] += 0x00008000;
		int32_t tempDy = (tempY[1] - tempY[0]) / (inX1 - inX0);
		tempDrawX = inX0;
		tempDrawY = tempY[0];
		for(; tempDrawX < inX1; tempDrawX++, tempDrawY += tempDy)
			tempBuffer[((tempDrawY >> 16) * gfx_render_target->width) + tempDrawX] = inColor;
	}
}

void _gfx_line_draw_clipped(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, gfx_color inColor) {
	uint16_t* tempBuffer = (uint16_t*)gfx_render_target->address;
	int32_t tempDrawX, tempDrawY;
	if(ABS(inY1 - inY0) >= ABS(inX1 - inX0)) {
		int32_t tempX[2] = { inX0, inX1 };
		if(inY0 > inY1) {
			tempX[0] = inX1;
			tempX[1] = inX0;
			inY0 = inY0 ^ inY1;
			inY1 = inY0 ^ inY1;
			inY0 = inY0 ^ inY1;
		} else if(inY0 == inY1)
			return;
		tempX[0] <<= 16;
		tempX[1] <<= 16;
		tempX[0] += 0x00008000;
		tempX[1] += 0x00008000;
		int32_t tempDx = (tempX[1] - tempX[0]) / (inY1 - inY0);
		tempDrawY = inY0;
		tempDrawX = tempX[0];
		if(tempDrawY < 0) {
			tempDrawX -= (tempDrawY * tempDx);
			tempDrawY = 0;
		}
		if(inY1 > gfx_render_target->height)
			inY1 = gfx_render_target->height;
		tempBuffer = &tempBuffer[tempDrawY * gfx_render_target->width];
		if(tempDx >= 0) {
			for(; (tempDrawY < inY1) && (tempDrawX < 0); tempDrawY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempDrawX += tempDx);
			for(; (tempDrawY < inY1) && (tempDrawX < (gfx_render_target->width << 16)); tempDrawY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempDrawX += tempDx)
				tempBuffer[tempDrawX >> 16] = inColor;
		} else {
			for(; (tempDrawY < inY1) && (tempDrawX >= (gfx_render_target->width << 16)); tempDrawY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempDrawX += tempDx);
			for(; (tempDrawY < inY1) && (tempDrawX >= 0); tempDrawY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempDrawX += tempDx)
				tempBuffer[tempDrawX >> 16] = inColor;
		}
	} else {
		int32_t tempY[2] = { inY0, inY1 };
		if(inX0 > inX1) {
			inX0 = inX0 ^ inX1;
			inX1 = inX0 ^ inX1;
			inX0 = inX0 ^ inX1;
			tempY[0] = inY1;
			tempY[1] = inY0;
		} else if(inX0 == inX1)
			return;
		tempY[0] <<= 16;
		tempY[1] <<= 16;
		tempY[0] += 0x00008000;
		tempY[1] += 0x00008000;
		int32_t tempDy = (tempY[1] - tempY[0]) / (inX1 - inX0);
		tempDrawX = inX0;
		tempDrawY = tempY[0];
		if(tempDrawX < 0) {
			tempDrawY -= (tempDrawX * tempDy);
			tempDrawX = 0;
		}
		if(inX1 > gfx_render_target->width)
			inX1 = gfx_render_target->width;
		if(tempDy >= 0) {
			for(; (tempDrawX < inX1) && (tempDrawY < 0); tempDrawX++, tempDrawY += tempDy);
			for(; (tempDrawX < inX1) && (tempDrawY < (gfx_render_target->height << 16)); tempDrawX++, tempDrawY += tempDy)
				tempBuffer[((tempDrawY >> 16) * gfx_render_target->width) + tempDrawX] = inColor;
		} else {
			for(; (tempDrawX < inX1) && (tempDrawY >= (gfx_render_target->height << 16)); tempDrawX++, tempDrawY += tempDy);
			for(; (tempDrawX < inX1) && (tempDrawY >= 0); tempDrawX++, tempDrawY += tempDy)
				tempBuffer[((tempDrawY >> 16) * gfx_render_target->width) + tempDrawX] = inColor;
		}
	}
}

uint8_t _gfx_line_clip(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1) {
	if((inX0 < 0) && (inX1 < 0))
		return _GFX_CLIP_ALL;
	if((inX0 >= gfx_render_target->width) && (inX1 > gfx_render_target->width))
		return _GFX_CLIP_ALL;
	if((inY0 < 0) && (inY1 < 0))
		return _GFX_CLIP_ALL;
	if((inY0 >= gfx_render_target->height) && (inY1 > gfx_render_target->height))
		return _GFX_CLIP_ALL;

	if((inX0 < 0) || (inX1 < 0))
		return _GFX_CLIP_PART;
	if((inX0 >= gfx_render_target->width) || (inX1 > gfx_render_target->width))
		return _GFX_CLIP_PART;
	if((inY0 < 0) || (inY1 < 0))
		return _GFX_CLIP_PART;
	if((inY0 >= gfx_render_target->height) || (inY1 > gfx_render_target->height))
		return _GFX_CLIP_PART;

	return _GFX_CLIP_NONE;
}

void gfx_line_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, gfx_color inColor) {
	if(inY0 == inY1) {
		if((inY0 >= 0) && (inY0 < gfx_render_target->height))
			_gfx_line_draw_h(inX0, inX1, inY0, inColor);
		return;
	}
	if(inX0 == inX1) {
		if((inX0 >= 0) && (inX0 < gfx_render_target->width))
			_gfx_line_draw_v(inX0, inY0, inY1, inColor);
		return;
	}
	switch(_gfx_line_clip(inX0, inY0, inX1, inY1)) {
		case _GFX_CLIP_NONE:
			_gfx_line_draw(inX0, inY0, inX1, inY1, inColor);
			break;
		case _GFX_CLIP_PART:
			_gfx_line_draw_clipped(inX0, inY0, inX1, inY1, inColor);
			break;
		case _GFX_CLIP_ALL:
			break;
	}
}



void gfx_rect_draw(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, gfx_color inColor) {
	if(((inX + inWidth) <= 0) || (inX >= gfx_render_target->width))
		return;
	if(((inY + inHeight) <= 0) || (inY >= gfx_render_target->height))
		return;

	if(inWidth < 0) {
		inX += inWidth;
		inWidth = -inWidth;
	}
	if(inHeight < 0) {
		inY += inHeight;
		inHeight = -inHeight;
	}

	uint16_t* tempBuffer = (uint16_t*)gfx_render_target->address;
	uintptr_t i, j;
	j = 0;
	for(j = inY, i = (inX > 0 ? inX : 0); (i < gfx_render_target->width) && (i < (inX + inWidth)); i++)
		tempBuffer[(j * gfx_render_target->width) + i] = inColor;
	for(j = (inY + 1); (j < gfx_render_target->height) && (j < (inY + inHeight - 1)); j++) {
		if(inX >= 0)
			tempBuffer[(j * gfx_render_target->width) + inX] = inColor;
		if((inX + inWidth - 1) < gfx_render_target->width)
			tempBuffer[(j * gfx_render_target->width) + (inX + inWidth - 1)] = inColor;
	}
	for(j = (inY + inHeight - 1), i = (inX > 0 ? inX : 0); (i < gfx_render_target->width) && (i < (inX + inWidth)); i++)
		tempBuffer[(j * gfx_render_target->width) + i] = inColor;
}

void gfx_rect_fill_draw(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, gfx_color inColor) {
	if(((inX + inWidth) <= 0) || (inX >= gfx_render_target->width))
		return;
	if(((inY + inHeight) <= 0) || (inY >= gfx_render_target->height))
		return;

	if(inWidth < 0) {
		inX += inWidth;
		inWidth = -inWidth;
	}
	if(inHeight < 0) {
		inY += inHeight;
		inHeight = -inHeight;
	}
	if(inX < 0) {
		inWidth += inX;
		inX = 0;
	}
	if((inX + inWidth) > gfx_render_target->width)
		inWidth = (gfx_render_target->width - inX);
	if(inY < 0) {
		inHeight += inY;
		inY = 0;
	}
	if((inY + inHeight) > gfx_render_target->height)
		inHeight = (gfx_render_target->height - inY);

	uint16_t* tempBuffer = (uint16_t*)gfx_render_target->address;
	uintptr_t i, j;
	for(j = inY; (j < gfx_render_target->height) && (j < (inY + inHeight)); j++) {
		for(i = inX; (i < gfx_render_target->width) && (i < (inX + inWidth)); i++) {
			tempBuffer[(j * gfx_render_target->width) + i] = inColor;
		}
	}
}



inline uint8_t _gfx_tri_clip(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, int16_t inX2, int16_t inY2) {
	bool tempClipX[3];
	bool tempClipY[3];
	tempClipX[0] = ((inX0 < 0) || (inX0 > gfx_render_target->width));
	tempClipX[1] = ((inX1 < 0) || (inX1 > gfx_render_target->width));
	tempClipX[2] = ((inX2 < 0) || (inX2 > gfx_render_target->width));
	tempClipY[0] = ((inY0 < 0) || (inY0 > gfx_render_target->height));
	tempClipY[1] = ((inY1 < 0) || (inY1 > gfx_render_target->height));
	tempClipY[2] = ((inY2 < 0) || (inY2 > gfx_render_target->height));

	if(tempClipX[0] && tempClipX[1] && tempClipX[2])
		return _GFX_CLIP_ALL;
	if(tempClipY[0] && tempClipY[1] && tempClipY[2])
		return _GFX_CLIP_ALL;
	if(tempClipX[0] || tempClipX[1] || tempClipX[2])
		return _GFX_CLIP_PART;
	if(tempClipY[0] || tempClipY[1] || tempClipY[2])
		return _GFX_CLIP_PART;
	return _GFX_CLIP_NONE;
}

void gfx_tri_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, int16_t inX2, int16_t inY2, gfx_color inColor) {
	gfx_line_draw(inX0, inY0, inX1, inY1, inColor);
	gfx_line_draw(inX1, inY1, inX2, inY2, inColor);
	gfx_line_draw(inX2, inY2, inX0, inY0, inColor);
}

void _gfx_tri_fill_draw_clipped(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, int16_t inX2, int16_t inY2, gfx_color inColor) {
	int32_t tempX[3] = { inX0, inX1, inX2 };
	int32_t i;
	for(i = 0; i < 3; i++) {
		tempX[i] <<= 16;
		tempX[i] += 0x00008000;
	}

	if(inY0 > inY1) {
		tempX[0] = tempX[0] ^ tempX[1];
		tempX[1] = tempX[0] ^ tempX[1];
		tempX[0] = tempX[0] ^ tempX[1];
		inY0 = inY0 ^ inY1;
		inY1 = inY0 ^ inY1;
		inY0 = inY0 ^ inY1;
	}
	if(inY0 > inY2) {
		tempX[0] = tempX[0] ^ tempX[2];
		tempX[2] = tempX[0] ^ tempX[2];
		tempX[0] = tempX[0] ^ tempX[2];
		inY0 = inY0 ^ inY2;
		inY2 = inY0 ^ inY2;
		inY0 = inY0 ^ inY2;
	}
	if(inY1 > inY2) {
		tempX[1] = tempX[1] ^ tempX[2];
		tempX[2] = tempX[1] ^ tempX[2];
		tempX[1] = tempX[1] ^ tempX[2];
		inY1 = inY1 ^ inY2;
		inY2 = inY1 ^ inY2;
		inY1 = inY1 ^ inY2;
	}

	int32_t tempDx[3];
	if((inY1 - inY0) > 0)
		tempDx[0] = (tempX[1] - tempX[0]) / (inY1 - inY0);
	else
		tempDx[0] = tempX[1] - tempX[0];
	if((inY2 - inY0) > 0)
		tempDx[1] = (tempX[2] - tempX[0]) / (inY2 - inY0);
	else
		tempDx[1] = 0;
	if((inY2 - inY1) > 0)
		tempDx[2] = (tempX[2] - tempX[1]) / (inY2 - inY1);
	else
		tempDx[2] = 0;

	int32_t tempStartX = tempX[0];
	int32_t tempStartY = inY0;
	int32_t tempEndX   = tempX[0];
	uint16_t* tempBuffer = (uint16_t*)(gfx_render_target->address);
	tempBuffer = &tempBuffer[inY0 * gfx_render_target->width];
	if(tempDx[0] > tempDx[1]) {
		for(; tempStartY < inY1; tempStartY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempStartX += tempDx[1], tempEndX += tempDx[0]) {
			if((tempStartY < 0) || (tempStartY >= gfx_render_target->height))
				continue;
			for(i = MAX(0, (tempStartX >> 16)); i < MIN(gfx_render_target->width, (tempEndX >> 16)); i++)
				tempBuffer[i] = inColor;
		}
		tempEndX = tempX[1];
		for(; tempStartY < inY2; tempStartY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempStartX += tempDx[1], tempEndX += tempDx[2]) {
			if((tempStartY < 0) || (tempStartY >= gfx_render_target->height))
				continue;
			for(i = MAX(0, (tempStartX >> 16)); i < MIN(gfx_render_target->width, (tempEndX >> 16)); i++)
				tempBuffer[i] = inColor;
		}
	} else {
		for(; tempStartY < inY1; tempStartY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempStartX += tempDx[0], tempEndX += tempDx[1]) {
			if((tempStartY < 0) || (tempStartY >= gfx_render_target->height))
				continue;
			for(i = MAX(0, (tempStartX >> 16)); i < MIN(gfx_render_target->width, (tempEndX >> 16)); i++)
				tempBuffer[i] = inColor;
		}
		tempStartX = tempX[1];
		tempStartY = inY1;
		tempBuffer = (uint16_t*)(gfx_render_target->address);
		tempBuffer = &tempBuffer[inY1 * gfx_render_target->width];
		for(; tempStartY < inY2; tempStartY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempStartX += tempDx[2], tempEndX += tempDx[1]) {
			if((tempStartY < 0) || (tempStartY >= gfx_render_target->height))
				continue;
			for(i = MAX(0, (tempStartX >> 16)); i < MIN(gfx_render_target->width, (tempEndX >> 16)); i++)
				tempBuffer[i] = inColor;
		}
	}
}

void _gfx_tri_fill_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, int16_t inX2, int16_t inY2, gfx_color inColor) {
	int32_t tempX[3] = { inX0, inX1, inX2 };

	int32_t i;
	for(i = 0; i < 3; i++) {
		tempX[i] <<= 16;
		tempX[i] += 0x00008000;
	}

	if(inY0 > inY1) {
		tempX[0] = tempX[0] ^ tempX[1];
		tempX[1] = tempX[0] ^ tempX[1];
		tempX[0] = tempX[0] ^ tempX[1];
		inY0 = inY0 ^ inY1;
		inY1 = inY0 ^ inY1;
		inY0 = inY0 ^ inY1;
	}
	if(inY0 > inY2) {
		tempX[0] = tempX[0] ^ tempX[2];
		tempX[2] = tempX[0] ^ tempX[2];
		tempX[0] = tempX[0] ^ tempX[2];
		inY0 = inY0 ^ inY2;
		inY2 = inY0 ^ inY2;
		inY0 = inY0 ^ inY2;
	}
	if(inY1 > inY2) {
		tempX[1] = tempX[1] ^ tempX[2];
		tempX[2] = tempX[1] ^ tempX[2];
		tempX[1] = tempX[1] ^ tempX[2];
		inY1 = inY1 ^ inY2;
		inY2 = inY1 ^ inY2;
		inY1 = inY1 ^ inY2;
	}

	int32_t tempDx[3];
	if((inY1 - inY0) > 0)
		tempDx[0] = (tempX[1] - tempX[0]) / (inY1 - inY0);
	else
		tempDx[0] = tempX[1] - tempX[0];
	if((inY2 - inY0) > 0)
		tempDx[1] = (tempX[2] - tempX[0]) / (inY2 - inY0);
	else
		tempDx[1] = 0;
	if((inY2 - inY1) > 0)
		tempDx[2] = (tempX[2] - tempX[1]) / (inY2 - inY1);
	else
		tempDx[2] = 0;

	int32_t tempStartX = tempX[0];
	int32_t tempStartY = inY0;
	int32_t tempEndX   = tempX[0];
	uint16_t* tempBuffer = (uint16_t*)(gfx_render_target->address);
	tempBuffer = &tempBuffer[inY0 * gfx_render_target->width];
	if(tempDx[0] > tempDx[1]) {
		for(; tempStartY < inY1; tempStartY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempStartX += tempDx[1], tempEndX += tempDx[0]) {
			for(i = (tempStartX >> 16); i < (tempEndX >> 16); i++)
				tempBuffer[i] = inColor;
		}
		tempEndX = tempX[1];
		for(; tempStartY < inY2; tempStartY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempStartX += tempDx[1], tempEndX += tempDx[2]) {
			for(i = (tempStartX >> 16); i < (tempEndX >> 16); i++)
				tempBuffer[i] = inColor;
		}
	} else {
		for(; tempStartY < inY1; tempStartY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempStartX += tempDx[0], tempEndX += tempDx[1]) {
			for(i = (tempStartX >> 16); i < (tempEndX >> 16); i++)
				tempBuffer[i] = inColor;
		}
		tempStartX = tempX[1];
		tempStartY = inY1;
		tempBuffer = (uint16_t*)(gfx_render_target->address);
		tempBuffer = &tempBuffer[inY1 * gfx_render_target->width];
		for(; tempStartY < inY2; tempStartY++, tempBuffer = &tempBuffer[gfx_render_target->width], tempStartX += tempDx[2], tempEndX += tempDx[1]) {
			for(i = (tempStartX >> 16); i < (tempEndX >> 16); i++)
				tempBuffer[i] = inColor;
		}
	}
}

void gfx_tri_fill_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, int16_t inX2, int16_t inY2, gfx_color inColor) {
	switch(_gfx_tri_clip(inX0, inY0, inX1, inY1, inX2, inY2)) {
		case _GFX_CLIP_NONE:
			_gfx_tri_fill_draw(inX0, inY0, inX1, inY1, inX2, inY2, inColor);
			break;
		case _GFX_CLIP_PART:
			_gfx_tri_fill_draw_clipped(inX0, inY0, inX1, inY1, inX2, inY2, inColor);
			break;
		case _GFX_CLIP_ALL:
			break;
	}
}


void gfx_tex_draw(int16_t inX, int16_t inY, gfx_texture* inTexture) {
	if(inTexture == NULL)
		return;

	uint16_t* tempBuffer = gfx_render_target->address;
	uint16_t* tempTexture = inTexture->address;

	int16_t x, y, i, j;
	for(j = 0, y = inY; (j < inTexture->height) && (y < gfx_render_target->height); j++, y++) {
		for(i = 0, x = inX; (i < inTexture->width) && (x < gfx_render_target->width); i++, x++) {
			tempBuffer[(y * gfx_render_target->width) + x] = tempTexture[(j * inTexture->width) + i];
		}
	}
}



gfx_font* gfx_font_load(const char* inPath, gfx_color inKey) {
	gfx_texture* tempTexture = gfx_tex_load_tga(inPath);
	if (tempTexture == NULL)
		return NULL;

	gfx_font* tempFont = (gfx_font*)malloc(sizeof(gfx_font));
	if (tempFont == NULL)
		return NULL;

	tempFont->texture = tempTexture;
	tempFont->colorKey = inKey;

	return tempFont;
}

gfx_font* gfx_font_load_from_buffer(uint8_t* buffer, size_t size, gfx_color inKey) {
	gfx_texture* tempTexture = gfx_tex_load_tga_from_buffer(buffer, size);
	if (tempTexture == NULL)
		return NULL;

	gfx_font* tempFont = (gfx_font*)malloc(sizeof(gfx_font));
	if (tempFont == NULL)
		return NULL;

	tempFont->texture = tempTexture;
	tempFont->colorKey = inKey;

	return tempFont;
}

void gfx_font_delete(gfx_font* inFont) {
	if (inFont == NULL)
		return;

	if (inFont->texture == NULL)
		free(inFont->texture);

	free(inFont);
}



uint16_t gfx_font_width(gfx_font* inFont, char* inString) {
	if((inFont == NULL) || (inString == NULL) || (inFont->texture == NULL))
		return 0;
	uintptr_t i, tempCur, tempMax;
	for(i = 0, tempCur = 0, tempMax = 0; inString[i] != '\0'; i++) {
		if(inString[i] == '\t')
			tempCur += 4;
		else if((inString[i] == '\r') || (inString[i] == '\n'))
			tempCur = 0;
		else
			tempCur++;
		if(tempCur > tempMax) tempMax = tempCur;
	}
	tempMax *= (inFont->texture->width >> 4);
	return tempMax;
}

uint16_t gfx_font_height(gfx_font* inFont) {
	if(inFont == NULL  || inFont->texture == NULL)
		return 0;
	return (inFont->texture->height >> 4);
}

void gfx_font_print_char(int16_t inX, int16_t inY, gfx_font* inFont, char inChar) {
	char tempStr[2] = { inChar, '\0' };
	gfx_font_print(inX, inY, inFont, tempStr);
}

void gfx_font_print(int16_t inX, int16_t inY, gfx_font* inFont, char* inString) {
	if((inFont == NULL) || (inString == NULL) || (inFont->texture == NULL))
		return;

	uint16_t* tempBuffer = gfx_render_target->address;
	uint16_t* tempFont = inFont->texture->address;
	uint8_t*  tempChar;
	int16_t   tempX = inX;
	int16_t   tempY = inY;
	uintptr_t i, j, x, y;

	for(tempChar = (uint8_t*)inString; *tempChar != '\0'; tempChar++) {
		if(*tempChar == ' ') {
			tempX += (inFont->texture->width >> 4);
			continue;
		}
		if(*tempChar == '\t') {
			tempX += ((inFont->texture->width >> 4) << 2);
			continue;
		}
		if(*tempChar == '\r') {
			tempX = inX;
			continue;
		}
		if(*tempChar == '\n') {
			tempX = inX;
			tempY += (inFont->texture->height >> 4);
			continue;
		}
		for(j = ((*tempChar >> 4) * (inFont->texture->height >> 4)), y = tempY; (j < (((*tempChar >> 4) + 1) * (inFont->texture->height >> 4))) && (y < gfx_render_target->height); j++, y++) {
			for(i = ((*tempChar & 0x0F) * (inFont->texture->width >> 4)), x = tempX; (i < (((*tempChar & 0x0F) + 1) * (inFont->texture->width >> 4))) && (x < gfx_render_target->width); i++, x++) {
				if (tempFont[(j * inFont->texture->width) + i] != inFont->colorKey) {
					tempBuffer[(y * gfx_render_target->width) + x] = tempFont[(j * inFont->texture->width) + i];
				}
			}
		}
		tempX += (inFont->texture->width >> 4);
	}
}

void gfx_font_print_center(int16_t inY, gfx_font* inFont, char* inString) {
	int16_t tempX = (gfx_render_target->width - gfx_font_width(inFont, inString)) >> 1;
	gfx_font_print(tempX, inY, inFont, inString);
}

void gfx_font_print_fromright(int16_t inX, int16_t inY, gfx_font* inFont, char* inString) {
	int16_t tempX = inX - gfx_font_width(inFont, inString);
	gfx_font_print(tempX, inY, inFont, inString);
}
