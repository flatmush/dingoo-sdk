#include "SDL.h"
#include <ucos2.h>




Uint32 SDL_GetTicks() {
	uint64_t tempTicks = OSTimeGet();
	tempTicks *= 1000;
	tempTicks /= OS_TICKS_PER_SEC;
	return (Uint32)tempTicks;
}

void SDL_Delay(Uint32 ms) {
	if(ms > (1000 / OS_TICKS_PER_SEC))
		OSTimeDly((ms * OS_TICKS_PER_SEC) / 1000);
	else
		msdelay(ms);
}

SDL_TimerID SDL_AddTimer(Uint32 interval, SDL_NewTimerCallback callback, void* param) {
	// TODO - Multi-threaded timer callbacks (Ugh!)
}

SDL_bool SDL_RemoveTimer(SDL_TimerID id) {
	// TODO - Multi-threaded timer callbacks (Ugh!)
}

int SDL_SetTimer(Uint32 interval, SDL_TimerCallback callback) {
	// TODO - Multi-threaded timer callbacks (Ugh!)
}
