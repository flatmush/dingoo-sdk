#include <stdlib.h>
#include <sys/time.h>
#include <dingoo/ucos2.h>

int gettimeofday(struct timeval* tv, void* tzp){
	if(tv != NULL) {
		long tt = OSTimeGet();
		tv->tv_sec = tt / 100;
		tv->tv_usec = ((tt * 10000) % 1000000);
	}
	return 0;
}
