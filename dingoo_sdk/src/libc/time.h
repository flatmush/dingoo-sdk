#ifndef __time_h__
#define __time_h__

#include <stddef.h>

typedef unsigned long int clock_t;

#define CLOCKS_PER_SEC 960000

extern clock_t clock();

#endif
