#ifndef __SDL_ERROR_h__
#define __SDL_ERROR_h__

#include "SDL_TYPES.h"

typedef enum {
	SDL_ENOMEM,
	SDL_EFREAD,
	SDL_EFWRITE,
	SDL_EFSEEK,
	SDL_UNSUPPORTED,
	SDL_LASTERROR = SDL_UNSUPPORTED
} SDL_errorcode;

extern char* SDL_GetError();
extern char* SDL_GetErrorMsg(char* errstr, unsigned int maxlen);
extern void  SDL_SetError (const char *fmt, ...);
extern void  SDL_Error(SDL_errorcode code);
extern void  SDL_ClearError();

extern void  SDL_OutOfMemory();
extern void  SDL_Unsupported();

#endif
