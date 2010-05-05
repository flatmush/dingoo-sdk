#ifndef __libc_dirent_h__
#define __libc_dirent_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>

struct dirent {
	ino_t d_ino;
	char* d_name;
};

typedef void DIR;

extern DIR* opendir(const char *name);
extern int  closedir(DIR* dir);

extern struct dirent* readdir(DIR* dir);

extern void rewinddir(DIR* dir);
extern void seekdir(DIR* dir, long offset);
extern long telldir(DIR* dir);

#ifdef __cplusplus
}
#endif

#endif
