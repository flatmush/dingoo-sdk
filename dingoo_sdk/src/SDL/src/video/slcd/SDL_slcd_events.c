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
#include "SDL_config.h"

#include "SDL.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"

#include "SDL_slcd_video.h"
#include "SDL_slcd_events_c.h"

#include <sml/control.h>
#include <sml/timer.h>

static SDLKey keymap[32];
static SDL_keysym *TranslateKey(int scancode, SDL_keysym *keysym);
static int posted = 0;

static Uint8 dingooIgnoreOSEvents = 0;

void SLCD_PumpEvents(_THIS)
{
	SDL_keysym keysym;
	int pressed, scancode;

	// Poll keys
	control_poll();

	for (scancode = 0; scancode < 32; scancode++)
	{
		if (control_check(scancode).changed)
		{
			if (control_check(scancode).pressed)
				pressed = SDL_PRESSED;
			else
				pressed = SDL_RELEASED;

			posted += SDL_PrivateKeyboard(pressed, TranslateKey(scancode, &keysym));
		}
	}

	if (!dingooIgnoreOSEvents)
	{
		// Quit on system events
		if (_sys_judge_event(NULL) < 0)
		{
			posted = SDL_PrivateQuit();
		}
	}
}

void SLCD_InitOSKeymap(_THIS)
{
	char* env;
	env = SDL_getenv("DINGOO_IGNORE_OS_EVENTS");
	if (env)
		dingooIgnoreOSEvents = 1;

	control_init();
	control_lock(timer_resolution / 4);

	int i;
	for (i = 0; i < 32; i++)
		keymap[i] = SDLK_UNKNOWN;

	keymap[CONTROL_POWER] = SDLK_PAUSE;
	keymap[CONTROL_BUTTON_A] = SDLK_LCTRL;
	keymap[CONTROL_BUTTON_B] = SDLK_LALT;
	keymap[CONTROL_BUTTON_X] = SDLK_SPACE;
	keymap[CONTROL_BUTTON_Y] = SDLK_LSHIFT;
	keymap[CONTROL_BUTTON_START] = SDLK_RETURN;
	keymap[CONTROL_BUTTON_SELECT] = SDLK_ESCAPE;
	keymap[CONTROL_TRIGGER_LEFT] = SDLK_TAB;
	keymap[CONTROL_TRIGGER_RIGHT] = SDLK_BACKSPACE;
	keymap[CONTROL_DPAD_UP] = SDLK_UP;
	keymap[CONTROL_DPAD_DOWN] = SDLK_DOWN;
	keymap[CONTROL_DPAD_LEFT] = SDLK_LEFT;
	keymap[CONTROL_DPAD_RIGHT] = SDLK_RIGHT;
}

static SDL_keysym *TranslateKey(int scancode, SDL_keysym *keysym)
{
	/* Set the keysym information */
	keysym->scancode = scancode;
	keysym->sym = keymap[scancode];
	keysym->mod = KMOD_NONE;

	/* If UNICODE is on, get the UNICODE value for the key */
	keysym->unicode = 0;
	if ( SDL_TranslateUNICODE ) {
		/* Populate the unicode field with the ASCII value */
		keysym->unicode = scancode;
	}
	return(keysym);
}

/* end of SDL_slcd_events.c ... */

