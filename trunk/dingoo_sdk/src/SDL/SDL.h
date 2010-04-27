#ifndef __SDL_h__
#define __SDL_h__

#include "SDL_TYPES.h"

#include "SDL_ERROR.h"
#include "SDL_VIDEO.h"
#include "SDL_GL.h"
#include "SDL_WM.h"
#include "SDL_EVENTS.h"
#include "SDL_MOUSE.h"
#include "SDL_JOYSTICK.h"
#include "SDL_AUDIO.h"
#include "SDL_CDROM.h"
#include "SDL_THREADS.h"
#include "SDL_TIMER.h"

#define SDL_INIT_EVERYTHING  (SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_CDROM | SDL_INIT_JOYSTICK)
#define SDL_INIT_NOPARACHUTE (1 << 16)
#define SDL_INIT_EVENTTHREAD (1 << 17)

#define SDL_MAJOR_VERSION 1
#define SDL_MINOR_VERSION 2
#define SDL_PATCHLEVEL    13
#define SDL_VERSION(pointer) { if((pointer) != NULL) { (pointer)->major = SDL_MAJOR_VERSION; (pointer)->minor = SDL_MINOR_VERSION; (pointer)->patch = SDL_PATCHLEVEL; } }

extern int    SDL_Init(Uint32 flags);
extern int    SDL_InitSubSystem(Uint32 flags);
extern void   SDL_QuitSubSystem(Uint32 flags);
extern void   SDL_Quit();
extern Uint32 SDL_WasInit(Uint32 flags);

extern void*  SDL_LoadObject(const char* sofile);
extern void*  SDL_LoadFunction(void* handle, const char* name);
extern void   SDL_UnloadObject(void *handle);

extern const  SDL_version* SDL_Linked_Version();

#endif
