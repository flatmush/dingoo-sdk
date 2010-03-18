#ifndef __control_h__
#define __control_h__

#include <stdint.h>
#include <stdbool.h>



#define CONTROL_POWER         7

#define CONTROL_BUTTON_A      31
#define CONTROL_BUTTON_B      21
#define CONTROL_BUTTON_X      16 // Gemei X760+ triangle
#define CONTROL_BUTTON_Y      6  // Gemei X760+ X
#define CONTROL_BUTTON_START  11 // Does not exist on the Gemei X760+
#define CONTROL_BUTTON_SELECT 10 // Does not exist on the Gemei X760+

#define CONTROL_TRIGGER_LEFT  8  // Does not exist on the Gemei X760+
#define CONTROL_TRIGGER_RIGHT 29 // Does not exist on the Gemei X760+

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

#define control_just_pressed(inControl) (control_check(inControl).pressed && control_check(inControl).changed)
#define control_just_released(inControl) (!control_check(inControl).pressed && control_check(inControl).changed)
#define control_still_pressed(inControl) (control_check(inControl).pressed && !control_check(inControl).changed)
#define control_still_released(inControl) (!control_check(inControl).pressed && !control_check(inControl).changed)

#endif
