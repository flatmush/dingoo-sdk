#include <sml/wav.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dingoo/fsys.h>

#include <sml/fixmath.h>
#include <sml/graphics.h>
#include <sml/control.h>



typedef struct {
	char     wrcChunkID[4];
	uint32_t wrcChunkSize;
	char     wrcFormat[4];
} wavRiffChunk;

typedef struct {
	char     wfcSubchunk1ID[4];
	uint32_t wfcSubchunk1Size;
	uint16_t wfcAudioFormat;
	uint16_t wfcNumChannels;
	uint32_t wfcSampleRate;
	uint32_t wfcByteRate;
	uint16_t wfcBlockAlign;
	uint16_t wfcBitsPerSample;
} wavFmtChunk;

typedef struct {
	char   wdcSubchunk2ID[4];
	uint32_t wdcSubchunk2Size;
} wavDataChunk;



int _strncmp(const char* inStr0, const char* inStr1, size_t inLength) {
	if(inStr0 == NULL) {
		if(inStr1 == NULL)
			return 0;
		return -inStr1[0];
	}
	if(inStr1 == NULL)
		return (unsigned char)inStr0[0];
	uintptr_t i;
	for(i = 0; (i < inLength) && ((inStr0[i] != '\0') || (inStr1[i] != '\0')); i++) {
		if(inStr0[i] != inStr1[i])
			return (inStr0[i] - inStr1[i]);
	}
	return 0;
}

void draw_sine_wave(fix16_t inFrequency);

void wav_error(const char* inError, const uint32_t inNumber) {
	/*FILE* tempFile = fsys_fopen("b:\\wav_error.txt", "wb");
	if(tempFile == NULL)
		return;
	fsys_fwrite(inError, 1, strlen(inError), tempFile);
	fsys_fclose(tempFile);*/
	gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	draw_sine_wave(fix16_from_int(inNumber));
	gfx_render_target_swap();
	while(true) {
		control_poll();
		if(control_check(CONTROL_BUTTON_A).pressed)
			break;
	}
}

sound_t* wav_load(char* inFile) {
	if(inFile == NULL)
		return NULL;

	FILE* tempFile = fsys_fopen(inFile, "rb");
	if(tempFile == NULL) {
		wav_error("Unable to open file.", 2);
		return NULL;
	}

	fsys_fseek(tempFile, 0, FSYS_SEEK_END);
	uint32_t tempLen = fsys_ftell(tempFile);
	fsys_fseek(tempFile, 0, FSYS_SEEK_SET);

	sound_t* tempOut = NULL;

	wavRiffChunk tempRiffChunk;
	fsys_fread(tempRiffChunk.wrcChunkID, 1, 4, tempFile);
	fsys_fread(&tempRiffChunk.wrcChunkSize, 4, 1, tempFile);
	fsys_fread(tempRiffChunk.wrcFormat, 1, 4, tempFile);

	if(_strncmp(tempRiffChunk.wrcChunkID, "RIFF", 4) != 0) {
		wav_error("Not RIFF file.", 3);
		goto wav_load_end;
	}
	if(_strncmp(tempRiffChunk.wrcFormat, "WAVE", 4) != 0) {
		wav_error("Not wave file.", 4);
		goto wav_load_end;
	}
	if(tempRiffChunk.wrcChunkSize < (tempLen - 8)) {
		wav_error("Incorrect data length.", 5);
		goto wav_load_end;
	}

	wavFmtChunk tempFmtChunk;
	fsys_fread(tempFmtChunk.wfcSubchunk1ID, 1, 4, tempFile);
	fsys_fread(&tempFmtChunk.wfcSubchunk1Size, 4, 1, tempFile);
	fsys_fread(&tempFmtChunk.wfcAudioFormat, 2, 1, tempFile);
	fsys_fread(&tempFmtChunk.wfcNumChannels, 2, 1, tempFile);
	fsys_fread(&tempFmtChunk.wfcSampleRate, 4, 1, tempFile);
	fsys_fread(&tempFmtChunk.wfcByteRate, 4, 1, tempFile);
	fsys_fread(&tempFmtChunk.wfcBlockAlign, 2, 1, tempFile);
	fsys_fread(&tempFmtChunk.wfcBitsPerSample, 2, 1, tempFile);

	if(_strncmp(tempFmtChunk.wfcSubchunk1ID, "fmt ", 4) != 0) {
		wav_error("No fmt chunk detected.", 6);
		goto wav_load_end;
	}
	if(tempFmtChunk.wfcAudioFormat != 1) {
		wav_error("Incompatible audio format.", 7);
		goto wav_load_end;
	}
	fsys_fseek(tempFile, (tempFmtChunk.wfcSubchunk1Size - 16), FSYS_SEEK_CUR);

	wavDataChunk tempDataChunk;
	fsys_fread(tempDataChunk.wdcSubchunk2ID, 1, 4, tempFile);
	fsys_fread(&tempDataChunk.wdcSubchunk2Size, 4, 1, tempFile);

	if(_strncmp(tempDataChunk.wdcSubchunk2ID, "fact", 4) == 0) {
		fsys_fseek(tempFile, tempDataChunk.wdcSubchunk2Size, FSYS_SEEK_CUR);
		fsys_fread(tempDataChunk.wdcSubchunk2ID, 1, 4, tempFile);
		fsys_fread(&tempDataChunk.wdcSubchunk2Size, 4, 1, tempFile);
	}

	if(_strncmp(tempDataChunk.wdcSubchunk2ID, "data", 4) != 0) {
		wav_error("No data chunk detected.", 8);
		goto wav_load_end;
	}
	if(tempLen < ((tempFmtChunk.wfcSubchunk1Size + 8) + (tempDataChunk.wdcSubchunk2Size + 8) + 12)) {
		wav_error("Incorrect data chunk length.", 9);
		goto wav_load_end;
	}
	if(tempFmtChunk.wfcBlockAlign == 0) {
		wav_error("Invalid block align value in format chunk.", 10);
		goto wav_load_end;
	}

	uint32_t tempSampleCount = (tempDataChunk.wdcSubchunk2Size / tempFmtChunk.wfcBlockAlign);
	uint32_t tempSampleSize = ((tempFmtChunk.wfcBitsPerSample * tempFmtChunk.wfcNumChannels) >> 3);
	uint32_t tempSampleSkip = tempFmtChunk.wfcBlockAlign - tempSampleSize;

	tempOut = (sound_t*)malloc(sizeof(sound_t) + (tempSampleCount * tempSampleSize));
	if(tempOut == NULL) {
		wav_error("Unable to allocate wav buffer.", 11);
		goto wav_load_end;
	}
	tempOut->data         = (int16_t*)((uintptr_t)tempOut + sizeof(sound_t));
	tempOut->sample_count = tempSampleCount;
	tempOut->sample_rate  = tempFmtChunk.wfcSampleRate;
	tempOut->sample_bits  = tempFmtChunk.wfcBitsPerSample;
	tempOut->channels     = tempFmtChunk.wfcNumChannels;

	uint32_t i;
	uint8_t* tempSamplePtr;
	if(tempSampleSkip > 0) {
		tempSamplePtr = (uint8_t*)tempOut->data;
		for(i = 0; i < tempSampleCount; i++, tempSamplePtr += tempSampleSize) {
			fsys_fread(tempSamplePtr, tempSampleSize, 1, tempFile);
			fsys_fseek(tempFile, tempSampleSkip, FSYS_SEEK_CUR);
		}
	} else {
		fsys_fread(tempOut->data, tempSampleSize, tempSampleCount, tempFile);
	}

wav_load_end:
	fsys_fclose(tempFile);
	return tempOut;
}

void wav_delete(sound_t* inBuffer) {
	free(inBuffer);
}
