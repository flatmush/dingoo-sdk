#ifndef __libc_sys_time_h__
#define __libc_sys_time_h__

#ifdef __cplusplus
extern "C"
{
#endif

struct timeval {
	long tv_sec;                  // Seconds
	long tv_usec;                 // Microseconds
};

int gettimeofday(struct timeval* tv, void* tzp);

#ifdef __cplusplus
}
#endif

#endif
