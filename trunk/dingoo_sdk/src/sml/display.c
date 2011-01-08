#include <stdlib.h>
#include <dingoo/slcd.h>
#include <dingoo/cache.h>
#include <sml/display.h>

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

uint16_t* g_pGameDecodeBuf = NULL;

bool _display_buffer_allocated = false;

uint16_t* _display_lut[16] = {
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
	};

uintptr_t _display_lut_ref[16] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
	};

uint8_t _display_format_size[16] = {
	  0,   4,   8,   4,
	  8,   8,  16,  24,
	  8,  16,  16,  32,
	255, 255, 255, 255
	};

uint8_t*  _display_lut_blend6 = NULL;
uintptr_t _display_lut_ref_blend6 = 0;



inline uint16_t _display_merge4_rgb565(uint16_t inColor0, uint16_t inColor1, uint16_t inColor2, uint16_t inColor3) {
	uint32_t tempR = (((inColor0 + inColor1 + inColor2 + inColor3) >> 2) & 0xF800);
	uint32_t tempG = ((((inColor0 & 0x07E0) + (inColor1 & 0x07E0) + (inColor2 & 0x07E0) + (inColor3 & 0x07E0)) >> 2) & 0x07E0);
	uint32_t tempB = (((inColor0 & 0x001F) + (inColor1 & 0x001F) + (inColor2 & 0x001F) + (inColor3 & 0x001F)) >> 2);
	return (tempR | tempG | tempB);
}

inline uint16_t _display_blend4_rgb565(uint16_t inColor0, uint16_t inColor1, uint16_t inColor2, uint16_t inColor3, uint32_t inXFract, uint32_t inYFract) {
	uint8_t* tempLutX = &_display_lut_blend6[inXFract & 0x0000F000];
	uint8_t* tempLutY = &_display_lut_blend6[inYFract & 0x0000F000];

	uint16_t tempR0 = tempLutX[((inColor0 & 0xF800) >> 10) | ((inColor1 & 0xF800) >> 4)];
	uint16_t tempG0 = tempLutX[((inColor0 & 0x07E0) >> 5) | ((inColor1 & 0x07E0) << 1)];
	uint16_t tempB0 = tempLutX[((inColor0 & 0x001F) << 1) | ((inColor1 & 0x001F) << 7)];

	uint16_t tempR1 = tempLutX[((inColor2 & 0xF800) >> 10) | ((inColor3 & 0xF800) >> 4)];
	uint16_t tempG1 = tempLutX[((inColor2 & 0x07E0) >> 5) | ((inColor3 & 0x07E0) << 1)];
	uint16_t tempB1 = tempLutX[((inColor2 & 0x001F) << 1) | ((inColor3 & 0x001F) << 7)];

	tempR0 = tempLutY[tempR0 | (tempR1 << 6)];
	tempG0 = tempLutY[tempG0 | (tempG1 << 6)];
	tempB0 = tempLutY[tempB0 | (tempB1 << 6)];

	return (((tempR0 << 10) & 0xF800) | (tempG0 << 5) | (tempB0 >> 1));
}

void _display_resize_rgb565(uint16_t* inDst, uint16_t* inSrc, uint16_t inSrcWidth, uint16_t inSrcHeight) {
	uintptr_t j_dst, j_src;
	uint32_t j_fract = ((inSrcHeight - 1) << 16) / SCREEN_HEIGHT;
	uint32_t i_fract = ((inSrcWidth  - 1) << 16) / SCREEN_WIDTH;
	for(j_src = 0, j_dst = 0; j_dst < SCREEN_HEIGHT; j_dst++) {
		uintptr_t s_a = ((j_src >> 16) * inSrcWidth);
		uint16_t j_blend = (j_src & 0xFFFF);
		j_src += j_fract;
		uintptr_t s_b = ((j_src >> 16) * inSrcWidth);
		uintptr_t i_src, i_dst;
		for(i_src = 0, i_dst = 0; i_dst < SCREEN_WIDTH; i_dst++) {
			uint16_t i_blend = (i_src & 0xFFFF);
			uint16_t a = inSrc[s_a + (i_src >> 16)];
			uint16_t c = inSrc[s_b + (i_src >> 16)];
			i_src += i_fract;
			uint16_t b = inSrc[s_a + (i_src >> 16)];
			uint16_t d = inSrc[s_b + (i_src >> 16)];
			inDst[(j_dst * SCREEN_WIDTH) + i_dst] = _display_blend4_rgb565(a, b, c, d, i_blend, j_blend);
		}
	}
}



bool _display_lut_create_blend6() {
	if(_display_lut_blend6 != NULL)
		free(_display_lut_blend6);
	_display_lut_blend6 = (uint8_t*)malloc(65536);
	if(_display_lut_blend6 == NULL)
		return false;
	uintptr_t i, j, k, l, ik, jk;
	for(l = 0, k = 0, ik = 16; k < 16; k++, ik--) {
		for(j = 0; j < 64; j++) {
			jk = j * k;
			for(i = 0; i < 64; i++, l++) {
				_display_lut_blend6[l] = (((i * ik) + jk) >> 4);
			}
		}
	}
	return true;
}



bool _display_lut_create_gray4() {
	if(_display_lut[DISPLAY_FORMAT_GRAY4] != NULL)
		free(_display_lut[DISPLAY_FORMAT_GRAY4]);
	_display_lut[DISPLAY_FORMAT_GRAY4] = (uint16_t*)malloc(32);
	if(_display_lut[DISPLAY_FORMAT_GRAY4] == NULL)
		return false;
	uintptr_t i;
	for(i = 0; i < 16; i++) {
		_display_lut[DISPLAY_FORMAT_GRAY4][i] = ((i << 1) | (i >> 3));
		_display_lut[DISPLAY_FORMAT_GRAY4][i] |= (_display_lut[DISPLAY_FORMAT_GRAY4][i] << 11);
		_display_lut[DISPLAY_FORMAT_GRAY4][i] |= ((i << 2) | (i >> 2)) << 5;
	}
	return true;
}

bool _display_lut_create_gray8() {
	if(_display_lut[DISPLAY_FORMAT_GRAY8] != NULL)
		free(_display_lut[DISPLAY_FORMAT_GRAY8]);
	_display_lut[DISPLAY_FORMAT_GRAY8] = (uint16_t*)malloc(512);
	if(_display_lut[DISPLAY_FORMAT_GRAY8] == NULL)
		return false;
	uintptr_t i;
	for(i = 0; i < 256; i++) {
		_display_lut[DISPLAY_FORMAT_GRAY8][i] = (i >> 3);
		_display_lut[DISPLAY_FORMAT_GRAY8][i] |= (_display_lut[DISPLAY_FORMAT_GRAY8][i] << 11);
		_display_lut[DISPLAY_FORMAT_GRAY8][i] |= (i >> 2) << 5;
	}
	return true;
}

bool _display_lut_create_rgb332() {
	if(_display_lut[DISPLAY_FORMAT_RGB332] != NULL)
		free(_display_lut[DISPLAY_FORMAT_RGB332]);
	_display_lut[DISPLAY_FORMAT_RGB332] = (uint16_t*)malloc(512);
	if(_display_lut[DISPLAY_FORMAT_RGB332] == NULL)
		return false;
	uintptr_t r, g, b, i;
	for(i = 0, r = 0; r < 8; r++) {
		for(g = 0; g < 8; g++) {
			for(b = 0; b < 4; b++, i++) {
				_display_lut[DISPLAY_FORMAT_RGB332][i] = ((r << 2) | (r >> 1)) << 11;
				_display_lut[DISPLAY_FORMAT_RGB332][i] |= ((g << 3) | g) << 5;
				_display_lut[DISPLAY_FORMAT_RGB332][i] |= ((b << 3) | (b << 1) | (b >> 1));
			}
		}
	}
	return true;
}

bool _display_lut_create_rgba2222() {
	if(_display_lut[DISPLAY_FORMAT_RGBA2222] != NULL)
		free(_display_lut[DISPLAY_FORMAT_RGB332]);
	_display_lut[DISPLAY_FORMAT_RGBA2222] = (uint16_t*)malloc(512);
	if(_display_lut[DISPLAY_FORMAT_RGBA2222] == NULL)
		return false;
	uintptr_t r, g, b, i;
	for(i = 0, r = 0; r < 4; r++) {
		for(g = 0; g < 4; g++) {
			for(b = 0; b < 4; b++, i++) {
				_display_lut[DISPLAY_FORMAT_RGB332][i << 2] = ((b << 3) | (b << 1) | (b >> 1));
				_display_lut[DISPLAY_FORMAT_RGB332][i << 2] |= _display_lut[DISPLAY_FORMAT_RGB332][i]  << 11;
				_display_lut[DISPLAY_FORMAT_RGB332][i << 2] |= ((g << 4) | (g << 2) | g) << 5;
				_display_lut[DISPLAY_FORMAT_RGB332][(i << 2) + 1] = _display_lut[DISPLAY_FORMAT_RGB332][i << 2];
				_display_lut[DISPLAY_FORMAT_RGB332][(i << 2) + 2] = _display_lut[DISPLAY_FORMAT_RGB332][i << 2];
				_display_lut[DISPLAY_FORMAT_RGB332][(i << 2) + 3] = _display_lut[DISPLAY_FORMAT_RGB332][i << 2];
			}
		}
	}
	return true;
}

bool _display_lut_create_rgba5551() {
	if(_display_lut[DISPLAY_FORMAT_RGBA5551] != NULL)
		free(_display_lut[DISPLAY_FORMAT_RGBA5551]);
	_display_lut[DISPLAY_FORMAT_RGBA5551] = (uint16_t*)malloc(65536);
	if(_display_lut[DISPLAY_FORMAT_RGBA5551] == NULL)
		return false;
	uintptr_t r, g, b, i;
	for(i = 0, r = 0; r < 32; r++) {
		for(g = 0; g < 32; g++) {
			for(b = 0; b < 32; b++, i++)
				_display_lut[DISPLAY_FORMAT_RGBA5551][i] = (r << 11) | (((g << 1) | (g >> 4)) << 5) | b;
		}
	}
	return true;
}

bool _display_lut_create_rgba4444() {
	if(_display_lut[DISPLAY_FORMAT_RGBA4444] != NULL)
		free(_display_lut[DISPLAY_FORMAT_RGBA4444]);
	_display_lut[DISPLAY_FORMAT_RGBA4444] = (uint16_t*)malloc(8192);
	if(_display_lut[DISPLAY_FORMAT_RGBA4444] == NULL)
		return false;
	uintptr_t r, g, b, i;
	for(i = 0, r = 0; r < 16; r++) {
		for(g = 0; g < 16; g++) {
			for(b = 0; b < 16; b++, i++)
				_display_lut[DISPLAY_FORMAT_RGBA5551][i] = ((r << 1) | (r >> 3)) << 11;
				_display_lut[DISPLAY_FORMAT_RGBA5551][i] |= ((g << 2) | (g >> 2)) << 5;
				_display_lut[DISPLAY_FORMAT_RGBA5551][i] |= ((b << 1) | (b >> 3));
		}
	}
	return true;
}



inline uintptr_t _display_buffer_size(display* inDisplay) {
	if(inDisplay == NULL)
		return 0;
	uintptr_t tempBufferSize;
	tempBufferSize = (inDisplay->stride * inDisplay->height);
	tempBufferSize *= _display_format_size[inDisplay->flags & 0xF];
	tempBufferSize = ((tempBufferSize + 7) >> 3);
	if(tempBufferSize & 3)
		tempBufferSize += (4 - (tempBufferSize & 3));
	return tempBufferSize;
}



display* display_create(uint16_t inWidth, uint16_t inHeight, uint16_t inStride, uint32_t inFlags, void* inBuffer, void* inPalette) {
	if((inWidth == 0) || (inHeight == 0))
		return NULL;
	if(((inFlags & 0xF) == 0) || ((inFlags & 0xF) > DISPLAY_FORMAT_LAST))
		return NULL;
	if((inStride != 0) && (inStride < inWidth))
		return NULL;

	if((inFlags & DISPLAY_STRETCH) == 0) {
		if(inFlags & DISPLAY_ROTATE) {
			if((inWidth > SCREEN_HEIGHT) || (inHeight > SCREEN_WIDTH))
				return NULL;
		} else if((inWidth > SCREEN_WIDTH) || (inHeight > SCREEN_HEIGHT))
			return NULL;
	}

	display* tempDisplay = (display*)malloc(sizeof(display));
	if(tempDisplay == NULL)
		return NULL;
	tempDisplay->width   = inWidth;
	tempDisplay->height  = inHeight;
	tempDisplay->stride  = (inStride == 0 ? inWidth : inStride);
	tempDisplay->flags   = inFlags;
	tempDisplay->buffer  = NULL;
	tempDisplay->palette = NULL;

	if(_display_lut_ref[tempDisplay->flags & 0xF] == 0) {
		switch(tempDisplay->flags & 0xF) {
			case DISPLAY_FORMAT_GRAY4:
				_display_lut_create_gray4();
				break;
			case DISPLAY_FORMAT_GRAY8:
				_display_lut_create_gray8();
				break;
			case DISPLAY_FORMAT_RGB332:
				_display_lut_create_rgb332();
				break;
			case DISPLAY_FORMAT_RGBA2222:
				_display_lut_create_rgba2222();
				break;
			case DISPLAY_FORMAT_RGBA5551:
				_display_lut_create_rgba5551();
				break;
			case DISPLAY_FORMAT_RGBA4444:
				_display_lut_create_rgba4444();
				break;
		}
	}
	_display_lut_ref[tempDisplay->flags & 0xF]++;

	if((((inFlags & 0xF) == DISPLAY_FORMAT_CLUT4) || ((inFlags & 0xF) == DISPLAY_FORMAT_CLUT8)) && (inPalette != NULL)) {
		if((uintptr_t)inPalette & 0x3) {
			display_delete(tempDisplay);
			return NULL;
		}
		tempDisplay->palette = inPalette;
		tempDisplay->flags |= DISPLAY_PALETTE_STATIC;
	} else {
		switch(tempDisplay->flags & 0xF) {
			case DISPLAY_FORMAT_CLUT4:
				tempDisplay->palette = malloc(32);
				if(tempDisplay->palette == NULL) {
					display_delete(tempDisplay);
					return NULL;
				}
				break;
			case DISPLAY_FORMAT_CLUT8:
				tempDisplay->palette = malloc(512);
				if(tempDisplay->palette == NULL) {
					display_delete(tempDisplay);
					return NULL;
				}
				break;
			default:
				tempDisplay->palette = NULL;
		}
	}

	if(inBuffer != NULL) {
		if((uintptr_t)inBuffer & 0x3) {
			display_delete(tempDisplay);
			return NULL;
		}
		if(inBuffer == g_pGameDecodeBuf) {
			if(_display_buffer_allocated) {
				display_delete(tempDisplay);
				return NULL;
			}
			_display_buffer_allocated = true;
		}
		tempDisplay->buffer = inBuffer;
		tempDisplay->flags |= DISPLAY_BUFFER_STATIC;
	} else {
		uintptr_t tempBuffSize = _display_buffer_size(tempDisplay);
		if(inFlags & DISPLAY_BUFFER_STATIC) {
			if((_display_buffer_allocated) || (tempBuffSize > (SCREEN_WIDTH * SCREEN_HEIGHT * 2))) {
				display_delete(tempDisplay);
				return NULL;
			}
			tempDisplay->buffer = g_pGameDecodeBuf;
			_display_buffer_allocated = true;
		} else {
			tempDisplay->buffer = malloc(tempBuffSize);
			if(tempDisplay->buffer == NULL) {
				display_delete(tempDisplay);
				return NULL;
			}
		}
	}

	return tempDisplay;
}

display* display_create_fastest() {
	return display_create(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH, (DISPLAY_FORMAT_RGB565 | DISPLAY_BUFFER_STATIC), NULL, NULL);
}

void display_delete(display* inDisplay) {
	if(inDisplay == NULL)
		return;
	if((inDisplay->flags & DISPLAY_PALETTE_STATIC) == 0)
		free(inDisplay->palette);
	if((inDisplay->flags & DISPLAY_BUFFER_STATIC) == 0)
		free(inDisplay->buffer);
	else if(inDisplay->buffer == g_pGameDecodeBuf)
		_display_buffer_allocated = false;
	_display_lut_ref[inDisplay->flags & 0xF]--;
	if(_display_lut_ref[inDisplay->flags & 0xF] == 0) {
		free(_display_lut[inDisplay->flags & 0xF]);
		_display_lut[inDisplay->flags & 0xF] = NULL;
	}
	free(inDisplay);
}



void display_clear(display* inDisplay) {
	if(inDisplay == NULL)
		return;
	uint32_t* tempPtr = inDisplay->buffer;
	uint32_t* tempEnd = (uint32_t*)((uintptr_t)tempPtr + _display_buffer_size(inDisplay));
	while(tempPtr < tempEnd)
		*(tempPtr++) = 0x00000000;
}



void* display_flip(display* inDisplay) {
	if(inDisplay == NULL)
		return NULL;

	if((inDisplay->width == SCREEN_WIDTH) &&
       (inDisplay->height == SCREEN_HEIGHT) &&
       (inDisplay->stride == inDisplay->width) &&
       ((inDisplay->flags & (DISPLAY_HFLIP | DISPLAY_VFLIP | DISPLAY_ROTATE)) == 0)) {
		if((inDisplay->flags & 0xF) == DISPLAY_FORMAT_RGB565) {
			uint32_t* tempDispBuff32 = (uint32_t*)_lcd_get_frame();
			uint32_t* tempDispEnd32  = (uint32_t*)((uintptr_t)tempDispBuff32 + _display_buffer_size(inDisplay));
			uint32_t* tempDrawBuff32 = (uint32_t*)inDisplay->buffer;
			while(tempDispBuff32 < tempDispEnd32) {
				*(tempDispBuff32++) = *(tempDrawBuff32++);
				*(tempDispBuff32++) = *(tempDrawBuff32++);
				*(tempDispBuff32++) = *(tempDrawBuff32++);
				*(tempDispBuff32++) = *(tempDrawBuff32++);
				*(tempDispBuff32++) = *(tempDrawBuff32++);
				*(tempDispBuff32++) = *(tempDrawBuff32++);
				*(tempDispBuff32++) = *(tempDrawBuff32++);
				*(tempDispBuff32++) = *(tempDrawBuff32++);
			}
		} else if(((inDisplay->flags & 0xF) == DISPLAY_FORMAT_RGB888) || ((inDisplay->flags & 0xF) == DISPLAY_FORMAT_RGBA8888)) {
			uint8_t   tempRed, tempGreen, tempBlue;
			uint16_t* tempDispBuff16 = (uint16_t*)_lcd_get_frame();
			uint16_t* tempDispEnd16  = (uint16_t*)((uintptr_t)tempDispBuff16 + _display_buffer_size(inDisplay));
			uint8_t*  tempDrawBuff8  = (uint8_t*)inDisplay->buffer;
			if((inDisplay->flags & 0xF) == DISPLAY_FORMAT_RGB888) {
				while(tempDispBuff16 < tempDispEnd16) {
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
				}
			} else {
				while(tempDispBuff16 < tempDispEnd16) {
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					tempDrawBuff8++;
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					tempDrawBuff8++;
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					tempDrawBuff8++;
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					tempDrawBuff8++;
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					tempDrawBuff8++;
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					tempDrawBuff8++;
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					tempDrawBuff8++;
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
					tempRed   = *(tempDrawBuff8++);
					tempGreen = *(tempDrawBuff8++);
					tempBlue  = *(tempDrawBuff8++);
					tempDrawBuff8++;
					*(tempDispBuff16++) = ((tempRed & 0xF8) << 8) | ((tempGreen & 0xFC) << 3) | ((tempBlue & 0xF8) >> 3);
				}
			}
		} else {
			uint16_t* tempPalette = (inDisplay->palette == NULL ? _display_lut[inDisplay->flags & 0xF] : inDisplay->palette);
			uint16_t* tempDispBuff16 = (uint16_t*)_lcd_get_frame();
			uint16_t* tempDispEnd16  = (uint16_t*)((uintptr_t)tempDispBuff16 + _display_buffer_size(inDisplay));
			uint16_t* tempDrawBuff16 = (uint16_t*)inDisplay->buffer;
			uint8_t*  tempDrawBuff8  = (uint8_t*)inDisplay->buffer;
			switch(inDisplay->flags & 0xF) {
				case DISPLAY_FORMAT_GRAY4:
				case DISPLAY_FORMAT_CLUT4:
					while(tempDispBuff16 < tempDispEnd16) {
						*(tempDispBuff16++) = tempPalette[*tempDrawBuff8 >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++) & 0xF];
						*(tempDispBuff16++) = tempPalette[*tempDrawBuff8 >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++) & 0xF];
						*(tempDispBuff16++) = tempPalette[*tempDrawBuff8 >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++) & 0xF];
						*(tempDispBuff16++) = tempPalette[*tempDrawBuff8 >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++) & 0xF];
						*(tempDispBuff16++) = tempPalette[*tempDrawBuff8 >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++) & 0xF];
						*(tempDispBuff16++) = tempPalette[*tempDrawBuff8 >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++) & 0xF];
						*(tempDispBuff16++) = tempPalette[*tempDrawBuff8 >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++) & 0xF];
						*(tempDispBuff16++) = tempPalette[*tempDrawBuff8 >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++) & 0xF];
					}
					break;
				case DISPLAY_FORMAT_GRAY8:
				case DISPLAY_FORMAT_CLUT8:
				case DISPLAY_FORMAT_RGB332:
				case DISPLAY_FORMAT_RGBA2222:
					while(tempDispBuff16 < tempDispEnd16) {
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++)];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++)];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++)];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++)];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++)];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++)];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++)];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff8++)];
					}
					break;
				case DISPLAY_FORMAT_RGBA5551:
					while(tempDispBuff16 < tempDispEnd16) {
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 1];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 1];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 1];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 1];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 1];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 1];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 1];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 1];
					}
					break;
				case DISPLAY_FORMAT_RGBA4444:
					while(tempDispBuff16 < tempDispEnd16) {
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 4];
						*(tempDispBuff16++) = tempPalette[*(tempDrawBuff16++) >> 4];
					}
					break;
				default:
					return NULL;
			}
		}
	} else {
		// TODO - Pain in the arse display modes.
		if(((inDisplay->flags & 0xF) == DISPLAY_FORMAT_RGB565) && (inDisplay->stride != inDisplay->width) && (inDisplay->width == 320) && (inDisplay->height == 240)) {
			uint16_t* tempDispBuff16    = (uint16_t*)_lcd_get_frame();
			uint16_t* tempDrawBuff16    = (uint16_t*)inDisplay->buffer;
			uint16_t* tempDrawLineEnd16 = (uint16_t*)((uintptr_t)tempDrawBuff16 + (inDisplay->width << 1));
			uint16_t* tempDrawBuffEnd16 = (uint16_t*)((uintptr_t)tempDrawBuff16 + _display_buffer_size(inDisplay));
			while(tempDrawBuff16 < tempDrawBuffEnd16) {
				while(tempDrawBuff16 < tempDrawLineEnd16)
					*(tempDispBuff16++) = *(tempDrawBuff16++);
				tempDrawBuff16    += (inDisplay->stride - inDisplay->width);
				tempDrawLineEnd16 += inDisplay->stride;
			}
		} else if(inDisplay->flags & DISPLAY_STRETCH) {
			if((inDisplay->flags & 0xF) == DISPLAY_FORMAT_RGB565) {
				if((inDisplay->flags & DISPLAY_FILTER_LINEAR) && (inDisplay->width == (SCREEN_WIDTH << 1)) && (inDisplay->height == (SCREEN_HEIGHT << 1))) {
					uint16_t* tempDispBuff16 = (uint16_t*)_lcd_get_frame();
					uint16_t* tempDispLineEnd16 = (uint16_t*)((uintptr_t)tempDispBuff16 + (SCREEN_WIDTH << 1));
					uint16_t* tempDispBuffEnd16 = (uint16_t*)((uintptr_t)tempDispBuff16 + _display_buffer_size(inDisplay));
					uint16_t* tempDrawBuff16a = (uint16_t*)inDisplay->buffer;
					uint16_t* tempDrawBuff16b = (uint16_t*)((uintptr_t)tempDrawBuff16a + (inDisplay->stride << 1));
					uint16_t  tempPixel0, tempPixel1, tempPixel2, tempPixel3;
					while(tempDispBuff16 < tempDispBuffEnd16) {
						while(tempDispBuff16 < tempDispLineEnd16) {
							tempPixel0 = *(tempDrawBuff16a++);
							tempPixel1 = *(tempDrawBuff16a++);
							tempPixel2 = *(tempDrawBuff16b++);
							tempPixel3 = *(tempDrawBuff16b++);
							*(tempDispBuff16++) = _display_merge4_rgb565(tempPixel0, tempPixel1, tempPixel2, tempPixel3);
						}
						tempDrawBuff16a += inDisplay->stride;
						tempDrawBuff16b += inDisplay->stride;
						tempDispLineEnd16 += SCREEN_WIDTH;
					}
				} else {
					if(inDisplay->flags & DISPLAY_FILTER_LINEAR) {
						if(_display_lut_blend6 == NULL)
							_display_lut_create_blend6();
						_display_resize_rgb565((uint16_t*)_lcd_get_frame(), (uint16_t*)inDisplay->buffer, inDisplay->width, inDisplay->height);
					} else {
						uint16_t* tempDispBuff16 = (uint16_t*)_lcd_get_frame();
						uint16_t* tempDrawBuff16 = (uint16_t*)inDisplay->buffer;
						uint16_t* tempDrawLine16;
						uint32_t i_disp, i_draw;
						uint32_t j_disp, j_draw;
						uint32_t i_draw_dx = ((inDisplay->width - 1) << 16) / (SCREEN_WIDTH - 1);
						uint32_t j_draw_dx = ((inDisplay->height - 1) << 16) / (SCREEN_HEIGHT - 1);
						for(j_disp = 0, j_draw = 0; j_disp < SCREEN_HEIGHT; j_disp++, j_draw += j_draw_dx) {
							tempDrawLine16 = &tempDrawBuff16[(j_draw >> 16) * inDisplay->stride];
							for(i_disp = 0, i_draw = 0; i_disp < SCREEN_WIDTH; i_disp++, i_draw += i_draw_dx)
								*(tempDispBuff16++) = tempDrawLine16[i_draw >> 16];
						}
					}
				}
			} else {
				return NULL;
			}
		} else {
			return NULL;
		}
	}

	__dcache_writeback_all();
	_lcd_set_frame();
	return inDisplay->buffer;
}
