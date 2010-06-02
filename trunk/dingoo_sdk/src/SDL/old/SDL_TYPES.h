#ifndef __SDL_TYPES_h__
#define __SDL_TYPES_h__

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t Uint32;
typedef int32_t  Sint32;
typedef uint16_t Uint16;
typedef int16_t  Sint16;
typedef uint8_t  Uint8;
typedef int8_t   Sint8;

typedef bool SDL_bool;


typedef struct {
    Uint8 major;
    Uint8 minor;
    Uint8 patch;
} SDL_version;



typedef struct {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 unused;
} SDL_Color;



typedef struct {
	Sint16 x, y;
	Uint16 w, h;
} SDL_Rect;



typedef struct {
	int        ncolors;
	SDL_Color* colors;
} SDL_Palette;



typedef struct SDL_PixelFormat {
	SDL_Palette* palette;
	Uint8        BitsPerPixel;
	Uint8        BytesPerPixel;
	Uint8        Rloss, Gloss, Bloss, Aloss;
	Uint8        Rshift, Gshift, Bshift, Ashift;
	Uint32       Rmask, Gmask, Bmask, Amask;
	Uint32       colorkey;
	Uint8        alpha;
} SDL_PixelFormat;



typedef struct SDL_Surface {
	Uint32           flags;
	SDL_PixelFormat* format;
	int              w, h;
	Uint16           pitch;
	void*            pixels;
	SDL_Rect         clip_rect;
	int              refcount;
} SDL_Surface;

#define SDL_SWSURFACE   (1 << 0)
#define SDL_HWSURFACE   (1 << 1)
#define SDL_ASYNCBLIT   (1 << 2)
#define SDL_ANYFORMAT   (1 << 3)
#define SDL_HWPALETTE   (1 << 4)
#define SDL_DOUBLEBUF   (1 << 5)
#define SDL_FULLSCREEN  (1 << 6)
#define SDL_OPENGL      (1 << 7)
#define SDL_OPENGLBLIT  (1 << 8)
#define SDL_RESIZABLE   (1 << 9)
#define SDL_HWACCEL     (1 << 10)
#define SDL_SRCCOLORKEY (1 << 11)
#define SDL_RLEACCEL    (1 << 12)
#define SDL_SRCALPHA    (1 << 13)
#define SDL_PREALLOC    (1 << 14)



typedef struct {
	Uint32           hw_available:1;
	Uint32           wm_available:1;
	Uint32           blit_hw:1;
	Uint32           blit_hw_CC:1;
	Uint32           blit_hw_A:1;
	Uint32           blit_sw:1;
	Uint32           blit_sw_CC:1;
	Uint32           blit_sw_A:1;
	Uint32           blit_fill;
	Uint32           video_mem;
	SDL_PixelFormat* vfmt;
} SDL_VideoInfo;



typedef struct{
	Uint32  format;
	int     w, h;
	int     planes;
	Uint16* pitches;
	Uint8** pixels;
	Uint32  hw_overlay:1;
} SDL_Overlay;

#endif
