#ifndef __dingoo_time_h__
#define __dingoo_time_h__

#include <dingoo/ucos2.h>

extern unsigned int GetTickCount();
#define GetTickCount_Resolution 1000000
// The macro below fixes the problems with GetTickCount, but the function shouldn't be used.
#define GetTickCount() ((OSTimeGet() * GetTickCount_Resolution) / OS_TICKS_PER_SEC)

extern void udelay(uint32_t);
extern void mdelay(uint32_t);

extern void delay_ms(uint32_t);

#endif
