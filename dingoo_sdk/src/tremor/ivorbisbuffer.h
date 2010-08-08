/*
Extension of tremor, supports ogg files from buffer.
Thanks to http://www.devmaster.net/articles/openal-ogg-file/
*/
#ifndef _OV_BUF_H_
#define _OV_BUF_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <string.h>
#include "ivorbisfile.h"

typedef struct
{
	char*		dataPtr;			// Pointer to the data in memoru
	int			dataSize;			// Sizeo fo the data
	int			dataRead;			// How much data we have read so far
} SOggFile;

SOggFile        oggMemoryFile;		// Data on the ogg file in memory
ov_callbacks    vorbisCallbacks;

int ov_open_buffer(OggVorbis_File *vf, char* buffer, int length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
