#ifndef __time_h__
#define __time_h__

#include <stddef.h>
#include <dingoo/ucos2.h>

typedef unsigned long int clock_t;

#define CLOCKS_PER_SEC OS_TICKS_PER_SEC

extern clock_t clock();

#endif
