#ifndef __ctype_h__
#define __ctype_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <ctype.h>

extern int iscntrl(int chr);
extern int isspace(int chr);
extern int isupper(int chr);
extern int islower(int chr);
extern int isalpha(int chr);
extern int isbdigit(int chr); // Non-Standard
extern int isdigit(int chr);
extern int isodigit(int chr); // Non-Standard
extern int isxdigit(int chr);
extern int isalnum(int chr);
extern int ispunct(int chr);
extern int isgraph(int chr);
extern int isprint(int chr);

extern int toupper(int chr);
extern int tolower(int chr);

extern int isascii(int chr);
extern int toascii(int chr);

#ifdef __cplusplus
}
#endif

#endif
