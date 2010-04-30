#ifndef __sml_wav_h__
#define __sml_wav_h__

#include <stdbool.h>
#include <stdint.h>

#include <sml/sound.h>

extern sound_t* wav_load(char* inFile);
extern void     wav_delete(sound_t* inBuffer);

#endif
