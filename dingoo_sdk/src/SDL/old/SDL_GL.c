#include "SDL.h"

int SDL_GL_LoadLibrary(const char* path) {
	SDL_Unsupported();
	return -1;
}

void* SDL_GL_GetProcAddress(const char* proc) {
	SDL_Unsupported();
	return NULL;
}

int SDL_GL_GetAttribute(SDL_GLattr attr, int* value) {
	SDL_Unsupported();
	return -1;
}

int SDL_GL_SetAttribute(SDL_GLattr attr, int value) {
	SDL_Unsupported();
	return -1;
}

void SDL_GL_SwapBuffers() {
	SDL_Unsupported();
}
