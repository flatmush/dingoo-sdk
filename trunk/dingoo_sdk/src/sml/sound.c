#include <sml/sound.h>

#include <stdlib.h>
#include <string.h>

#include <dingoo/ucos2.h>

#include <sml/mtaudio.h>


volatile uintptr_t  _sound_sources_count = 16;
volatile sound_t    _sound_sources[16];
volatile bool       _sound_sources_lock = false;

uintptr_t _sound_buffer_size = 4096;
int16_t   _sound_buffer[4096];



#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

inline int16_t _ssadd(int16_t inArg0, int16_t inArg1) {
	int32_t outResult = inArg0 + inArg1;
	if(outResult > +0x7FFF) outResult = +0x7FFF;
	if(outResult < -0x8000) outResult = -0x8000;
	return (int16_t)outResult;
}



void _sound_callback() {
	while(_sound_sources_lock)
		OSTimeDly(1);
	_sound_sources_lock = true;

	uintptr_t i, j;
	uintptr_t tempCountCur;
	uintptr_t tempCountMax = 0;
	for(i = 0; i < _sound_sources_count; i++) {
		if(_sound_sources[i].sample_count != 0) {
			tempCountCur = min((_sound_sources[i].sample_count << 1), _sound_buffer_size);
			if(_sound_sources[i].channels == 2) {
				for(j = 0; j < min(tempCountCur, tempCountMax); j++) {
					_sound_buffer[j] = _ssadd(_sound_buffer[j], _sound_sources[i].data[0]);
					_sound_sources[i].data++;
				}
				if(tempCountMax < tempCountCur) {
					for(j = tempCountMax; j < tempCountCur; j++) {
						_sound_buffer[j] = _sound_sources[i].data[0];
						_sound_sources[i].data++;
					}
					tempCountMax = tempCountCur;
				}
			} else {
				for(j = 0; j < min(tempCountCur, tempCountMax); j += 2) {
					_sound_buffer[j + 0] = _ssadd(_sound_buffer[j + 0], _sound_sources[i].data[0]);
					_sound_buffer[j + 1] = _ssadd(_sound_buffer[j + 1], _sound_sources[i].data[0]);
					_sound_sources[i].data++;
				}
				if(tempCountMax < tempCountCur) {
					for(j = tempCountMax; j < tempCountCur; j += 2) {
						_sound_buffer[j + 0] = _sound_sources[i].data[0];
						_sound_buffer[j + 1] = _sound_sources[i].data[0];
						_sound_sources[i].data++;
					}
					tempCountMax = tempCountCur;
				}
			}
			_sound_sources[i].sample_count -= (tempCountCur >> 1);
		}
	}
	_sound_sources_lock = false;

	if(tempCountMax == 0) {
		mtaudio_buffer_set(NULL, 0, 2, 100);
		return;
	}

	mtaudio_buffer_set(_sound_buffer, (tempCountMax << 1), 2, 100);
}



bool sound_init() {
	uintptr_t i;
	for(i = 0; i < _sound_sources_count; i++)
		_sound_sources[i].sample_count = 0;
	return mtaudio_init(_sound_callback);
}

void sound_term() {
	mtaudio_term();
}

bool sound_play(sound_t inSound) {
	if(inSound.sample_count == 0)
		return true;
	if(inSound.data == NULL)
		return false;
	if((inSound.sample_bits != 16) || (inSound.sample_rate != 48000))
		return false;
	if((inSound.channels == 0) || (inSound.channels > 2))
		return false;

	while(_sound_sources_lock)
		OSTimeDly(1);
	_sound_sources_lock = true;

	uintptr_t i;
	for(i = 0; i < _sound_sources_count; i++) {
		if(_sound_sources[i].sample_count == 0) {
			_sound_sources[i] = inSound;
			_sound_sources_lock = false;
			return true;
		}
	}

	_sound_sources_lock = false;
	return false;
}
