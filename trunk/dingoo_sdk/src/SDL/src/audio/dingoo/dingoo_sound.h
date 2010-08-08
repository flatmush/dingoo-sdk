#ifndef __dingoo_sound_h__
#define __dingoo_sound_h__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dingoo/ucos2.h>
#include <dingoo/audio.h>
#include <ctype.h>

#define min(x, y) (x < y ? x : y)

#define  TASK_START_PRIO 5
#define  TASK_STK_SIZE   1024                   // Size of each task's stacks (# of WORDs)
#define  NO_TASKS        2                      // Number of identical tasks 

#define SOUNDBUFFERS 3

extern char _lastError[];

int dingooSoundInit(waveout_args waveformat, uint32_t bufsize, void (*callback)(void *, uint32_t));
void dingooSoundClose();
void dingooSoundPause(bool b);
bool dingooSoundIsPaused();
void dingooSoundClearBuffers();
void _dingooSoundPlay(void *none);
void _dingooSoundUpdate(void *none);
char* dingooSoundLastError();

#endif
