#ifndef __stat_h__
#define __stat_h__

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned int mode_t;

int mkdir(const char *path, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
