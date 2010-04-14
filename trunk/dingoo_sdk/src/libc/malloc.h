#ifndef __malloc_h__
#define __malloc_h__

#include <stddef.h>

#define malloc _malloc
#define realloc _realloc
#define free _free

extern void* _malloc(size_t size);
extern void* _realloc(void* ptr, size_t size);
extern void  _free(void* ptr);

extern void* calloc(size_t num, size_t size);
extern void* memalign(size_t align, size_t size);
extern void* valloc(size_t size);

#endif
