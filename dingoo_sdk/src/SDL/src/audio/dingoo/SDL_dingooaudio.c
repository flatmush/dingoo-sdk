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

/* Allow access to a raw mixing buffer */

#include "SDL_timer.h"
#include "SDL_audio.h"
#include "../SDL_audio_c.h"
#include "SDL_dingooaudio.h"
#include "dingoo_sound.h"


/* Audio driver functions */
static int DINGOO_OpenAudio(_THIS, SDL_AudioSpec *spec);
static void DINGOO_WaitAudio(_THIS);
static void DINGOO_PlayAudio(_THIS);
static Uint8 *DINGOO_GetAudioBuf(_THIS);
static void DINGOO_CloseAudio(_THIS);
static void DINGOO_LockAudio(_THIS);
static void DINGOO_UnlockAudio(_THIS);

static SDL_AudioDevice *sdl_dingoo_audiodevice; 

/* Audio driver bootstrap functions */

static int Audio_Available(void)
{
	return(1);
}

static void Audio_DeleteDevice(SDL_AudioDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_AudioDevice *Audio_CreateDevice(int devindex)
{
	SDL_AudioDevice *this;

	/* Initialize all variables that we clean on shutdown */
	this = (SDL_AudioDevice *)SDL_malloc(sizeof(SDL_AudioDevice));
	if ( this ) {
		SDL_memset(this, 0, (sizeof *this));
		this->hidden = (struct SDL_PrivateAudioData *)
				SDL_malloc((sizeof *this->hidden));
	}
	if ( (this == NULL) || (this->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( this ) {
			SDL_free(this);
		}
		return(0);
	}
	SDL_memset(this->hidden, 0, (sizeof *this->hidden));

	/* Set the function pointers */
	this->OpenAudio = DINGOO_OpenAudio;
	this->WaitAudio = DINGOO_WaitAudio;
	this->PlayAudio = DINGOO_PlayAudio;
	this->GetAudioBuf = DINGOO_GetAudioBuf;	
	this->CloseAudio = DINGOO_CloseAudio;
	this->LockAudio   = DINGOO_LockAudio;
    this->UnlockAudio = DINGOO_UnlockAudio;

	this->free = Audio_DeleteDevice;

	return this;
}

AudioBootStrap DINGOO_bootstrap = {
	"dingoo", "Dingoo WaveOut",
	Audio_Available, Audio_CreateDevice
};

static void DINGOO_LockAudio(_THIS)
{
	Uint8 tempError;
    OSSemPend(audio_sem, 1000, &tempError);
}

static void DINGOO_UnlockAudio(_THIS)
{
    OSSemPost(audio_sem);
}

/* The Dingoo callback for handling the audio buffer */
static void FillSound(void *stream, uint32_t len)
{
	SDL_AudioDevice *audio = (SDL_AudioDevice *)sdl_dingoo_audiodevice;

	/* Silence the buffer, since it's ours */
	SDL_memset(stream, audio->spec.silence, len);

	/* Only do something if audio is enabled */
	if ( ! audio->enabled )
		return;

	if ( ! audio->paused ) {
		if ( audio->convert.needed ) {
			SDL_LockAudio();
			(*audio->spec.callback)(audio->spec.userdata,
				(Uint8 *)audio->convert.buf,audio->convert.len);
			SDL_UnlockAudio();
			SDL_ConvertAudio(&audio->convert);
			SDL_memcpy(stream,audio->convert.buf,audio->convert.len_cvt);
		} else {
			SDL_LockAudio();
			(*audio->spec.callback)(audio->spec.userdata,
						(Uint8 *)stream, len);
			SDL_UnlockAudio();
		}
	}
	return;
}

/* Dummy functions -- we don't use thread-based audio */
void DINGOO_WaitAudio(_THIS)
{
	return;
}
void DINGOO_PlayAudio(_THIS)
{
	return;
}
Uint8 *DINGOO_GetAudioBuf(_THIS)
{
	return(NULL);
}

void DINGOO_CloseAudio(_THIS)
{
	dingooSoundClose();
	Uint8 tempError;
	audio_sem = OSSemDel(audio_sem, OS_DEL_ALWAYS, &tempError);
}

int DINGOO_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	int valid_datatype = 0;
	waveout_args waveformat;
	memset(&waveformat, 0, sizeof(waveformat));
	Uint16 test_format = SDL_FirstAudioFormat(spec->format);

	while ((!valid_datatype) && (test_format)) {
        valid_datatype = 1;
        spec->format = test_format;
        switch (test_format) {

            /*case AUDIO_U8:
                format.format = media_raw_audio_format::B_AUDIO_UCHAR;
                break;*/

            case AUDIO_S16LSB:
                waveformat.format = 16;
                break;

            default:
                valid_datatype = 0;
                test_format = SDL_NextAudioFormat();
                break;
        }
    }

	if (!valid_datatype) { /* shouldn't happen, but just in case... */
        SDL_SetError("Unsupported audio format");
        return (-1);
    }

	if (spec->channels > 2)
        spec->channels = 2;  /* no more than stereo on the Dingoo. */

	waveformat.sample_rate = spec->freq;
	waveformat.channel = spec->channels;
	waveformat.volume = 30;

	/* Update the fragment size as size in bytes */
	SDL_CalculateAudioSpec(spec);

	audio_sem = OSSemCreate(1);
	if (audio_sem == NULL)
	{
		SDL_SetError("Unable to start Dingoo Sound (audio_sem == NULL)");
		return(-1);
	}

	if (dingooSoundInit(waveformat, spec->samples, FillSound) < 0)
	{
		SDL_SetError("Unable to start Dingoo Sound (init failed)");
		Uint8 tempError;
		OSSemDel(audio_sem, OS_DEL_ALWAYS, &tempError);
		return(-1);
	}
	dingooSoundPause(false);

	sdl_dingoo_audiodevice = this;

	/* Ready to go! */
	return(1);
}
