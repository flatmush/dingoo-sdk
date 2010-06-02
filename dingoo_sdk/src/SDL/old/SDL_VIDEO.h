#ifndef __SDL_VIDEO_h__
#define __SDL_VIDEO_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include "SDL_TYPES.h"

#define SDL_INIT_VIDEO (1 << 2)

extern SDL_Surface*   SDL_GetVideoSurface();
extern SDL_VideoInfo* SDL_GetVideoInfo();
extern char*          SDL_VideoDriverName(char* namebuf, int maxlen);
extern SDL_Rect**     SDL_ListModes(SDL_PixelFormat* format, Uint32 flags);
extern int            SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags);
extern SDL_Surface*   SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);
extern void           SDL_UpdateRect(SDL_Surface* screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
extern void           SDL_UpdateRects(SDL_Surface* screen, int numrects, SDL_Rect* rects);
extern int            SDL_Flip(SDL_Surface* screen);

extern int            SDL_SetColors(SDL_Surface* surface, SDL_Color* colors, int firstcolor, int ncolors);
extern int            SDL_SetPalette(SDL_Surface* surface, int flags, SDL_Color* colors, int firstcolor, int ncolors);

extern int            SDL_SetGamma(float redgamma, float greengamma, float bluegamma);
extern int            SDL_GetGammaRamp(Uint16* redtable, Uint16* greentable, Uint16* bluetable);
extern int            SDL_SetGammaRamp(Uint16* redtable, Uint16* greentable, Uint16* bluetable);

extern Uint32 SDL_MapRGB(SDL_PixelFormat* fmt, Uint8 r, Uint8 g, Uint8 b);
extern Uint32 SDL_MapRGBA(SDL_PixelFormat* fmt, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
extern void   SDL_GetRGB(Uint32 pixel, SDL_PixelFormat* fmt, Uint8* r, Uint8* g, Uint8* b);
extern void   SDL_GetRGBA(Uint32 pixel, SDL_PixelFormat* fmt, Uint8* r, Uint8* g, Uint8* b, Uint8* a);

extern SDL_Surface* SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern SDL_Surface* SDL_CreateRGBSurfaceFrom(void* pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern void         SDL_FreeSurface(SDL_Surface* surface);
extern int          SDL_LockSurface(SDL_Surface* surface);
extern void         SDL_UnlockSurface(SDL_Surface* surface);
extern SDL_Surface* SDL_LoadBMP(const char* file);
extern int          SDL_SaveBMP(SDL_Surface* surface, const char* file);
extern int          SDL_SetColorKey(SDL_Surface* surface, Uint32 flag, Uint32 key);
extern int          SDL_SetAlpha(SDL_Surface* surface, Uint32 flag, Uint8 alpha);
extern void         SDL_SetClipRect(SDL_Surface* surface, SDL_Rect* rect);
extern void         SDL_GetClipRect(SDL_Surface* surface, SDL_Rect* rect);
extern SDL_Surface* SDL_ConvertSurface(SDL_Surface* src, SDL_PixelFormat* fmt, Uint32 flags);
extern int          SDL_BlitSurface(SDL_Surface* src, SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect);
extern int          SDL_FillRect(SDL_Surface* dst, SDL_Rect* dstrect, Uint32 color);
extern SDL_Surface* SDL_DisplayFormat(SDL_Surface* surface);
extern SDL_Surface* SDL_DisplayFormatAlpha(SDL_Surface *surface);

extern SDL_Overlay* SDL_CreateYUVOverlay(int width, int height, Uint32 format, SDL_Surface* display);
extern int          SDL_LockYUVOverlay(SDL_Overlay* overlay);
extern void         SDL_UnlockYUVOverlay(SDL_Overlay* overlay);
extern int          SDL_DisplayYUVOverlay(SDL_Overlay* overlay, SDL_Rect* dstrect);
extern void         SDL_FreeYUVOverlay(SDL_Overlay* overlay);

#ifdef __cplusplus
}
#endif

#endif
