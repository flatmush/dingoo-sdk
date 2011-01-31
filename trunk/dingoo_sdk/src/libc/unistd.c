#include "unistd.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


typedef struct {
	FILE* stream;
	int   flags;
} _fd_t;

extern _fd_t* _fd_list;
extern int    _fd_count;



int close(int fildes) {
	if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
		errno = EINVAL;
		return -1;
	}
	fclose(_fd_list[fildes].stream);
	_fd_list[fildes].stream = NULL;
	return 0;
}



off_t lseek(int fildes, off_t offset, int whence) {
	if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
		errno = EINVAL;
		return -1;
	}
	if(fseek(_fd_list[fildes].stream, offset, whence) != 0)
		return (off_t)-1;
	return (off_t)ftell(_fd_list[fildes].stream); // TODO - Change to ftello when complete.
}



ssize_t read(int fildes, void* buf, size_t nbyte) {
	int tempRead = 0;
	if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
		errno = EINVAL;
		return -1;
	}
    if (nbyte == 0)
        return 0;
	tempRead = fread(buf, 1, nbyte, _fd_list[fildes].stream);
	return tempRead;
}

ssize_t write(int fildes, const void* buf, size_t nbyte) {
	if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
		errno = EINVAL;
		return -1;
	}
	int tempWrite = fwrite(buf, 1, nbyte, _fd_list[fildes].stream);
	if(tempWrite == 0)
		return -1;
	return tempWrite;
}
