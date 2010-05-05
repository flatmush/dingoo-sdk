#ifndef __sml_sound_h__
#define __sml_sound_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	int16_t* sample_data;
	uint32_t sample_count;
	uint32_t sample_rate;
	uint8_t  sample_bits;
	uint8_t  channels;
} sound_t;

extern bool sound_init();
extern void sound_term();

extern uintptr_t sound_play(sound_t inSound, bool inLoop);
extern void      sound_stop(uintptr_t inSound);

extern uintptr_t sound_volume_set(uintptr_t inVolume);

#ifdef __cplusplus
}
#endif

#endif
