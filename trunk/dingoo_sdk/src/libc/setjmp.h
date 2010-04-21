#ifndef __libc_setjmp_h__
#define __libc_setjmp_h__

#include <stdint.h>

typedef uintptr_t jmp_buf[32];

extern int  setjmp(jmp_buf env);
extern void longjmp(jmp_buf env, int value);

#endif
