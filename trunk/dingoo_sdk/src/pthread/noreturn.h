#ifndef __noreturn_h__
#define __noreturn_h__

#if defined(__BORLANDC__) || (_MSC_VER >= 1310)
# define noreturn _declspec(noreturn)
#elif (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 5))
# define noreturn __attribute__ ((noreturn))
/*#elif (__cplusplus >= ???)
# define noreturn [[noreturn]]
#elif (__STDC_VERSION__ >= 201ymmL)
# define noreturn _Noreturn*/
#else
#define noreturn
#endif

#endif
