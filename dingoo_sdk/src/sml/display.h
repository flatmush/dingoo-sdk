#ifndef __display_h__
#define __display_h__
/**
=sml/display.h=
Controls the LCD display.
*/
#include <stdbool.h>
#include <stdint.h>

/**
==Macros==
===Display format flags===
  * *DISPLAY`_`FORMAT`_`NONE*: Reset all the display format flags
  * *DISPLAY`_`FORMAT`_`GRAY4*: Use 4-color grayscale
  * *DISPLAY`_`FORMAT`_`GRAY8*: Use 8-color grayscale
  * *DISPLAY`_`FORMAT`_`CLUT4*: Use a 4-color palette
  * *DISPLAY`_`FORMAT`_`CLUT8*: Use an 8-color palette
  * *DISPLAY`_`FORMAT`_`RGB332*: Use a 332-color palette
  * *DISPLAY`_`FORMAT`_`RGB565*: Use a 565-color palette
  * *DISPLAY`_`FORMAT`_`RGB888*: Use an 888-color palette
  * *DISPLAY`_`FORMAT`_`RGBA2222*: Use a 2,222-color palette with transparency
  * *DISPLAY`_`FORMAT`_`RGBA5551*: Use a 5,551-color palette with transparency
  * *DISPLAY`_`FORMAT`_`RGBA4444*: Use a 4,444-color palette with transparency
  * *DISPLAY`_`FORMAT`_`RGBA8888*: Use an 8,888-color palette with transparency
  * *DISPLAY`_`FORMAT`_`LAST*: Use the highest format (8,888-color palette with transparency)
*/
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

/**
===Display mode flags===
  * *DISPLAY`_`HFLIP*: Flip the display horizontally
  * *DISPLAY`_`VFLIP*: Flip the display vertically
  * *DISPLAY`_`ROTATE*: Rotate the display
  * *DISPLAY`_`STRETCH*: Stretch the display
  * *DISPLAY`_`FILTER`_`NEAREST*: Stretch using a nearest-neighbor resize algorithm (usually ugly but very fast)
  * *DISPLAY`_`FILTER`_`LINEAR*: Stretch using a linear resize algorithm (usually nicer than nearest-neighbor, but slower)
  * *DISPLAY`_`BUFFER`_`STATIC*: TODO
  * *DISPLAY`_`PALETTE`_`STATIC*: TODO
*/
#define DISPLAY_HFLIP          (1 << 4)
#define DISPLAY_VFLIP          (1 << 5)
#define DISPLAY_ROTATE         (1 << 6)
#define DISPLAY_STRETCH        (1 << 7)
#define DISPLAY_FILTER_NEAREST (0 << 8)
#define DISPLAY_FILTER_LINEAR  (1 << 8)
#define DISPLAY_BUFFER_STATIC  (1 << 9)
#define DISPLAY_PALETTE_STATIC (1 << 10)

/**
==Structs==
===display===
Members:
  * _uint16_t_ *width*: Stores the display's width in pixels
  * _uint16_t_ *height*: Stores the display's height in pixels
  * _uint16_t_ *stride*: TODO
  * _uint32_t_ *flags*: Stores all the set flags for this display
  * _void_`*` *buffer*: Stores a pointer to the buffer in memory
  * _void_`*` *palette*: Stores a pointer to the palette in memory
*/
typedef struct {
	uint16_t width, height, stride;
	uint32_t flags;
	void*    buffer;
	void*    palette;
} display;

/**
==Functions==
  * [#display display]`*` *display`_`create*( _uint16_t_ width, _uint16_t_ height, _uint16_t_ stride, _uint32_t_ flags, _void_`*` buffer, _void_`*` palette ): Returns a pointer to the created display with the specified attributes.
  * [#display display]`*` *display`_`create_fastest*( _void_ ): Returns a pointer to a display with Dingoo defaults in mind (320x240 screen, full stride, 565-color static buffer) with no buffer or palette specified.
  * _void_ *display`_`delete*( [#display display]`*` ): Frees the specified display from memory.
  * _void_ *display`_`clear*( [#display display]`*` ): Clears the contents of the specified display.
  * _void_ *display`_`flip*( [#display display]`*` ): Pushes the specified display into view on the LCD.
**/
extern display* display_create(uint16_t inWidth, uint16_t inHeight, uint16_t inStride, uint32_t inFlags, void* inBuffer, void* inPalette);
extern display* display_create_fastest();
extern void     display_delete(display* inDisplay);
extern void     display_clear(display* inDisplay);
extern void*    display_flip(display* inDisplay);

// TODO - Support non-native palette formats.

#endif
