#ifndef __sml_display_h__
#define __sml_display_h__
/*!
	\file display.h
	\brief Defines flags and functions to manage the LCD display.
*/
#include <stdbool.h>
#include <stdint.h>

#define DISPLAY_FORMAT_NONE     0 /*!< Native display format */
#define DISPLAY_FORMAT_GRAY4    1 /*!< 4-bit grayscale */
#define DISPLAY_FORMAT_GRAY8    2 /*!< 8-bit grayscale */
#define DISPLAY_FORMAT_CLUT4    3 /*!< 4-bit color look-up table */
#define DISPLAY_FORMAT_CLUT8    4 /*!< 8-bit color look-up table */
#define DISPLAY_FORMAT_RGB332   5 /*!< 8 bpp, 3 in red, 3 in green, and 2 in blue */
#define DISPLAY_FORMAT_RGB565   6 /*!< 16 bpp, 5 in red, 6 in green, and 5 in blue */
#define DISPLAY_FORMAT_RGB888   7 /*!< 32 bpp, 8 in red, 8 in green, and 8 in blue */
#define DISPLAY_FORMAT_RGBA2222 8 /*!< 8 bpp, 2 in red, 2 in green, and 2 in blue, ignore 2 bits */
#define DISPLAY_FORMAT_RGBA5551 9 /*!< 16 bpp, 5 in red, 5 in green, and 5 in blue, ignore one bit */
#define DISPLAY_FORMAT_RGBA4444 10 /*!< 16 bpp, 4 in red, 4 in green, and 4 in blue, ignore 4 bits */ 
#define DISPLAY_FORMAT_RGBA8888 11 /*!< 32 bpp, 8 in red, 8 in green, and 8 in blue, ignore 8 bits */ 
#define DISPLAY_FORMAT_LAST     DISPLAY_FORMAT_RGBA8888 /*!< use RGBA8888 format */

#define DISPLAY_HFLIP          (1 << 4) /*!< Flip the display horizontally */
#define DISPLAY_VFLIP          (1 << 5) /*!< Flip the display vertically */
#define DISPLAY_ROTATE         (1 << 6) /*!< Rotate the display */
#define DISPLAY_STRETCH        (1 << 7) /*!< Stretch the display */
#define DISPLAY_FILTER_NEAREST (0 << 8) /*!< Resize the display using nearest-neighbor algorithm (usually ugly but fast) */
#define DISPLAY_FILTER_LINEAR  (1 << 8) /*!< Resize the display using linear algorithm (usually nice but slower) */
#define DISPLAY_BUFFER_STATIC  (1 << 9) /*!< Use a preallocated buffer, so it won't be freed from memory when the display is freed */
#define DISPLAY_PALETTE_STATIC (1 << 10) /*!< Use a preallocated palette, so it won't be freed from memory when the display is freed */

/*! Stores information about the display.
*/
typedef struct {
	uint16_t width; /*!< width of the display */
	uint16_t height; /*!< height of the display */
	uint16_t stride; /*!< the byte offset between one scan line and the next scan line in that block of memory */
	uint32_t flags; /*!< formatting flags to apply to this display */
	void*    buffer; /*!< the location of the display buffer */
	void*    palette; /*!< the location of the palette buffer */
} display;

/*! Creates a display with the specified attributes.
	\param inWidth the width of the display
	\param inHeight the height of the dislpay
	\param inStride holds the byte offset between one scan line and the next scan line in that block of memory
	\param inFlags flags to apply to the display
	\param inBuffer the address of the dislpay buffer to use
	\param inPalette the address of the pallete to use
	\return a configured display
*/
extern display* display_create(uint16_t inWidth, uint16_t inHeight, uint16_t inStride, uint32_t inFlags, void* inBuffer, void* inPalette);

/*! Creates a display with Dingoo defaults: 320x240 screen, RGB565 (16-bit color), and a static display buffer.
	\return a default configured display
*/
extern display* display_create_fastest();

/*! Frees the specified display from memory.
	\param inDisplay the display to remove from memory (does not free the display buffer or palette if they are set to static)
*/
extern void     display_delete(display* inDisplay);

/*! Blanks the specified display with black.
	\param inDisplay the display to flush with black
*/
extern void     display_clear(display* inDisplay);

/*! Prints the specified display to the LCD for viewing.
	\param inDisplay the display to display
*/
extern void*    display_flip(display* inDisplay);

// TODO - Support non-native palette formats.

#endif
