#ifndef __libc_fcntl_h__
#define __libc_fcntl_h__

#define O_RDONLY   (1 << 0)
#define O_WRONLY   (2 << 0)
#define O_RDWR     (3 << 0)
#define O_APPEND   (1 << 2)
#define O_CREAT    (1 << 3)
#define O_DSYNC    (1 << 4)
#define O_EXCL     (1 << 5)
#define O_NOCTTY   (1 << 6)
#define O_NONBLOCK (1 << 7)
#define O_RSYNC    (1 << 8)
#define O_SYNC     (1 << 9)
#define O_TRUNC    (1 << 10)

#define O_CREATE O_CREAT

extern int open(const char* path, int oflag, ...);

#endif
