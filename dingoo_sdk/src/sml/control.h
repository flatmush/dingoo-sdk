#ifndef __sml_control_h__
#define __sml_control_h__
/*!
	\file control.h
	\brief Defines the button codes and methods for polling the buttons for changes in
	their state.
*/
#include <stdint.h>
#include <stdbool.h>

#define CONTROL_POWER         7 /*!< the power slider (does not track the HOLD position) */

#define CONTROL_BUTTON_A      31 /*!< the A button */
#define CONTROL_BUTTON_B      21 /*!< the B button */
#define CONTROL_BUTTON_X      16 /*!< the X button; Gemei X760+ triangle */
#define CONTROL_BUTTON_Y      6  /*!< the Y button; Gemei X760+ X */
#define CONTROL_BUTTON_START  11 /*!< the START button (does not exist on the Gemei X760+) */
#define CONTROL_BUTTON_SELECT 10 /*!< the SELECT button (does not exist on the Gemei X760+) */

#define CONTROL_TRIGGER_LEFT  8  /*!< the left shoulder button (does not exist on the Gemei X760+) */
#define CONTROL_TRIGGER_RIGHT 29 /*!< the right shoulder button (does not exist on the Gemei X760+) */

#define CONTROL_DPAD_UP       20 /*!< the UP button on the directional pad */
#define CONTROL_DPAD_DOWN     27 /*!< the DOWN button on the directional pad */
#define CONTROL_DPAD_LEFT     28 /*!< the LEFT button on the directional pad */
#define CONTROL_DPAD_RIGHT    18 /*!< the RIGHT button on the directional pad */

/*! Determines if a control button was just pressed since the last poll.
	\param inControl the control to check
	\return non-zero if the control was pressed since the last poll, otherwise returns zero. 
*/
#define control_just_pressed(inControl)   ( control_check(inControl).pressed &&  control_check(inControl).changed)

/*! Determines if a control button was just released since the last poll.
	\param inControl the control to check
	\return non-zero if the control was released since the last poll, otherwise returns zero.
*/
#define control_just_released(inControl)  (!control_check(inControl).pressed &&  control_check(inControl).changed)

/*! Determines if a control button is still being pressed since the last poll.
	\param inControl the control to check
	\return non-zero if the control is still being held since the last poll, otherwise returns zero.
*/
#define control_still_pressed(inControl)  ( control_check(inControl).pressed && !control_check(inControl).changed)

/*! Determines if a control button was released since the last poll.
	\param inControl the control to check
	\return non-zero if the control was released since the last poll, otherwise returns zero.
*/
#define control_still_released(inControl) (!control_check(inControl).pressed && !control_check(inControl).changed)

/*! Tracks the state of a control button.
*/
typedef struct {
	bool changed; /*!< indicates this control has changed since the last poll */
	bool pressed; /*!< indicates this control is being pressed down */
	bool repeat; /*!< indicates this control is still being held down since the last poll */
} control_state;

extern void          control_init(); /*!< initializes the control state tracker */
extern void          control_term(); /*!< terminates the control state tracker and frees it from memory */
extern void          control_poll(); /*!< updates the control state tracker */

/*! Gets the control state of the specified control button.
	\param inControl the control button to get the state of
	\return the control state of the specified control button
*/
extern control_state control_check(uint32_t inControl);

/*! Instructs the given control to ignore changes in its state for the specified duration.
	\param inDuration the duration, in milliseconds, to ignore changes in state
*/
extern void          control_lock(uint32_t inDuration);

#endif
