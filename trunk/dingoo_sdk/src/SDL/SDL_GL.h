#ifndef __SDL_GL_h__
#define __SDL_GL_h__

typedef enum {
	SDL_GL_RED_SIZE,
	SDL_GL_GREEN_SIZE,
	SDL_GL_BLUE_SIZE,
	SDL_GL_ALPHA_SIZE,
	SDL_GL_DOUBLEBUFFER,
	SDL_GL_BUFFER_SIZE,
	SDL_GL_DEPTH_SIZE,
	SDL_GL_STENCIL_SIZE,
	SDL_GL_ACCUM_RED_SIZE,
	SDL_GL_ACCUM_GREEN_SIZE,
	SDL_GL_ACCUM_BLUE_SIZE,
	SDL_GL_ACCUM_ALPHA_SIZE
} SDL_GLattr;

extern int   SDL_GL_LoadLibrary(const char* path);
extern void* SDL_GL_GetProcAddress(const char* proc);
extern int   SDL_GL_GetAttribute(SDL_GLattr attr, int* value);
extern int   SDL_GL_SetAttribute(SDL_GLattr attr, int value);
extern void  SDL_GL_SwapBuffers();

#endif
