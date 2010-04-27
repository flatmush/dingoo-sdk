#ifndef __SDL_TIMER_h__
#define __SDL_TIMER_h__

#include "SDL_TYPES.h"

#define SDL_INIT_TIMER (1 << 0)

typedef uint32_t SDL_TimerID;

typedef Uint32 (*SDL_TimerCallback)(Uint32 interval);
typedef Uint32 (*SDL_NewTimerCallback)(Uint32 interval, void* param);

extern Uint32      SDL_GetTicks();
extern void        SDL_Delay(Uint32 ms);
extern SDL_TimerID SDL_AddTimer(Uint32 interval, SDL_NewTimerCallback callback, void* param);
extern SDL_bool    SDL_RemoveTimer(SDL_TimerID id);
extern int         SDL_SetTimer(Uint32 interval, SDL_TimerCallback callback);

#endif
