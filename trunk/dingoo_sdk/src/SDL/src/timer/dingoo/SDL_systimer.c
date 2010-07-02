/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

// We use OSTimeGet and OSTimeDly which works in 1/100 of a second
// so the accuracy isn't perfect...

#include "SDL_config.h"

#ifdef SDL_TIMER_DINGOO

#include <dingoo/ucos2.h>
#include <dingoo/time.h>

#include "SDL_thread.h"
#include "SDL_timer.h"
#include "../SDL_timer_c.h"

static uint32_t start;

void SDL_StartTicks(void)
{
	/* Set first ticks value */
	start = OSTimeGet();
}

Uint32 SDL_GetTicks(void)
{
	uint32_t now = OSTimeGet();
	return((now - start) * 10);
}

void SDL_Delay(Uint32 ms)
{
	Uint32 h = ms / 10;
	OSTimeDly(h);
	delay_ms(ms - h);
}

/* This is only called if the event thread is not running */
int SDL_SYS_TimerInit(void)
{
	return 0;
}

void SDL_SYS_TimerQuit(void)
{
}

int SDL_SYS_StartTimer(void)
{
	return 0;
}

void SDL_SYS_StopTimer(void)
{
}

#endif
