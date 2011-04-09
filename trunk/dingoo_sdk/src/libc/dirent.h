#ifndef __libc_dirent_h__
#define __libc_dirent_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h> /* for FILENAME_MAX */
#include <sys/types.h>

struct dirent {
	ino_t d_ino;
	unsigned char d_type;
	char d_name[FILENAME_MAX];
};

enum { DT_UNKNOWN = 0, DT_DIR = 4, DT_REG = 8 };
#define DT_UNKNOWN DT_UNKNOWN
#define DT_DIR DT_DIR
#define DT_REG DT_REG

typedef void DIR;

extern DIR* opendir(const char *name);
extern int  closedir(DIR* dir);

extern struct dirent* readdir(DIR* dir);

/*
** rewinddir(), seekdir(), telldir() have not been tested
** rewinddir() almost certainly does not work
** Code commented out to ensure potential problems are
** detected at build time rather than runtime
*/

#ifdef DINGOO_BUILD_COMMENTED_OUT_CODE
extern void rewinddir(DIR* dir);
extern void seekdir(DIR* dir, long offset);
extern long telldir(DIR* dir);
#endif /* DINGOO_BUILD_COMMENTED_OUT_CODE */


#ifdef __cplusplus
}
#endif

#endif
