#ifndef __alloca_h__
#define __alloca_h__

#ifdef __cplusplus
extern "C"
{
#endif

#define alloca(size) __builtin_alloca (size)

#ifdef __cplusplus
}
#endif

#endif
