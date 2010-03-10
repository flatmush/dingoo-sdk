#ifndef __malloc_h__
#define __malloc_h__

#include <stddef.h>

extern void* malloc(size_t size);
extern void* realloc(void* ptr, size_t size);
extern void  free(void* ptr);

#define malloc(size)       _malloc(size)
#define realloc(ptr, size) _realloc(ptr, size)
#define free(ptr)          _free(ptr)

extern void* _malloc(size_t size);
extern void* _realloc(void* ptr, size_t size);
extern void  _free(void* ptr);

extern void* calloc(size_t num, size_t size);
extern void* memalign(size_t align, size_t size);
extern void* valloc(size_t size);

#endif
