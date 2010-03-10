#ifndef __stdbool_h__
#define __stdbool_h__

typedef int bool;

#ifndef false
#define false 0
#endif

#ifndef true
#define true !false
#endif

#define __bool_true_false_are_defined (defined(true) && defined(false))

// Non-Standard
typedef bool bool_t;
#define TRUE true
#define FALSE false

#endif
