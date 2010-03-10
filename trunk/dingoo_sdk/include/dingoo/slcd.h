#ifndef __slcd_h__
#define __slcd_h__

extern void  _lcd_set_frame();
extern void* _lcd_get_frame();

extern void  lcd_backlight_on();  // Enables the screen?
extern void  lcd_backlight_off(); // Disables the screen.

extern void  lcd_set_frame();
extern void* lcd_get_frame();

#endif
