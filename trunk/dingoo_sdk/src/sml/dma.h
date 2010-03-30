#ifndef __dma_h__
#define __dma_h__

#include <stdbool.h>
#include <stdint.h>

extern bool dma_wait(uintptr_t inChannel, uintptr_t inTimeout);
extern bool dma_copy(uintptr_t inChannel, void* inDest, void* inSource, uintptr_t inSize);
extern bool dma_set(uintptr_t inChannel, void* inDest, uintptr_t inDestSize, void* inSource, uintptr_t inSourceSize);

#endif
