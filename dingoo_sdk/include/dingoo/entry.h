#ifndef __entry_h__
#define __entry_h__

typedef struct {
	unsigned long pressed;
	unsigned long released;
	unsigned long status;
} KEY_STATUS;

extern int _sys_judge_event(void*);
/*
#define _sys_judge_event(x) 0

extern void lcd_set_frame();
#define _lcd_set_frame lcd_set_frame

extern unsigned short* lcd_get_frame();
#define _lcd_get_frame lcd_get_frame

extern void kbd_get_status(KEY_STATUS* ks);
#define _kbd_get_status kbd_get_status
*/

extern wchar_t* __to_unicode_le(char*);
extern char*    __to_locale_ansi(wchar_t*);


extern unsigned int GetTickCount();

#endif
