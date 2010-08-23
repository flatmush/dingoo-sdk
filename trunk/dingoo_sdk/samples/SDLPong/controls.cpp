#include "controls.h"

ControlState controlState[13];

// Only true the moment you press
bool controlJustPressed(Control control)
{
	return (controlState[control].pressed && controlState[control].changed);
}

// As long as control is pressed
bool controlPressed(Control control)
{
	return (controlState[control].pressed);
}

void controlSetPressed(Control control, bool pressed)
{
	controlState[control].changed = (controlState[control].pressed != pressed);
	controlState[control].pressed = pressed;
}
