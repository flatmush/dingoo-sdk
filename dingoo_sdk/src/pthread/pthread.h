#ifndef __pthread_h__
#define __pthread_h__

#include <pthread/noreturn.h>

typedef uintptr_t pthread_t;
typedef void      pthread_attr_t;

extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine)(void*), void *arg);
extern noreturn void pthread_exit(void *value_ptr);

extern int pthread_join(pthread_t thread, void **value_ptr);

#endif
