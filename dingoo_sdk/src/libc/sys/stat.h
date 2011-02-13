#ifndef __stat_h__
#define __stat_h__

#ifdef __cplusplus
extern "C"
{
#endif
    
#include <time.h>
#include <sys/types.h>

typedef unsigned int mode_t;

    
struct stat {
    mode_t    st_mode;    /* protection */
    off_t     st_size;    /* total size, in bytes */
    time_t    st_mtime;   /* time of last modification */
    int FAKE_STAT; /* this is not a copy/paste of a posix struct, this is bare min for some apps */
};

#define S_IFDIR     FSYS_ATTR_DIR
#define S_IFREG     FSYS_ATTR_FILE

#define S_ISREG(m) ((m) & S_IFREG)
#define S_ISDIR(m) ((m) & S_IFDIR)


int mkdir(const char *path, mode_t mode);
int stat(const char *path, struct stat *buf);

#ifdef __cplusplus
}
#endif

#endif
