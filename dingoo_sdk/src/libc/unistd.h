#ifndef __libc_unistd_h__
#define __libc_unistd_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <stdio.h>

extern int close(int fildes);

extern off_t lseek(int fildes, off_t offset, int whence);

extern ssize_t read(int fildes, void* buf, size_t nbyte);
extern ssize_t write(int fildes, const void* buf, size_t nbyte);

extern int chdir(const char *path);
extern char *getcwd(char *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif
