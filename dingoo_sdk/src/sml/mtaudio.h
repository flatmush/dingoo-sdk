#ifndef __sml_mtaudio_h__
#define __sml_mtaudio_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

extern bool mtaudio_init(void* inCallback,uint32_t inRate);
extern void mtaudio_term();

extern void mtaudio_buffer_set(void* inBuffer, uintptr_t inSize, uint8_t inChannels, uint8_t inVolume, uint32_t inRate);

extern void mtaudio_pause(bool pause);

#ifdef __cplusplus
}
#endif

#endif
