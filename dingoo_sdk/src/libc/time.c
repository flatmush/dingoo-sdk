#include <time.h>

clock_t clock() {
	return (clock_t)(OSTimeGet() * 10000);
}

time_t time(time_t* timer) {
	return -1;
}
