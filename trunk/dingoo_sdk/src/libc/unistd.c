#include "unistd.h"
#include <stdlib.h>
#include <stdio.h>



extern FILE** _fd_list;
extern int    _fd_count;



int close(int fildes) {
	if((fildes < 0) || (fildes >= _fd_count))
		return -1;
	if(_fd_list[fildes] == NULL)
		return -1;
	fclose(_fd_list[fildes]);
	_fd_list[fildes] = NULL;
	return 0;
}



off_t lseek(int fildes, off_t offset, int whence) {
	if((fildes < 0) || (fildes >= _fd_count))
		return (off_t)-1;
	if(_fd_list[fildes] == NULL)
		return (off_t)-1;
	if(fseek(_fd_list[fildes], offset, whence) != 0)
		return (off_t)-1;
	return (off_t)ftell(_fd_list[fildes]); // TODO - Change to ftello when complete.
}



ssize_t read(int fildes, void* buf, size_t nbyte) {
	if((fildes < 0) || (fildes >= _fd_count))
		return (ssize_t)-1;
	if(_fd_list[fildes] == NULL)
		return (ssize_t)-1;
	int tempRead = fread(buf, 1, nbyte, _fd_list[fildes]);
	if(tempRead == 0)
		return -1;
	return tempRead;
}

ssize_t write(int fildes, const void* buf, size_t nbyte) {
	if((fildes < 0) || (fildes >= _fd_count))
		return (ssize_t)-1;
	if(_fd_list[fildes] == NULL)
		return (ssize_t)-1;
	int tempWrite = fwrite(buf, 1, nbyte, _fd_list[fildes]);
	if(tempWrite == 0)
		return -1;
	return tempWrite;
}
