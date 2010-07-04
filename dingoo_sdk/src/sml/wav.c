#include <sml/wav.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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



void wav_error(const char* inError, const uint32_t inNumber) {
	// TODO - Report error somehow.
}

sound_t* _wav_load(FILE* tempFile) {
	if(tempFile == NULL) {
		wav_error("Unable to open file.", 2);
		return NULL;
	}

	fseek(tempFile, 0, SEEK_END);
	uint32_t tempLen = ftell(tempFile);
	fseek(tempFile, 0, SEEK_SET);

	sound_t* tempOut = NULL;

	wavRiffChunk tempRiffChunk;
	fread(tempRiffChunk.wrcChunkID, 1, 4, tempFile);
	fread(&tempRiffChunk.wrcChunkSize, 4, 1, tempFile);
	fread(tempRiffChunk.wrcFormat, 1, 4, tempFile);

	if(strncmp(tempRiffChunk.wrcChunkID, "RIFF", 4) != 0) {
		wav_error("Not RIFF file.", 3);
		goto wav_load_end;
	}
	if(strncmp(tempRiffChunk.wrcFormat, "WAVE", 4) != 0) {
		wav_error("Not wave file.", 4);
		goto wav_load_end;
	}
	if(tempRiffChunk.wrcChunkSize < (tempLen - 8)) {
		wav_error("Incorrect data length.", 5);
		goto wav_load_end;
	}

	wavFmtChunk tempFmtChunk;
	fread(tempFmtChunk.wfcSubchunk1ID, 1, 4, tempFile);
	fread(&tempFmtChunk.wfcSubchunk1Size, 4, 1, tempFile);
	fread(&tempFmtChunk.wfcAudioFormat, 2, 1, tempFile);
	fread(&tempFmtChunk.wfcNumChannels, 2, 1, tempFile);
	fread(&tempFmtChunk.wfcSampleRate, 4, 1, tempFile);
	fread(&tempFmtChunk.wfcByteRate, 4, 1, tempFile);
	fread(&tempFmtChunk.wfcBlockAlign, 2, 1, tempFile);
	fread(&tempFmtChunk.wfcBitsPerSample, 2, 1, tempFile);

	if(strncmp(tempFmtChunk.wfcSubchunk1ID, "fmt ", 4) != 0) {
		wav_error("No fmt chunk detected.", 6);
		goto wav_load_end;
	}
	if(tempFmtChunk.wfcAudioFormat != 1) {
		wav_error("Incompatible audio format.", 7);
		goto wav_load_end;
	}
	fseek(tempFile, (tempFmtChunk.wfcSubchunk1Size - 16), SEEK_CUR);

	wavDataChunk tempDataChunk;
	fread(tempDataChunk.wdcSubchunk2ID, 1, 4, tempFile);
	fread(&tempDataChunk.wdcSubchunk2Size, 4, 1, tempFile);

	if(strncmp(tempDataChunk.wdcSubchunk2ID, "fact", 4) == 0) {
		fseek(tempFile, tempDataChunk.wdcSubchunk2Size, SEEK_CUR);
		fread(tempDataChunk.wdcSubchunk2ID, 1, 4, tempFile);
		fread(&tempDataChunk.wdcSubchunk2Size, 4, 1, tempFile);
	}

	if(strncmp(tempDataChunk.wdcSubchunk2ID, "data", 4) != 0) {
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
	tempOut->sample_data  = (int16_t*)((uintptr_t)tempOut + sizeof(sound_t));
	tempOut->sample_count = tempSampleCount;
	tempOut->sample_rate  = tempFmtChunk.wfcSampleRate;
	tempOut->sample_bits  = tempFmtChunk.wfcBitsPerSample;
	tempOut->channels     = tempFmtChunk.wfcNumChannels;

	uint32_t i;
	uint8_t* tempSamplePtr;
	if(tempSampleSkip > 0) {
		tempSamplePtr = (uint8_t*)tempOut->sample_data;
		for(i = 0; i < tempSampleCount; i++, tempSamplePtr += tempSampleSize) {
			fread(tempSamplePtr, tempSampleSize, 1, tempFile);
			fseek(tempFile, tempSampleSkip, SEEK_CUR);
		}
	} else {
		fread(tempOut->sample_data, tempSampleSize, tempSampleCount, tempFile);
	}

wav_load_end:
	fclose(tempFile);
	return tempOut;
}

sound_t* wav_load(char* inFile) {
	if(inFile == NULL)
		return NULL;

	FILE* tempFile = fopen(inFile, "rb");
	return _wav_load(tempFile);
}

sound_t* wav_load_from_buffer(uint8_t* buffer, size_t size) {
	if(buffer == NULL)
		return NULL;

	FILE* tempFile = fmemopen(buffer, size, "rb");
	return _wav_load(tempFile);
}

void wav_delete(sound_t* inBuffer) {
	free(inBuffer);
}
