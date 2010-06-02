#include "SDL.h"
#include <stdlib.h>

SDL_version _SDL_Version = { SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL };

extern int  _SDL_VideoInit();
extern void _SDL_VideoTerm();

int SDL_Init(Uint32 flags) {
	int tempOut = SDL_InitSubSystem(flags);
	return tempOut;
}

int SDL_InitSubSystem(Uint32 flags) {
	int tempOut = 0;
	if(flags & SDL_INIT_VIDEO) {
		tempOut = _SDL_VideoInit();
		if(tempOut < 0) return tempOut;
	}
	return tempOut;
}

void SDL_QuitSubSystem(Uint32 flags) {
	if(flags & SDL_INIT_VIDEO)
		_SDL_VideoTerm();
}

void SDL_Quit() {
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
}

Uint32 SDL_WasInit(Uint32 flags) {
	return 0;
}


void* SDL_LoadObject(const char* sofile) {
	SDL_Unsupported();
	return NULL;
}

void* SDL_LoadFunction(void* handle, const char* name) {
	SDL_Unsupported();
	return NULL;
}

void SDL_UnloadObject(void *handle) {
	SDL_Unsupported();
}



const SDL_version* SDL_Linked_Version() {
	return &_SDL_Version;
}
