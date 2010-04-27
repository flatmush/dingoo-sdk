#ifndef __SDL_WM_h__
#define __SDL_WM_h__

#include "SDL_TYPES.h"

typedef enum {
	SDL_GRAB_QUERY,
	SDL_GRAB_OFF,
	SDL_GRAB_ON
} SDL_GrabMode;

typedef struct {
  SDL_version version;
} SDL_SysWMinfo;

extern int          SDL_GetWMInfo(SDL_SysWMinfo* info);
extern void         SDL_WM_SetCaption(const char* title, const char* icon);
extern void         SDL_WM_GetCaption(char** title, char** icon);
extern void         SDL_WM_SetIcon(SDL_Surface* icon, Uint8* mask);
extern int          SDL_WM_IconifyWindow();
extern int          SDL_WM_ToggleFullScreen(SDL_Surface* surface);
extern SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode);

#endif
