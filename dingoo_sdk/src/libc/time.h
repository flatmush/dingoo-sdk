#ifndef __libc_time_h__
#define __libc_time_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <dingoo/ucos2.h>

typedef unsigned long int clock_t;
typedef long int time_t;

#define CLOCKS_PER_SEC OS_TICKS_PER_SEC

extern clock_t clock();

extern time_t time(time_t* timer);

#ifdef __cplusplus
}
#endif

#endif
