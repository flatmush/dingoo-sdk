#include <time.h>

clock_t clock() {
	return (clock_t)(OSTimeGet() * 10000);
}

time_t time(time_t* timer) {
	return -1;
}

static struct tm tm_storage;

struct tm* localtime(const time_t* timer)
{
	return &tm_storage;
}

time_t mktime(struct tm *tm)
{
    return 0;
}
