#ifndef __vrtkey_h__
#define __vrtkey_h__

#ifdef __cplusplus
extern "C"
{
#endif

#define VRTKEY_NO_ACTION 0
#define VRTKEY_ACCEPT    1
#define VRTKEY_CANCEL    2

#define VRTKEY_MODE_BASIC         0
#define VRTKEY_MODE_WITHTEXTFIELD 1

#define VRTKEY_OPT_ENABLECANCEL  0
#define VRTKEY_OPT_SHOWTEXTFIELD 1

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <sml/display.h>
#include <sml/graphics.h>
#include <sml/control.h>

extern bool     vrtkey_init(uint8_t mode);
extern void     vrtkey_term();
extern void     vrtkey_config(uint8_t option, int value);
extern uint8_t  vrtkey_update();
extern void     vrtkey_draw();

extern void     vrtkey_open(char* buffer, uint8_t max_length, bool instantly);
extern void     vrtkey_close(bool instantly);
extern bool     vrtkey_isopen();

#ifdef __cplusplus
}
#endif

#endif
