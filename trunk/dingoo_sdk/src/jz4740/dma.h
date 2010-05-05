#ifndef __jz4740_dma_h__
#define __jz4740_dma_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

extern uintptr_t dma_free();
extern bool      dma_wait(uintptr_t inChannel, uintptr_t inTimeout);
extern bool      dma_copy(uintptr_t inChannel, void* inDest, const void* inSource, uintptr_t inSize);
extern bool      dma_set(uintptr_t inChannel, void* inDest, uintptr_t inDestSize, void* inSource, uintptr_t inSourceSize);

#ifdef __cplusplus
}
#endif

#endif
