#include <time.h>

extern unsigned long int GetTickCount();

clock_t clock() {
	return (clock_t)GetTickCount();
}
