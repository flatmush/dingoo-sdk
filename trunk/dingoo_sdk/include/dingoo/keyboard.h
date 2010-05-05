#ifndef __keyboard_h__
#define __keyboard_h__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
	unsigned long pressed;
	unsigned long released;
	unsigned long status;
} KEY_STATUS;

//extern void kbd_get_status(KEY_STATUS* ks);
extern void _kbd_get_status(KEY_STATUS* ks);

#ifdef __cplusplus
}
#endif

#endif
