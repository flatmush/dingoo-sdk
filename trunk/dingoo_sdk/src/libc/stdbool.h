#ifndef __stdbool_h__
#define __stdbool_h__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __cplusplus

#define bool _Bool
#define true 1
#define false 0

#endif//__cplusplus

#define __bool_true_false_are_defined (defined(true) && defined(false))

// Non-Standard
typedef bool bool_t;
#define TRUE true
#define FALSE false

#ifdef __cplusplus
}
#endif

#endif
