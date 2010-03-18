#ifndef __wav_h__
#define __wav_h__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	void*    data;
	uint32_t sample_count;
	uint32_t sample_rate;
	uint8_t  sample_bits;
	uint8_t  channels;
} wav_buffer;

extern wav_buffer* wav_load(char* inFile);
extern void        wav_delete(wav_buffer* inBuffer);

#endif
