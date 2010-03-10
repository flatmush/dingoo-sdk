#include <stdlib.h>
#include <dingoo/keyboard.h>
#include "control.h"
#include "timer.h"

KEY_STATUS _control_status[2];
uint32_t   _control_duration[32] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};
uint32_t   _control_repeat = (timer_resolution / 4);
timer*     _control_timer  = NULL;
uint32_t   _control_lock   = 0;

void control_init() {
	_kbd_get_status(_control_status);
	_control_status[1] = _control_status[0];
	_control_lock = 0;
	_control_timer = timer_create();
}

void control_term() {
	timer_delete(_control_timer);
	_control_timer = NULL;
}

void control_poll() {
	uint32_t tempDelta = timer_delta(_control_timer);
	if(tempDelta < _control_lock) {
		_control_lock -= tempDelta;
		return;
	}

	_control_status[1] = _control_status[0];
	_kbd_get_status(_control_status);
	_control_lock = 0;

	uintptr_t i;
	for(i = 0; i < 32; i++) {
		if((_control_status[0].status ^ _control_status[1].status) & (1 << i))
			_control_duration[i] = 0;
		else if(_control_duration[i] < _control_repeat)
			_control_duration[i] += tempDelta;
	}
}

control_state control_check(uint32_t inControl) {
	control_state tempOut;
	if(_control_lock == 0) {
		tempOut.changed = (((_control_status[0].status ^ _control_status[1].status) & (1 << inControl)) != 0);
		tempOut.pressed = ((_control_status[0].status & (1 << inControl)) != 0);
		tempOut.repeat  = (tempOut.pressed && (tempOut.changed || (_control_duration[inControl] >= _control_repeat)));
	} else {
		tempOut.changed = false;
		tempOut.pressed = false;
		tempOut.repeat  = false;
	}
	return tempOut;
}

void control_lock(uint32_t inDuration) {
	_control_lock += inDuration;
}
