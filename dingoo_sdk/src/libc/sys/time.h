#ifndef __libc_sys_time_h__
#define __libc_sys_time_h__

struct timeval {
	long tv_sec;                  // Seconds
	long tv_usec;                 // Microseconds
};

int gettimeofday(struct timeval* tv, void* tzp);

#endif
