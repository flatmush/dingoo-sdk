#ifndef __stdint_h__
#define __stdint_h__


#define INT64_MIN (int64_t)0x8000000000000000ULL
#define INT64_MAX (int64_t)0x7FFFFFFFFFFFFFFFULL
#define INT64_C(value) ((int64_t)value ## LL)
typedef signed long long int int64_t;
#define UINT64_MAX (uint64_t)0xFFFFFFFFFFFFFFFFULL
#define UINT64_C(value) ((uint64_t)value ## ULL)
typedef unsigned long long int uint64_t;

#define INT32_MIN (int32_t)0x80000000
#define INT32_MAX (int32_t)0x7FFFFFFF
#define INT32_C(value) ((int32_t)value ## L)
typedef signed int int32_t;
#define UINT32_MAX (uint32_t)0xFFFFFFFF
#define UINT32_C(value) ((uint32_t)value ## UL)
typedef unsigned int uint32_t;

#define INT16_MIN (int16_t)0x8000
#define INT16_MAX (int16_t)0x7FFF
#define INT16_C(value) ((int16_t)value)
typedef signed short int int16_t;
#define UINT16_MAX (uint16_t)0xFFFF
#define UINT16_C(value) ((int16_t)value ## U)
typedef unsigned short int uint16_t;

#define INT8_MIN (int8_t)0x80
#define INT8_MAX (int8_t)0x7F
#define INT8_C(value) ((int8_t)value)
typedef signed char int8_t;
#define UINT8_MAX (uint8_t)0xFF
#define UINT8_C(value) ((int8_t)value ## U)
typedef unsigned char uint8_t;



#define INT_LEAST64_MIN INT64_MIN
#define INT_LEAST64_MAX INT64_MAX
#define INT_LEAST64_C(value) INT64_C(value)
typedef int64_t int_least64_t;
#define UINT_LEAST64_MAX UINT64_MAX
#define UINT_LEAST64_C(value) UINT64_C(value)
typedef uint64_t uint_least64_t;

#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST32_C(value) INT32_C(value)
typedef int32_t int_least32_t;
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST32_C(value) UINT32_C(value)
typedef uint32_t uint_least32_t;

#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST16_C(value) INT16_C(value)
typedef int16_t int_least16_t;
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST16_C(value) UINT16_C(value)
typedef uint16_t uint_least16_t;

#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST8_C(value) INT8_C(value)
typedef int8_t int_least8_t;
#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST8_C(value) UINT8_C(value)
typedef uint8_t uint_least8_t;



#define INT_FAST64_MIN INT64_MIN
#define INT_FAST64_MAX INT64_MAX
#define INT_FAST64_C(value) INT64_C(value)
typedef int64_t int_fast64_t;
#define UINT_FAST64_MAX UINT64_MAX
#define UINT_FAST64_C(value) UINT64_C(value)
typedef uint64_t uint_fast64_t;

#define INT_FAST32_MIN INT32_MIN
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST32_C(value) INT32_C(value)
typedef signed int int_fast32_t;
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST32_C(value) UINT32_C(value)
typedef unsigned int uint_fast32_t;

#define INT_FAST16_MIN INT32_MIN
#define INT_FAST16_MAX INT32_MAX
#define INT_FAST16_C(value) INT32_C(value)
typedef signed int int_fast16_t;
#define UINT_FAST16_MAX UINT32_MAX
#define UINT_FAST16_C(value) UINT32_C(value)
typedef unsigned int uint_fast16_t;

#define INT_FAST8_MIN INT32_MIN
#define INT_FAST8_MAX INT32_MAX
#define INT_FAST8_C(value) INT32_C(value)
typedef signed int int_fast8_t;
#define UINT_FAST8_MAX UINT32_MAX
#define UINT_FAST8_C(value) UINT32_C(value)
typedef unsigned int uint_fast8_t;



#define INTPTR_MIN INT32_MIN
#define INTPTR_MAX INT32_MAX
#define INTPTR_C(value) INT32_C(value)
typedef signed int intptr_t;
#define UINTPTR_MAX UINT32_MAX
#define UINTPTR_C(value) UINT32_C(value)
typedef unsigned int uintptr_t;



#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define INTMAX_C(value) INT64_C(value)
typedef int64_t intmax_t;
#define UINTMAX_MAX UINT64_MAX
#define UINTMAX_C(value) UINT64_C(value)
typedef uint64_t uintmax_t;

// Non-Standard
#define INTMIN_MIN INT8_MIN
#define INTMIN_MAX INT8_MAX
#define INTIN_C(value) INT8_C(value)
typedef int8_t intmin_t;
#define UINTMIN_MAX UINT8_MAX
#define UINTIN_C(value) UINT8_C(value)
typedef uint8_t uintmin_t;



#define PTRDIFF_MAX INT32_MAX
#define PTRDIFF_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT8_MAX
#define SIG_ATOMIC_MIN INT8_MIN
#define SIZE_MAX INT32_MAX

#endif
