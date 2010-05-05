#ifndef __jz4740_cpu_h__
#define __jz4740_cpu_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

extern bool cpu_clock_set(uintptr_t inCore); //, uintptr_t inMem);
extern void cpu_clock_get(uintptr_t* outCore, uintptr_t* outMem);

#ifdef __cplusplus
}
#endif

#endif
