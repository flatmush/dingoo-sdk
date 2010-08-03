#ifndef __stl_config__dingoo_h
#define __stl_config__dingoo_h

#define _STLP_PLATFORM "Dingoo"

#include <features.h>

#define _STLP_NO_THREADS
#define _STLP_NO_NEW_C_HEADERS
#define _STLP_NO_WCHAR_T
#define _STLP_NO_NATIVE_MBSTATE_T
#define _STLP_NO_VENDOR_MATH_F
#define _STLP_USE_MALLOC 1
#define _STLP_USE_STDIO_IO
#define _STLP_LITTLE_ENDIAN 1
#define _STLP_NO_CSTD_FUNCTION_IMPORTS
#define _STLP_USE_DEFAULT_FILE_OFFSET

inline int puts(const char* str) { return 0; }

#endif /* __stl_config__dingoo_h */
