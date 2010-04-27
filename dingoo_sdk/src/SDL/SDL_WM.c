#include "SDL.h"

int SDL_GetWMInfo(SDL_SysWMinfo* info) {
	if(info == NULL)
		return -1;
	SDL_VERSION(info->version);
	return 1;
}

void SDL_WM_SetCaption(const char* title, const char* icon) {
	SDL_Unsupported();
}

void SDL_WM_GetCaption(char** title, char** icon) {

}

void SDL_WM_SetIcon(SDL_Surface* icon, Uint8* mask) {
	SDL_Unsupported();
}

int SDL_WM_IconifyWindow() {
	SDL_Unsupported();
	return 0;
}

int SDL_WM_ToggleFullScreen(SDL_Surface* surface) {
	if(surface == NULL)
		return 0;
	if(((surface->flags & SDL_FULLSCREEN) == 0) && (surface->flags & SDL_HWSURFACE) && ((width != 320) || (height != 240)))
		return 0;
	surface->flags ^= SDL_FULLSCREEN;
	return 1;
}

SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode) {
	return SDL_GRAB_ON;
}
