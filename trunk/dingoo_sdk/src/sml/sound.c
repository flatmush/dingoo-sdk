#include <sml/sound.h>

#include <stdlib.h>
#include <string.h>

#include <dingoo/ucos2.h>

#include <sml/mtaudio.h>

typedef struct {
	int16_t*  sample_data;
	uintptr_t sample_count;
	uintptr_t sample_offset;
	uintptr_t sound_id;
	bool      stereo;
	bool      loop;
	bool      used;
} _channel_t;



volatile uintptr_t  _sound_sources_count = 16;
volatile _channel_t _sound_sources[16];
OS_EVENT*  _sound_sources_semaphore = NULL;

int16_t   _sound_buffer[3][4096];
uintptr_t _sound_buffer_used[3]  = { 0, 0, 0 };
uintptr_t _sound_buffer_size     = 4096;
uintptr_t _sound_buffer_volume   = 100;

uintptr_t _sound_buffer_mixing   = 0;
uintptr_t _sound_buffer_playnext = 3;
uintptr_t _sound_buffer_playing  = 1;

uint8_t       _sound_mixer_thread_id         = 255;
uintptr_t     _sound_mixer_thread_stack_size = 0;
OS_STK*       _sound_mixer_thread_stack      = NULL;
volatile bool _sound_mixer_thread_kill       = false;

bool _sound_ready  = false;
bool _sound_atexit = false;

uintptr_t _sound_id_max = 1;



#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef swap
#define swap(x, y) { typeof(y) __swap = (y); (y) = (x); (x) = __swap; }
#endif

inline int16_t _ssadd(int16_t inArg0, int16_t inArg1) {
	int32_t tempOut = (int32_t)inArg0 + (int32_t)inArg1;
	if(tempOut > +0x7FFF) tempOut = +0x7FFF;
	if(tempOut < -0x8000) tempOut = -0x8000;
	return (int16_t)tempOut;
}

inline void _sound_channel_mix(volatile _channel_t* inChannel) {
	if((inChannel == NULL) || !inChannel->used)
		return;

	uintptr_t i, j, j_max;
	for(i = 0; i < _sound_buffer_used[_sound_buffer_mixing];) {
		if(inChannel->stereo) {
			j_max = inChannel->sample_offset + min((_sound_buffer_used[_sound_buffer_mixing] - i), (inChannel->sample_count - inChannel->sample_offset));
			for(j = inChannel->sample_offset; j < j_max; j++, i++)
				_sound_buffer[_sound_buffer_mixing][i] = _ssadd(_sound_buffer[_sound_buffer_mixing][i], inChannel->sample_data[j]);
		} else {
			j_max = inChannel->sample_offset + min(((_sound_buffer_used[_sound_buffer_mixing] - i) >> 1), (inChannel->sample_count - inChannel->sample_offset));
			for(j = inChannel->sample_offset; j < j_max; j++, i += 2) {
				_sound_buffer[_sound_buffer_mixing][i + 0] = _ssadd(_sound_buffer[_sound_buffer_mixing][i + 0], inChannel->sample_data[j]);
				_sound_buffer[_sound_buffer_mixing][i + 1] = _ssadd(_sound_buffer[_sound_buffer_mixing][i + 1], inChannel->sample_data[j]);
			}
		}
		inChannel->sample_offset = j;
		if(inChannel->sample_offset >= inChannel->sample_count) {
			if(!inChannel->loop) {
				inChannel->used = false;
				return;
			}
			inChannel->sample_offset -= inChannel->sample_count;
		}
	}

	for(; i < _sound_buffer_size;) {
		if(inChannel->stereo) {
			j_max = inChannel->sample_offset + min((_sound_buffer_size - i), (inChannel->sample_count - inChannel->sample_offset));
			for(j = inChannel->sample_offset; j < j_max; j++, i++)
				_sound_buffer[_sound_buffer_mixing][i] = inChannel->sample_data[j];
		} else {
			j_max = inChannel->sample_offset + min(((_sound_buffer_size - i) >> 1), (inChannel->sample_count - inChannel->sample_offset));
			for(j = inChannel->sample_offset; j < j_max; j++, i += 2) {
				_sound_buffer[_sound_buffer_mixing][i + 0] = inChannel->sample_data[j];
				_sound_buffer[_sound_buffer_mixing][i + 1] = inChannel->sample_data[j];
			}
		}
		inChannel->sample_offset = j;
		if(inChannel->sample_offset >= inChannel->sample_count) {
			if(!inChannel->loop) {
				inChannel->used = false;
				_sound_buffer_used[_sound_buffer_mixing] = i;
				return;
			}
			inChannel->sample_offset -= inChannel->sample_count;
		}
	}

	_sound_buffer_used[_sound_buffer_mixing] = i;
}



void _sound_mixer_thread(void* inDummy) {
	uintptr_t i;
	uint8_t tempError;

	while(!_sound_mixer_thread_kill) {
		_sound_buffer_mixing = 0;
		while((_sound_buffer_mixing == _sound_buffer_playnext) || (_sound_buffer_mixing == _sound_buffer_playing))
			_sound_buffer_mixing++;

		OSSemPend(_sound_sources_semaphore, 0, &tempError);
		_sound_buffer_used[_sound_buffer_mixing] = 0;
		for(i = 0; i < _sound_sources_count; i++)
			_sound_channel_mix(&_sound_sources[i]);
		OSSemPost(_sound_sources_semaphore);

		while(_sound_buffer_playnext < 3) // TODO - Fix possible deadlock.
			OSTimeDly(1);
		_sound_buffer_playnext = _sound_buffer_mixing;
	}
	_sound_mixer_thread_kill = false;
	OSTaskDel(_sound_mixer_thread_id);
}

void _sound_callback() {
	_sound_buffer_playing = 3;

	// Wait until there is a buffer to be played.
	while(_sound_buffer_playnext >= 3) // TODO - Fix possible deadlock.
		OSTimeDly(1);

	_sound_buffer_playing  = _sound_buffer_playnext;
	_sound_buffer_playnext = 3;

	if(_sound_buffer_used[_sound_buffer_playing] == 0) {
		mtaudio_buffer_set(NULL, 0, 2, 100,48000);
		return;
	}

	mtaudio_buffer_set(_sound_buffer[_sound_buffer_playing], (_sound_buffer_used[_sound_buffer_playing] << 1), 2, _sound_buffer_volume, 48000);
}



bool sound_init() {
	if(_sound_ready)
		sound_term();

	uintptr_t i;
	for(i = 0; i < _sound_sources_count; i++)
		_sound_sources[i].used = false;

	_sound_sources_semaphore = OSSemCreate(1);
	if(_sound_sources_semaphore == NULL)
		return false;

	if(!mtaudio_init(_sound_callback,48000)) {
		OSSemDel(_sound_sources_semaphore, OS_DEL_NO_PEND, NULL);
		_sound_sources_semaphore = NULL;
		return false;
	}

	_sound_mixer_thread_stack_size = 1024;
	_sound_mixer_thread_stack = (OS_STK*)malloc(_sound_mixer_thread_stack_size * sizeof(OS_STK));
	if(_sound_mixer_thread_stack == NULL) {
		mtaudio_term();
		OSSemDel(_sound_sources_semaphore, OS_DEL_NO_PEND, NULL);
		_sound_sources_semaphore = NULL;
		return false;
	}

	uint8_t j;
	for(i = 6;; i++) {
		j = OSTaskCreate(_sound_mixer_thread, NULL, (void*)&_sound_mixer_thread_stack[_sound_mixer_thread_stack_size - 1], i);
		if(j == OS_NO_ERR)
			break;
		if(j >= OS_PRIO_INVALID) {
			free(_sound_mixer_thread_stack);
			_sound_mixer_thread_stack = NULL;
			mtaudio_term();
			OSSemDel(_sound_sources_semaphore, OS_DEL_NO_PEND, NULL);
			_sound_sources_semaphore = NULL;
			return false;
		}
	}
	_sound_mixer_thread_id = i;

	_sound_ready = true;
	if(!_sound_atexit && (atexit(sound_term) != 0))
		sound_term();
	_sound_atexit = true;
	return _sound_ready;
}

void sound_term() {
	if(!_sound_ready)
		return;
	_sound_ready = false;

	_sound_mixer_thread_kill = true;
	while(_sound_mixer_thread_kill)
		OSTimeDly(1);

	free(_sound_mixer_thread_stack);
	_sound_mixer_thread_stack = NULL;

	mtaudio_term();
	uint8_t tempError;
	OSSemDel(_sound_sources_semaphore, OS_DEL_NO_PEND, &tempError);
	_sound_sources_semaphore = NULL;
}

uintptr_t sound_play(sound_t inSound, bool inLoop) {
	if(inSound.sample_count == 0)
		return 0;
	if(inSound.sample_data == NULL)
		return 0;
	if((inSound.sample_bits != 16) || (inSound.sample_rate != 48000))
		return 0;
	if((inSound.channels == 0) || (inSound.channels > 2))
		return 0;

	uint8_t tempError;
	OSSemPend(_sound_sources_semaphore, 0, &tempError);

	uintptr_t i;
	for(i = 0; i < _sound_sources_count; i++) {
		if(!_sound_sources[i].used) {
			_sound_sources[i].sample_data   = inSound.sample_data;
			_sound_sources[i].sample_count  = inSound.sample_count;
			_sound_sources[i].sample_offset = 0;
			_sound_sources[i].sound_id      = _sound_id_max++;
			_sound_sources[i].stereo        = (inSound.channels == 2);
			_sound_sources[i].loop          = inLoop;
			_sound_sources[i].used          = true;
			OSSemPost(_sound_sources_semaphore);
			return _sound_sources[i].sound_id;
		}
	}

	OSSemPost(_sound_sources_semaphore);
	return 0;
}

void sound_stop(uintptr_t inSound) {
	if(inSound >= _sound_id_max)
		return;

	uint8_t tempError;
	OSSemPend(_sound_sources_semaphore, 0, &tempError);

	uintptr_t i;
	for(i = 0; i < _sound_sources_count; i++) {
		if((inSound == 0) || (_sound_sources[i].sound_id == inSound))
			_sound_sources[i].used = false;
	}

	OSSemPost(_sound_sources_semaphore);
}



uintptr_t sound_volume_set(uintptr_t inVolume) {
	uintptr_t tempVol = min(inVolume, 100);
	_sound_buffer_volume = tempVol;
	return tempVol;
}
