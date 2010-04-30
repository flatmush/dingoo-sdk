#ifndef __sml_timer_h__
#define __sml_timer_h__

#include <stdbool.h>
#include <stdint.h>
#include <time.h>



typedef struct {
	uint32_t start;
	uint32_t last;
} timer;

#define timer_resolution CLOCKS_PER_SEC

extern timer*   timer_create();
extern void     timer_delete(timer* inTimer);
extern uint32_t timer_delta(timer* inTimer);
extern uint32_t timer_total(timer* inTimer);

#endif
