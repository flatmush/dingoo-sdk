#ifndef _CONTROLS_H_
#define _CONTROLS_H_

enum Control { CONTROL_A, CONTROL_B, CONTROL_X, CONTROL_Y, CONTROL_LEFT, CONTROL_RIGHT, CONTROL_UP, CONTROL_DOWN, CONTROL_L, CONTROL_R, CONTROL_START, CONTROL_SELECT, CONTROL_POWER };

typedef struct {
	bool changed;
	bool pressed;
} ControlState;

extern ControlState controlState[];

bool controlJustPressed(Control control);
bool controlPressed(Control control);
void controlSetPressed(Control control, bool pressed);

#endif
