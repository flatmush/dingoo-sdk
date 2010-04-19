#include "fcntl.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>



typedef struct {
	FILE* stream;
	int   flags;
} _fd_t;

_fd_t* _fd_list  = NULL;
int    _fd_count = 0;



int open(const char* path, int oflag, ... ) {
	if(oflag & _O_UNSUPPORTED) {
		errno = EINVAL;
		return -1;
	}
	// TODO - Deal more correctly with certain flags.

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
		if(_fd_list[i].stream == NULL) {
			_fd_list[i].stream = tempFile;
			_fd_list[i].flags  = oflag;
			return i;
		}
	}

	_fd_t* tempFdList;
	if(_fd_count == 0)
		tempFdList = (_fd_t*)malloc((8 * sizeof(_fd_t)));
	else
		tempFdList = (_fd_t*)realloc(_fd_list, ((_fd_count << 1) * sizeof(_fd_t)));

	if(tempFdList == NULL) {
		fclose(tempFile);
		return -1;
	}

	for(i = _fd_count; i < (_fd_count == 0 ? 8 : (_fd_count << 1)); i++)
		_fd_list[i].stream = NULL;

	_fd_list = tempFdList;
	int tempFD = _fd_count;

	_fd_list[tempFD].stream = tempFile;
	_fd_list[tempFD].flags  = oflag;

	_fd_count = (_fd_count == 0 ? 8 : (_fd_count << 1));
	return tempFD;
}

int fcntl(int fildes, int cmd, ...) {
	if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
		errno = EINVAL;
		return -1;
	}
	if((cmd <= 0) || (cmd > _F_LAST)) {
		errno = EINVAL;
		return -1;
	}

	va_list ap;

	int   arg;
	void* flock;
	switch(cmd) {
		case F_SETFD:
		case F_SETOWN:
			va_start(ap, cmd);
			arg = va_arg(ap, int);
			va_end(ap);
			break;
		case F_GETLK:
		case F_SETLK:
			va_start(ap, cmd);
			flock = va_arg(ap, void*);
			va_end(ap);
			break;
	}

	switch(cmd) {
		case F_DUPFD: // Duplicate file descriptors not supported.
			errno = EINVAL;
			return -1;
		case F_GETFD:
			return (_fd_list[fildes].flags & _F_FILE_DESC);
		case F_SETFD:
			arg &= _F_FILE_DESC;
			_fd_list[fildes].flags &= ~_F_FILE_DESC;
			_fd_list[fildes].flags |= arg;
			return 0;
		case F_GETFL:
			return (_fd_list[fildes].flags & (O_ACCMODE | _O_FILE_STATUS));
		case F_SETFL:
			arg &= (O_ACCMODE | _O_FILE_STATUS);
			if(arg & _O_UNSUPPORTED) {
				errno = EINVAL;
				return -1;
			}
			if((arg & O_ACCMODE) != (_fd_list[fildes].flags & O_ACCMODE)) {
				errno = EINVAL;
				return -1;
			}
			_fd_list[fildes].flags &= ~(O_ACCMODE | _O_FILE_STATUS);
			_fd_list[fildes].flags |= arg;
			return 0;
		case F_GETOWN:
		case F_SETOWN:
			errno = -1;
			return -1;
		case F_GETLK:
		case F_SETLK:
		case F_SETLKW:
			return -1;
	}

	errno = EINVAL;
	return -1;
}
