#include "vrtkey.h"
#include "gfxFont.h"

#define NMBR_KEYS_W 10
#define NMBR_KEYS_H 4
#define OPEN_CLOSE_SPEED 10
#define TEXTFIELD_HEIGHT 18

#define STATE_CLOSED 0
#define STATE_OPENED 1

gfx_font* font;
char* inputStr;
int inputLength;
int cursorPosition;
int startX;
int startY;
int width;
int height;
int width_tot;
int height_tot;
int keyW;
int keyH;
int okCancelW;
int okCancelH;
int textfieldH;

bool caps;
int selectedX;
int selectedY;
bool delayInput;

// Options
bool showTextfield;
bool enableCancelButton;

uint8_t state;
uint8_t percentOpened;

void _vrtkey_setsize()
{
	keyW = 15;
	keyH = 15;
	textfieldH = (showTextfield ? TEXTFIELD_HEIGHT : 0);
	width = keyW * NMBR_KEYS_W;
	height = keyH * NMBR_KEYS_H;
	okCancelW = keyW * 4;
	okCancelH = keyH * 1;
	width_tot = width + okCancelW;
	height_tot = height + textfieldH + 15;
	startX = (320 - width_tot) / 2;
	startY = 240 - height_tot;
}

bool vrtkey_init(uint8_t mode)
{
	font = gfx_font_load_from_buffer(gfxFont, gfxFontSize, COLOR_MAGENTA);
	if (font == NULL)
		return false;

	inputStr = NULL;
	inputLength = 0;
	cursorPosition = 0;

	// Options
	showTextfield = (mode == VRTKEY_MODE_WITHTEXTFIELD);
	enableCancelButton = true;

	_vrtkey_setsize();

	state = STATE_CLOSED;
	percentOpened = 0;
	caps = false;

	delayInput = false;

	return true;
}

void vrtkey_term()
{
	gfx_font_delete(font);
}

// New options can easily be added here
void vrtkey_config(uint8_t option, int value)
{
	switch (option)
	{
		case VRTKEY_OPT_ENABLECANCEL:
			enableCancelButton = (value == 1);
			break;
		case VRTKEY_OPT_SHOWTEXTFIELD:
			showTextfield = (value == 1);
			_vrtkey_setsize();
			break;
	}
}

int _mod(int a, int b)
{
	if (a < 0)
		a += b;

	if (a >= b)
		a -= b;

	return a;
}

char _getChar(int x, int y)
{
	if (x < NMBR_KEYS_W - 3)
	{
		// A, B, C
		char c = (caps ? 'A' : 'a') + x + y * (NMBR_KEYS_W - 3);
		char end = (caps ? 'Z' : 'z');
		if (c <= end)
			return c;

		if (c == end + 1)
			return '-';
		if (c == end + 2)
			return '?';
	}
	else if (x < NMBR_KEYS_W)
	{
		// 1, 2, 3
		char c = '1' + (x - (NMBR_KEYS_W - 3)) + y * 3;
		if (c <= '9')
			return c;
		
		if (c == '9' + 1)
			return '_';
		else if (c == '9' + 2)
			return '0';
		else if (c == '9' + 3)
			return '.';
	}
	
	// Else invalid
	return 0;
}

// Updates and checks key presses
uint8_t vrtkey_update()
{
	if (delayInput)
	{
		delayInput = false;
		return VRTKEY_NO_ACTION;
	}

	if (state == STATE_CLOSED && percentOpened > 0)
		percentOpened -= OPEN_CLOSE_SPEED;

	if (state == STATE_OPENED && percentOpened < 100)
		percentOpened += OPEN_CLOSE_SPEED;

	if (state == STATE_CLOSED)
		return VRTKEY_NO_ACTION;

	if (control_just_pressed(CONTROL_DPAD_UP))
		selectedY = _mod(selectedY - 1, NMBR_KEYS_H);

	if (control_just_pressed(CONTROL_DPAD_DOWN))
		selectedY = _mod(selectedY + 1, NMBR_KEYS_H);

	if (control_just_pressed(CONTROL_DPAD_LEFT))
		selectedX = _mod(selectedX - 1, NMBR_KEYS_W + 1);

	if (control_just_pressed(CONTROL_DPAD_RIGHT))
		selectedX = _mod(selectedX + 1, NMBR_KEYS_W + 1);

	if (control_just_pressed(CONTROL_BUTTON_A))
	{
		bool charInput = false;

		if (selectedX == NMBR_KEYS_W)
		{
			switch (selectedY)
			{
				case 0:
					return VRTKEY_ACCEPT;
				case 1:
					if (enableCancelButton)
						return VRTKEY_CANCEL;
					else
						break;
				case 2:
					caps = !caps;
					break;
				case 3:
					charInput = true;
					break;
			}
		}
		else
		{
			charInput = true;
		}

		if (charInput)
		{
			// Insert key to string

			char c;
			// Check space button
			if (selectedX == NMBR_KEYS_W && selectedY == 3)
				c = ' ';
			else
				c = _getChar(selectedX, selectedY);

			// Make sure we have space
			int curLength = strlen(inputStr);
			if (curLength + 1 == inputLength)
			{
				return VRTKEY_NO_ACTION;
			}

			int i;
			for (i = curLength; i >= 0; i--)
			{
				inputStr[i + 1] = inputStr[i];
				if (i == cursorPosition)
					break;
			}

			inputStr[cursorPosition] = c;
			cursorPosition++;
		}
	}

	if (control_just_pressed(CONTROL_BUTTON_B) && enableCancelButton)
	{
		return VRTKEY_CANCEL;
	}

	if (control_just_pressed(CONTROL_BUTTON_X))
	{
		if (cursorPosition > 0)
		{
			int curLength = strlen(inputStr);
			int i;
			for (i = cursorPosition - 1; i <= curLength; i++)
			{
				inputStr[i] = inputStr[i + 1];
			}

			cursorPosition--;
		}
	}

	if (showTextfield)
	{
		if (control_just_pressed(CONTROL_TRIGGER_RIGHT))
		{
			if (cursorPosition < strlen(inputStr))
				cursorPosition++;
		}

		if (control_just_pressed(CONTROL_TRIGGER_LEFT))
		{
			if (cursorPosition > 0)
				cursorPosition--;
		}
	}

	return VRTKEY_NO_ACTION;
}

void vrtkey_draw()
{
	if (percentOpened == 0)
		return;

	int showX = startX;
	int showY = startY + ((100 - percentOpened) * height_tot) / 100;

	gfx_color borderColor = gfx_color_rgb(70, 70, 70);
	gfx_color bgColor = gfx_color_rgb(10, 10, 10);
	gfx_color bgColorBtn = gfx_color_rgb(40, 40, 40);
	gfx_color selectColor = gfx_color_rgb(0, 0, 140);
	gfx_color selectColorBg = gfx_color_rgb(0, 0, 125);

	int i, x, y, c, c2;

	gfx_rect_fill_draw(showX, showY, width_tot, height_tot, bgColor);

	c = caps ? 'A' : 'a';
	c2 = '1';

	for (y = 0; y < NMBR_KEYS_H; y++)
	{
		for (x = 0; x < NMBR_KEYS_W; x++)
		{
			int xp = showX + x * keyW;
			int yp = showY + textfieldH + y * keyH;

			gfx_rect_fill_draw(xp, yp, keyW - 2, keyH - 2, x == selectedX && y == selectedY ? selectColorBg : bgColorBtn);
			gfx_rect_draw(xp, yp, keyW - 2, keyH - 2, x == selectedX && y == selectedY ? selectColor : borderColor);
			gfx_font_print_char(xp + 4, yp + 2, font, _getChar(x, y));
		}
	}

	char* specialButtonsText[] = {"Done", "Cancel", "Caps", "Space"};

	for (i = 0; i < 4; i++)
	{
		gfx_color bg;
		gfx_color brdr;
		gfx_font_color_clear(font);

		if (i == 0)
		{
			bg = gfx_color_rgb(0, 125, 0);
			brdr = gfx_color_rgb(0, 140, 0);
		}
		else if (i == 1)
		{
			if (enableCancelButton)
			{
				bg = gfx_color_rgb(125, 0, 0);
				brdr = gfx_color_rgb(140, 0, 0);
			}
			else
			{
				gfx_font_color_set(font, gfx_color_rgb(70, 70, 70));
				bg = bgColorBtn;
				brdr = gfx_color_rgb(140, 0, 0);
			}
		}
		else
		{
			bg = selectColorBg;
			brdr = selectColor;
		}

		gfx_rect_fill_draw(showX + width, showY + textfieldH + okCancelH * i, okCancelW, okCancelH - 2, NMBR_KEYS_W == selectedX && i == selectedY ? bg : bgColorBtn);
		gfx_rect_draw(showX + width, showY + textfieldH + okCancelH * i, okCancelW, okCancelH - 2, NMBR_KEYS_W == selectedX && i == selectedY ? brdr : borderColor);
		gfx_font_print_center_ex(showY + textfieldH + okCancelH * i + 2, okCancelW, showX + width, font, specialButtonsText[i]);
	}

	char* keysText = enableCancelButton ? "(A) Select  (B) Cancel  (X) Bckspc" : "(A) Select  (X) Backspace";
	gfx_font_print(showX, showY + textfieldH + height + 2, font, keysText);

	if (showTextfield)
	{
		int text_offset = 2;
		gfx_rect_fill_draw(showX, showY, width_tot, textfieldH - 2, COLOR_WHITE);
		gfx_rect_draw(showX, showY, width_tot, textfieldH - 2, borderColor);

		gfx_font_color_set(font, COLOR_BLACK);
		int charWidth = (font->texture->width >> 4);
		gfx_font_print(showX + text_offset, showY + 4, font, inputStr);
		gfx_rect_draw(showX + cursorPosition * charWidth + text_offset, showY + 2, 1, (textfieldH - 4 - 2), COLOR_BLACK);
		gfx_font_color_clear(font);
	}
}

// max_length INCLUDING \0
void vrtkey_open(char* buffer, uint8_t max_length, bool instantly)
{
	selectedX = 0;
	selectedY = 0;
	caps = false;
	delayInput = true;

	inputStr = buffer;
	inputLength = max_length;
	cursorPosition = strlen(inputStr);

	state = STATE_OPENED;
	if (instantly)
		percentOpened = 100;
}

void vrtkey_close(bool instantly)
{
	inputStr = NULL;
	inputLength = 0;
	cursorPosition = 0;

	state = STATE_CLOSED;
	if (instantly)
		percentOpened = 0;
}

bool vrtkey_isopen()
{
	return state == STATE_OPENED;
}
