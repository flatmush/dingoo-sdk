#include <stdlib.h>
#include <dingoo/slcd.h>
#include <dingoo/cache.h>
#include <dingoo/keyboard.h>

#include "FontWhite.h"

#define CONTROL_BUTTON_SELECT 0x00000400

unsigned short *g_pGameDecodeBuf = 0L;

void PutChar(int X, int Y, unsigned char Char) {
	unsigned short *Frame;
	int x, y;
	Frame = _lcd_get_frame();
	for(y=0; y<CHAR_HEIGHT; y++)
		for(x=0; x<CHAR_WIDTH; x++)
			Frame[(Y*CHAR_HEIGHT+y)*320 + (X*CHAR_WIDTH+x)] = CHARS[Char][y*CHAR_WIDTH+x];
}

void PutString(int X, int Y, unsigned char *Str) {
	while(*Str != 0) {
		PutChar(X, Y, *Str);
		X++;
		Str++;
		if(X > 39) {
			X = 0;
			Y++;
			if(Y > 13) Y = 0;
		}
	}
}


int main(int argc, char** argv) {
	KEY_STATUS KS;
	PutString(10, 5, "Hello, World!");
	__dcache_writeback_all();
	_lcd_set_frame();
	do {
		_kbd_get_status(&KS);
	} while((KS.status & CONTROL_BUTTON_SELECT) == 0);
	return EXIT_SUCCESS;
}

