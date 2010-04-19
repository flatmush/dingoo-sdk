#include "fcntl.h"
#include <stdlib.h>
#include <stdio.h>



FILE** _fd_list  = NULL;
int    _fd_count = 0;



int open(const char* path, int oflag, ... ) {
	// TODO - Deal more correctly with certain flags, especially trunc and creat.

	FILE* tempFile = fopen(path, "rb");
	if(oflag & O_CREAT) {
		if(tempFile == NULL)
			tempFile = fopen(path, "wb");
		if(tempFile == NULL)
			return -1;
	} else if(tempFile == NULL) {
		return -1;
	} else if(oflag & O_TRUNC) {
		tempFile = fopen(path, "wb");
		if(tempFile == NULL)
			return -1;
	}
	fclose(tempFile);

	char tempMode[16];
	if((oflag & 0x3) == O_RDONLY) {
		sprintf(tempMode, "rb");
	} else if((oflag & 0x3) == O_WRONLY) {
		if(oflag & O_APPEND)
			sprintf(tempMode, "ab");
		else
			sprintf(tempMode, "wb");
	} else if((oflag & 0x3) == O_RDWR) {
		if(oflag & O_APPEND)
			sprintf(tempMode, "ab+");
		else
			sprintf(tempMode, "rb+");
	}

	tempFile = fopen(path, tempMode);
	if(tempFile == NULL)
		return -1;

	int i;
	for(i = 0; i < _fd_count; i++) {
		if(_fd_list[i] == NULL) {
			_fd_list[i] = tempFile;
			return i;
		}
	}

	FILE** tempFdList;
	if(_fd_count == 0)
		tempFdList = (FILE**)malloc((8 * sizeof(FILE*)));
	else
		tempFdList = (FILE**)realloc(_fd_list, ((_fd_count << 1) * sizeof(FILE*)));

	if(tempFdList == NULL) {
		fclose(tempFile);
		return -1;
	}

	for(i = _fd_count; i < (_fd_count == 0 ? 8 : (_fd_count << 1)); i++)
		_fd_list[i] = NULL;

	_fd_list = tempFdList;
	int tempFD = _fd_count;
	_fd_list[tempFD] = tempFile;
	_fd_count = (_fd_count == 0 ? 8 : (_fd_count << 1));
	return tempFD;
}
