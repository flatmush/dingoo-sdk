#ifndef __display_h__
#define __display_h__

#include <stdbool.h>
#include <stdint.h>

#define DISPLAY_FORMAT_NONE     0
#define DISPLAY_FORMAT_GRAY4    1
#define DISPLAY_FORMAT_GRAY8    2
#define DISPLAY_FORMAT_CLUT4    3
#define DISPLAY_FORMAT_CLUT8    4
#define DISPLAY_FORMAT_RGB332   5
#define DISPLAY_FORMAT_RGB565   6
#define DISPLAY_FORMAT_RGB888   7
#define DISPLAY_FORMAT_RGBA2222 8
#define DISPLAY_FORMAT_RGBA5551 9
#define DISPLAY_FORMAT_RGBA4444 10
#define DISPLAY_FORMAT_RGBA8888 11
#define DISPLAY_FORMAT_LAST     DISPLAY_FORMAT_RGBA8888

#define DISPLAY_HFLIP          (1 << 4)
#define DISPLAY_VFLIP          (1 << 5)
#define DISPLAY_ROTATE         (1 << 6)
#define DISPLAY_STRETCH        (1 << 7)
#define DISPLAY_FILTER_NEAREST (0 << 8)
#define DISPLAY_FILTER_LINEAR  (1 << 8)
#define DISPLAY_BUFFER_STATIC  (1 << 9)
#define DISPLAY_PALETTE_STATIC (1 << 10)



typedef struct {
	uint16_t width, height, stride;
	uint32_t flags;
	void*    buffer;
	void*    palette;
} display;



extern display* display_create(uint16_t inWidth, uint16_t inHeight, uint16_t inStride, uint32_t inFlags, void* inBuffer, void* inPalette);
extern display* display_create_fastest();
extern void     display_delete(display* inDisplay);

extern void     display_clear(display* inDisplay);

extern void*    display_flip(display* inDisplay);

// TODO - Support non-native palette formats.

#endif
