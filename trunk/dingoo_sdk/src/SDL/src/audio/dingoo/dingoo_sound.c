#include "dingoo_sound.h"

waveout_inst* woHandle;

OS_STK   TaskStkPlay[TASK_STK_SIZE_PLAY];      // Play task stack
OS_STK   TaskStkUpdate[TASK_STK_SIZE_UPDATE];  // Update task stack
char     TaskData[NO_TASKS];                   // Parameters to pass to each task

volatile int curReadBuffer;
int curWriteBuffer;
static volatile int soundThreadPause;
static volatile int soundThreadExit;

bool dingooSoundStarted = false;
uint32_t dingooSoundBufferSize;

int8_t dingooSoundVolume;

char _lastError[256] = {0};


uint8_t* dingooSoundBufferTotal = NULL;			// The raw allocated mixing buffer
void* dingooSoundBuffers[SOUNDBUFFERS];			// Wave audio fragments

void (*getDataCallback)(void *, uint32_t);


int dingooSoundInit(waveout_args waveformat, uint32_t bufsize, void (*callback)(void *, uint32_t))
{
	getDataCallback = callback;

	dingooSoundBufferSize = bufsize;

	dingooSoundBufferTotal = (uint8_t*)malloc(SOUNDBUFFERS * dingooSoundBufferSize);
	if (dingooSoundBufferTotal == NULL)
	{
		return -1;
	}

	int i;
	for (i = 0; i < SOUNDBUFFERS; ++i)
	{
		dingooSoundBuffers[i] = (void *)(dingooSoundBufferTotal + ((dingooSoundBufferSize * i)));
	}

	dingooSoundClearBuffers();

	sprintf(_lastError, "No Error");

	dingooSoundVolume = waveformat.volume;
	woHandle = waveout_open(&waveformat);
	
	curReadBuffer = 0;
	curWriteBuffer = 0;
	soundThreadExit = 0;
	soundThreadPause = 1;

	OSTaskCreate(_dingooSoundPlay, (void *) 0, (void *)&TaskStkPlay[TASK_STK_SIZE_PLAY - 1], TASK_START_PRIO);
	OSTaskCreate(_dingooSoundUpdate, (void *) 0, (void *)&TaskStkUpdate[TASK_STK_SIZE_UPDATE - 1], TASK_START_PRIO + 1);
	dingooSoundStarted = true;

	return 0;
}

void dingooSoundClose()
{
	if (!dingooSoundStarted)
		return;

	soundThreadPause = 1;
	soundThreadExit = 1;

	while (soundThreadExit - 1 < NO_TASKS);  // to avoid pb with waveout_close -> can hang ??

	if (dingooSoundBufferTotal != NULL)
		free(dingooSoundBufferTotal);

	waveout_close(woHandle);
}

void dingooSoundPause(bool b)
{
	soundThreadPause = b ? 1 : 0;
}

bool dingooSoundIsPaused()
{
	return soundThreadPause;
}

void dingooSoundClearBuffers()
{
	if (dingooSoundBufferTotal != NULL)
		memset(dingooSoundBufferTotal, 0, dingooSoundBufferSize * SOUNDBUFFERS);
}

int8_t dingooSoundGetVolume()
{
	return dingooSoundVolume;
}

void _dingooSoundSetVolume(int8_t v)
{
#ifdef MPU_CC1800
	SYSSetVolume(v);
#else
	waveout_set_volume(v);
#endif
}

void dingooSoundSetVolume(int8_t v)
{
	if (v < 0)
		dingooSoundVolume = 0;
	else if (v > VOLUME_MAX)
		dingooSoundVolume = VOLUME_MAX;
	else
		dingooSoundVolume = v;

	_dingooSoundSetVolume(dingooSoundVolume);
}

void dingooSoundVolumeIncrease()
{
	if (dingooSoundVolume < VOLUME_MAX)
		dingooSoundVolume += VOLUME_STEP;

	if (dingooSoundVolume > VOLUME_MAX)
		dingooSoundVolume = VOLUME_MAX;

	_dingooSoundSetVolume(dingooSoundVolume);
}

void dingooSoundVolumeDecrease()
{
	if (dingooSoundVolume > 0)
		dingooSoundVolume -= VOLUME_STEP;

	if (dingooSoundVolume < 0)
		dingooSoundVolume = 0;

	_dingooSoundSetVolume(dingooSoundVolume);
}

void _dingooSoundPlay(void *none)
{
	do
	{
		if (soundThreadPause == 0)
		{
			curReadBuffer++;
			if (curReadBuffer == SOUNDBUFFERS)
				curReadBuffer = 0;

			waveout_write(woHandle, dingooSoundBuffers[curReadBuffer], dingooSoundBufferSize);
		}
		else
		{
			OSTimeDly(1);
		}
	} while (soundThreadExit == 0); // Until the end of the sound thread

	soundThreadExit++;

	OSTaskDel(TASK_START_PRIO);
}

void _dingooSoundUpdate(void *none)
{
	while (soundThreadExit == 0)
	{
		while (curWriteBuffer != curReadBuffer)
		{
			uint8_t *pCur = dingooSoundBuffers[curWriteBuffer];
			
			getDataCallback(pCur, dingooSoundBufferSize);

			curWriteBuffer++;
			curWriteBuffer = curWriteBuffer % SOUNDBUFFERS;
		}

		OSTimeDly(1);
	}

	soundThreadExit++;

	OSTaskDel(TASK_START_PRIO + 1);
}

char* dingooSoundLastError()
{
	return _lastError;
}
