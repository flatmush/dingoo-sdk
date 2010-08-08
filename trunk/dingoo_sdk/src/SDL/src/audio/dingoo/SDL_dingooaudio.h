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

#ifndef _SDL_dingooaudio_h
#define _SDL_dingooaudio_h

#include "../SDL_sysaudio.h"
#include <dingoo/ucos2.h>
#include <dingoo/audio.h>

waveout_inst* woHandle;

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_AudioDevice *this

struct SDL_PrivateAudioData {
	//waveout_inst* sound;
	////HANDLE audio_sem;
	//Uint8 *mixbuf;		/* The raw allocated mixing buffer */
	//void* wavebuf[NUM_BUFFERS];	/* Wave audio fragments */
	//int next_buffer;
};

/* Old variable names */
//#define sound			(this->hidden->sound)
//#define audio_sem 		(this->hidden->audio_sem)
//#define mixbuf			(this->hidden->mixbuf)
//#define wavebuf			(this->hidden->wavebuf)
//#define next_buffer		(this->hidden->next_buffer)

#endif /* _SDL_lowaudio_h */
