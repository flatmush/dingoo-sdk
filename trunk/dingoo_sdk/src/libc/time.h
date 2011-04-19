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

struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

#define CLOCKS_PER_SEC (OS_TICKS_PER_SEC * 10000)

extern clock_t clock();

extern time_t time(time_t* timer); //stub
extern struct tm* localtime(const time_t* timer); //stub
extern time_t mktime(struct tm *tm); //stub

#ifdef __cplusplus
}
#endif

#endif
