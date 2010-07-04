#ifndef __sml_wav_h__
#define __sml_wav_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <sml/sound.h>

extern sound_t* wav_load(char* inFile);
extern sound_t* wav_load_from_buffer(uint8_t* buffer, size_t size);
extern void     wav_delete(sound_t* inBuffer);

#ifdef __cplusplus
}
#endif

#endif
