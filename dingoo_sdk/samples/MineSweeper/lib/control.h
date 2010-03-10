#ifndef __control_h__
#define __control_h__

#include <stdint.h>
#include <stdbool.h>

#define CONTROL_BUTTON_A      31
#define CONTROL_BUTTON_B      21
#define CONTROL_BUTTON_X      16
#define CONTROL_BUTTON_Y      6
#define CONTROL_BUTTON_START  11
#define CONTROL_BUTTON_SELECT 10

#define CONTROL_TRIGGER_LEFT  8
#define CONTROL_TRIGGER_RIGHT 29

#define CONTROL_DPAD_UP       20
#define CONTROL_DPAD_DOWN     27
#define CONTROL_DPAD_LEFT     28
#define CONTROL_DPAD_RIGHT    18

typedef struct {
	bool changed;
	bool pressed;
	bool repeat;
} control_state;

extern void          control_init();
extern void          control_term();
extern void          control_poll();
extern control_state control_check(uint32_t inControl);
extern void          control_lock(uint32_t inDuration);

#endif
