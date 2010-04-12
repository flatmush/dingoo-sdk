#ifndef __stddef_h__
#define __stddef_h__

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef intptr_t ptrdiff_t;
typedef uintptr_t size_t;
typedef uint16_t wchar_t;

#endif
