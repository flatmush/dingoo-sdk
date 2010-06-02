#include "SDL.h"
#include <string.h>
#include <slcd.h>



unsigned short* g_pGameDecodeBuf = NULL;

bool           _SDL_VideoInitialized = false;
SDL_Surface*   _SDL_VideoSurface = NULL;
SDL_VideoInfo* _SDL_VideoInfo   = NULL;
char*          _SDL_VideoDriverName = "slcd";

int _SDL_VideoInit() {
	// TODO - Create _SDL_VideoSurface.
	// TODO - Create _SDL_VideoInfo.
	// TODO - Clear the lcd buffer and set the screen.
	_SDL_VideoInitialized = true;
	return 0;
}

void _SDL_VideoTerm() {
	free(_SDL_VideoSurface);
	free(_SDL_VideoInfo);
	_SDL_VideoInitialized = false;
}



SDL_Surface* SDL_GetVideoSurface() {
	if(!_SDL_VideoInitialized)
		return NULL;
	return _SDL_VideoSurface;
}

SDL_VideoInfo* SDL_GetVideoInfo() {
	if(!_SDL_VideoInitialized)
		return NULL;
	return _SDL_VideoInfo;
}

char* SDL_VideoDriverName(char* namebuf, int maxlen) {
	if(!_SDL_VideoInitialized || (namebuf == NULL))
		return NULL;
	strncpy(namebuf, _SDL_VideoDriverName, maxlen);
	return namebuf;
}

SDL_Rect** SDL_ListModes(SDL_PixelFormat* format, Uint32 flags) {
	// TODO - Create a list of compatible modes for input format.
}

int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags) {
	if(!_SDL_VideoInitialized)
		return 0;

	if((width <= 0) || (width > 640))
		return 0;
	if((height <= 0) || (height > 480))
		return 0;
	if((bpp != 8) && (bpp != 15) && (bpp != 16) && (bpp != 24) && (bpp != 32))
		return 0;
	if(flags & (SDL_ASYNCBLIT | SDL_ANYFORMAT | SDL_OPENGL | SDL_OPENGLBLIT | SDL_HWACCEL | SDL_HWPALETTE))
		return 0;

	if(flags & SDL_HWSURFACE) {
		if(bpp != 16)
			return 0;
		if(flags & SDL_RESIZABLE)
			return 0;
		if((flags & SDL_FULLSCREEN) && ((width != 320) || (height != 240)))
			return 0;
		if((width > 320) || (height > 240) || (width & 3))
			return 0;
		return bpp;
	}
	if(flags & SDL_PREALLOC)
		return 0;

	if(bpp != 16)
		return 0;
	if(flags & (SDL_SRCCOLORKEY | SDL_RLEACCEL | SDL_SRCALPHA))
		return 0;

	return bpp;
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags) {
	if(!_SDL_VideoInitialized)
		return NULL;
}

void SDL_UpdateRect(SDL_Surface* screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h) {
	if(screen == NULL)
		return;
	if((x == 0) && (y == 0) && (w == 0) && (h == 0)) {
		SDL_Flip(screen);
		return;
	}
	// TODO - Check rectangle dimensions.
	// TODO - Optimize this so that only the required area is updated.
	SDL_Flip(screen);
}

void SDL_UpdateRects(SDL_Surface* screen, int numrects, SDL_Rect* rects) {
	if(rects == NULL)
		return;
	/*uintptr_t i;
	for(i = 0; i < numrects; i++)
		SDL_UpdateRect(screen, rects[i].x, rects[i].y, rects[i].w, rects[i].h);*/
	SDL_Flip(screen);
}

int SDL_Flip(SDL_Surface* screen) {
	// TODO - Lots of work for non 16bpp surfaces.
	uint32_t i, j;
	if(screen->format->BitsPerPixel == 16) {
		if(screen->flags & SDL_HWSURFACE) {
			uint32_t* tempDispBuff32 = (uint32_t*)lcd_get_frame();
			uint32_t* tempDrawBuff32 = (uint32_t*)g_pGameDecodeBuf;
			uint32_t* tempDispEnd32;
			if(screen->flags & SDL_FULLSCREEN) {
				tempDispEnd32 = (uint32_t*)((uintptr_t)tempDispBuff32 + (320 * 240 * 2));
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
			} else {
				tempDispBuff32 = (uint32_t*)((uintptr_t)tempDispBuff32 + (320 * ((240 - screen->h) & 0xFFFFFFFE)) + ((320 - screen->w) & 0xFFFFFFFE));
				tempDispEnd32  = (uint32_t*)((uintptr_t)tempDispBuff32 + (screen->h * 640));
				while(tempDispBuff32 < tempDispEnd32) {
					for(i = 0; i < screen->w; i++)
						*(tempDispBuff32++) = *(tempDrawBuff32++);
					tempDispBuff32 = (uint32_t*)((uintptr_t)tempDispBuff32 + ((320 - screen->w) << 1));
					tempDrawBuff32 = (uint32_t*)((uintptr_t)tempDrawBuff32 + ((320 - screen->w) << 1));
				}
			}
		} else {
			uint16_t* tempDispBuff16 = (uint16_t*)lcd_get_frame();
			uint16_t* tempDrawBuff16 = (uint16_t*)screen->pixels;
			if(screen->flags & SDL_FULLSCREEN) {
				uint32_t i_fract, j_fract;
				uint32_t i_fract_inc = ((screen->w << 16) / 320);
				uint32_t j_fract_inc = ((screen->h << 16) / 240);
				for(j = 0, j_fract = 0; j < 240; j++, j_fract += j_fract_inc) {
					for(i = 0, i_fract = 0; i < 320; i++, i_fract += i_fract_inc)
						tempDispBuff16[i] = tempDrawBuff16[i_fract >> 16];
					tempDispBuff16 = (uint16_t*)((uintptr_t)lcd_get_frame() + (640 * (j_fract >> 16)));
					tempDrawBuff16 = (uint16_t*)((uintptr_t)tempDrawBuff16 + screen->pitch);
				}
			} else {
				// TODO - Draw at half size using linear downscaling.
			}
		}
		__dcache_writeback_all();
		lcd_set_frame();
	}
	return 0;
}



int SDL_SetColors(SDL_Surface* surface, SDL_Color* colors, int firstcolor, int ncolors) {}

int SDL_SetPalette(SDL_Surface* surface, int flags, SDL_Color* colors, int firstcolor, int ncolors) {}



int SDL_SetGamma(float redgamma, float greengamma, float bluegamma) {
	SDL_Unsupported();
	return -1;
}

int SDL_GetGammaRamp(Uint16* redtable, Uint16* greentable, Uint16* bluetable) {
	SDL_Unsupported();
	return -1;
}

int SDL_SetGammaRamp(Uint16* redtable, Uint16* greentable, Uint16* bluetable) {
	SDL_Unsupported();
	return -1;
}



Uint32 SDL_MapRGB(SDL_PixelFormat* fmt, Uint8 r, Uint8 g, Uint8 b) {}

Uint32 SDL_MapRGBA(SDL_PixelFormat* fmt, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {}

void SDL_GetRGB(Uint32 pixel, SDL_PixelFormat* fmt, Uint8* r, Uint8* g, Uint8* b) {}

void SDL_GetRGBA(Uint32 pixel, SDL_PixelFormat* fmt, Uint8* r, Uint8* g, Uint8* b, Uint8* a) {}



SDL_Surface* SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
	if((width <= 0) || (height <= 0))
		return NULL;
}

SDL_Surface* SDL_CreateRGBSurfaceFrom(void* pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {}

void SDL_FreeSurface(SDL_Surface* surface) {
	free(surface);
}

int SDL_LockSurface(SDL_Surface* surface) {
	// Does nothing since this is a single-threaded software implementation.
}

void SDL_UnlockSurface(SDL_Surface* surface) {
	// Does nothing since this is a single-threaded software implementation.
}

SDL_Surface* SDL_LoadBMP(const char* file) {}

int SDL_SaveBMP(SDL_Surface* surface, const char* file) {}

int SDL_SetColorKey(SDL_Surface* surface, Uint32 flag, Uint32 key) {}

int SDL_SetAlpha(SDL_Surface* surface, Uint32 flag, Uint8 alpha) {}

void SDL_SetClipRect(SDL_Surface* surface, SDL_Rect* rect) {
	if(surface == NULL)
		return;
	if(rect == NULL) {
		surface->clip_rect.x = 0;
		surface->clip_rect.y = 0;
		surface->clip_rect.w = surface->w;
		surface->clip_rect.h = surface->h;
		return;
	}
	surface->clip_rect = *rect;
	if(surface->clip_rect.x < 0) {
		surface->clip_rect.w += surface->clip_rect.x;
		surface->clip_rect.x = 0;
	}
	if(surface->clip_rect.y < 0) {
		surface->clip_rect.h += surface->clip_rect.y;
		surface->clip_rect.y = 0;
	}
	// TODO - More validation of clip area.
}

void SDL_GetClipRect(SDL_Surface* surface, SDL_Rect* rect) {
	if((surface == NULL) || (rect == NULL))
		return;
	*rect = surface->clip_rect;
}

SDL_Surface* SDL_ConvertSurface(SDL_Surface* src, SDL_PixelFormat* fmt, Uint32 flags) {}

int SDL_BlitSurface(SDL_Surface* src, SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect) {}

int SDL_FillRect(SDL_Surface* dst, SDL_Rect* dstrect, Uint32 color) {}

SDL_Surface* SDL_DisplayFormat(SDL_Surface* surface) {}

SDL_Surface* SDL_DisplayFormatAlpha(SDL_Surface *surface) {}




SDL_Overlay* SDL_CreateYUVOverlay(int width, int height, Uint32 format, SDL_Surface* display) {}

int SDL_LockYUVOverlay(SDL_Overlay* overlay) {}

void SDL_UnlockYUVOverlay(SDL_Overlay* overlay) {}

int SDL_DisplayYUVOverlay(SDL_Overlay* overlay, SDL_Rect* dstrect) {}

void SDL_FreeYUVOverlay(SDL_Overlay* overlay) {}
