#ifndef __stddef_h__
#define __stddef_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#define offsetof(st, m) \
    ((size_t) ( (char *)&((st *)(0))->m - (char *)0 ))

typedef intptr_t ptrdiff_t;
typedef uintptr_t size_t;
#ifndef __cplusplus
typedef uint16_t wchar_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
