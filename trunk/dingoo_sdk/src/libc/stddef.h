#ifndef __stddef_h__
#define __stddef_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define offsetof(st, m) \
    ((size_t) ( (char *)&((st *)(0))->m - (char *)0 ))

typedef intptr_t ptrdiff_t;
typedef uintptr_t size_t;
typedef uint16_t wchar_t;

#ifdef __cplusplus
}
#endif

#endif
