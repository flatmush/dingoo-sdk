#include <time.h>

clock_t clock() {
	return (clock_t)OSTimeGet();
}
