#ifndef __libc_dirent_h__
#define __libc_dirent_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>

struct dirent {
	ino_t d_ino;
	unsigned int d_type; /* NOTE using the same type that fsys uses for file type mask; unsigned int  attributes;*/
	char* d_name;
};

#define DT_DIR FSYS_ATTR_DIR
#define DT_REG FSYS_ATTR_FILE /* Not sure if this is a suitable mapping */

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
