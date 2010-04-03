#ifndef __control_h__
#define __control_h__
/**
=sml/control.h=
Defines the button codes and methods for polling the buttons for changes in
their state.
*/
#include <stdint.h>
#include <stdbool.h>


/**
==Macros==
===Control codes===
  * *CONTROL`_`POWER*: the power slider (does not track the HOLD position)
  * *CONTROL`_`BUTTON`_`A*: the A button
  * *CONTROL`_`BUTTON`_`B*: the B button
  * *CONTROL`_`BUTTON`_`X*: the X button
  * *CONTROL`_`BUTTON`_`Y*: the Y button
  * *CONTROL`_`BUTTON`_`START*: the START button (does not exist on the Gemei X760+)
  * *CONTROL`_`BUTTON`_`SELECT*: the SELECT button (does not exist on the Gemei X760+)
  * *CONTROL`_`TRIGGER`_`LEFT*: the left shoulder button (does not exist on the Gemei X760+)
  * *CONTROL`_`TRIGGER`_`RIGHT*: the right shoulder button (does not exist on the Gemei X760+)
  * *CONTROL`_`DPAD`_`UP*: the UP button on the directional pad
  * *CONTROL`_`DPAD`_`DOWN*: the DOWN button on the directional pad
  * *CONTROL`_`DPAD`_`LEFT*: the LEFT button on the directional pad
  * *CONTROL`_`DPAD`_`RIGHT*: the RIGHT button on the directional pad

===Macro functions===
  * _int_ *control`_`just`_`pressed*( [#Control_codes control_code] ): returns 0 if the control state is set to "just recently pressed", non-zero otherwise.
  * _int_ *control`_`just`_`released*( [#Control_codes control_code] ): returns 0 if the control state is set to "just recently released", non-zero otherwise.  * _int_ *control`_`still`_`pressed*( [#Control_codes control_code] ): returns 0 if the control state is set to "still pressed", non-zero otherwise.  * _int_ *control`_`still`_`released*( [#Control_codes control_code] ): returns 0 if the control state is set to "still released", non-zero otherwise.
*/
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

#define control_just_pressed(inControl)   ( control_check(inControl).pressed &&  control_check(inControl).changed)
#define control_just_released(inControl)  (!control_check(inControl).pressed &&  control_check(inControl).changed)
#define control_still_pressed(inControl)  ( control_check(inControl).pressed && !control_check(inControl).changed)
#define control_still_released(inControl) (!control_check(inControl).pressed && !control_check(inControl).changed)

/**
==Structs==
===control_state===
Members:
  * _bool_ *changed*: indicates this control has changed since the last check
  * _bool_ *pressed*: indicates this control is being pressed down
  * _bool_ *repeat*: indicates this control is _still_ being held down since the last check
*/
typedef struct {
	bool changed;
	bool pressed;
	bool repeat;
} control_state;

/**
==Functions==
Non-macro global functions.
  * _void_ *control`_`init*( _void_ ): initializes the control state tracker
  * _void_ *control`_`term*( _void_ ): terminates the control state tracker
  * _void_ *control`_`poll*( _void_ ): updates the control state tracker
  * [#control_state control_state] *control_check*( [#Control_codes control_code] ): gets the status of the specified control
  * _void_ *control`_`lock*( [#Control_codes control_code] ): prevents the specified control from being tracked
*/
extern void          control_init();
extern void          control_term();

extern void          control_poll();
extern control_state control_check(uint32_t inControl);
extern void          control_lock(uint32_t inDuration);

#endif
