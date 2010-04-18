#ifndef __types_h__
#define __types_h__

#include <stdbool.h>
#include <stdint.h>

#define	LITTLE_ENDIAN 1234
#define	BIG_ENDIAN 4321
#define	BYTE_ORDER LITTLE_ENDIAN

typedef int32_t off_t;
typedef int32_t ino_t;

typedef intptr_t ssize_t;

#endif
