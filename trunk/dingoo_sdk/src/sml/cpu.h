#ifndef __sml_cpu_h__
#define __sml_cpu_h__

#include <stdbool.h>
#include <stdint.h>

extern bool cpu_clock_set(uintptr_t inCore, uintptr_t inMem);
extern void cpu_clock_get(uintptr_t* outCore, uintptr_t* outMem);

#endif
