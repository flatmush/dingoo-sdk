#include "SDL.h"
#include <stdio.h>
#include <stdarg.h>

char _SDL_Error[256] = { 0 };

char* SDL_GetError() {
	if(_SDL_Error[0] == '\0')
		return NULL;
	return _SDL_Error;
}

char* SDL_GetErrorMsg(char* errstr, unsigned int maxlen) {
	if(maxlen > 256)
		maxlen = 256;
	strncpy(errstr, _SDL_Error, maxlen);
	return errstr;
}

void SDL_SetError (const char *fmt, ...) {
	va_list tempArgs;
	va_start(tempArgs, fmt);
	vsprintf(_SDL_Error, fmt, tempArgs);
	va_end(tempArgs);
}

void SDL_Error(SDL_errorcode code) {
	switch(code) {
		case SDL_ENOMEM:
			sprintf(_SDL_Error, "Out of memory");
			break;
		case SDL_EFREAD:
			sprintf(_SDL_Error, "Error reading from datastream");
			break;
		case SDL_EFWRITE:
			sprintf(_SDL_Error, "Error writing to datastream");
			break;
		case SDL_EFSEEK:
			sprintf(_SDL_Error, "Error seeking in datastream");
			break;
		case SDL_UNSUPPORTED:
			sprintf(_SDL_Error, "Unsupported SDL feature");
			break;
		default:
			sprintf(_SDL_Error, "Unknown SDL error");
			break;
	}
}

void SDL_ClearError() {
	_SDL_Error[0] = '\0';
}


void SDL_OutOfMemory() {
	SDL_Error(SDL_ENOMEM);
}

void SDL_Unsupported() {
	SDL_Error(SDL_UNSUPPORTED);
}
