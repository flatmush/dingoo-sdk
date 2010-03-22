#include <sml/mtaudio.h>

#include <stdlib.h>

#include <dingoo/ucos2.h>
#include <dingoo/audio.h>



bool          _audio_ready             = false;
waveout_inst* _audio_inst              = NULL;
uint8_t       _audio_thread_id         = 255;
uintptr_t     _audio_thread_stack_size = 0;
OS_STK*       _audio_thread_stack      = NULL;
volatile bool _audio_thread_kill       = false;

void*    _audio_buffer          = NULL;
uint32_t _audio_buffer_size     = 0;

uint8_t  _audio_buffer_channels = 2;
uint8_t  _audio_buffer_volume   = 100;

uint32_t _audio_buffer_rate     = 48000;
uint8_t  _audio_buffer_bits     = AFMT_S16_LE;

void (*_audio_callback)() = NULL;



void _audio_thread(void* inDummy) {
	while(!_audio_thread_kill) {
		if(!waveout_can_write()) {
			OSTimeDly(1);
			continue;
		}
		_audio_callback();
		if(_audio_buffer == NULL) {
			OSTimeDly(1);
			continue;
		}
		waveout_write(_audio_inst, _audio_buffer, _audio_buffer_size);
	}
	_audio_thread_kill = false;
	OSTaskDel(_audio_thread_id);
}



bool mtaudio_init(void* inCallback) {
	if(_audio_ready)
		mtaudio_term();

	_audio_thread_stack_size = 1024;
	_audio_thread_stack = (OS_STK*)malloc(_audio_thread_stack_size * sizeof(OS_STK));
	if(_audio_thread_stack == NULL)
		return false;

	waveout_args tempArgs = { _audio_buffer_rate, _audio_buffer_bits, _audio_buffer_channels, _audio_buffer_volume };
	_audio_inst = waveout_open(&tempArgs);
	if(_audio_inst == NULL) {
		free(_audio_thread_stack);
		_audio_thread_stack = NULL;
		return false;
	}

	_audio_callback = inCallback;
	uint8_t i, j;
	for(i = 5;; i++) {
		j = OSTaskCreate(_audio_thread, NULL, (void*)&_audio_thread_stack[_audio_thread_stack_size - 1], i);
		if(j == OS_NO_ERR)
			break;
		if(j >= OS_PRIO_INVALID) {
			waveout_close(_audio_inst);
			_audio_inst = NULL;
			free(_audio_thread_stack);
			_audio_thread_stack = NULL;
			return false;
		}
	}
	_audio_thread_id = i;

	_audio_ready = true;
	return _audio_ready;
}

void mtaudio_term() {
	if(!_audio_ready)
		return;

	_audio_thread_kill = true;
	while(_audio_thread_kill)
		OSTimeDly(1);

	free(_audio_thread_stack);
	_audio_thread_stack = NULL;

	waveout_close(_audio_inst);
	_audio_inst = NULL;

	_audio_callback = NULL;
	_audio_ready = false;
}



void mtaudio_buffer_set(void* inBuffer, uintptr_t inSize, uint8_t inChannels, uint8_t inVolume) {
	_audio_buffer      = NULL;
	_audio_buffer_size = 0;

	if((inChannels == 0) || (inChannels > 2))
		return;

	if(inVolume > 100)
		inVolume = 100;

	if(inChannels != _audio_buffer_channels) {
		waveout_close(_audio_inst);
		waveout_args tempArgs = { _audio_buffer_rate, _audio_buffer_bits, inChannels, inVolume };
		_audio_inst = waveout_open(&tempArgs);
		if(_audio_inst == NULL) {
			tempArgs.channel = _audio_buffer_channels;
			tempArgs.volume  = _audio_buffer_volume;
			_audio_inst = waveout_open(&tempArgs);
			return;
		}
		_audio_buffer_channels = inChannels;
		_audio_buffer_volume   = inVolume;
	} else if(inVolume != _audio_buffer_volume) {
		waveout_set_volume(inVolume);
		_audio_buffer_volume = inVolume;
	}

	_audio_buffer      = inBuffer;
	_audio_buffer_size = inSize;
}
