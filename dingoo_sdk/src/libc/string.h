#ifndef __libc_string_h__
#define __libc_string_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

extern int   strlen(const char *inStr);

extern const char *strchr(const char *inStr, int inChar);
extern const char *strrchr(const char *inStr, int inChar);
extern const char *strstr(const char *inStr, const char *inFind);

extern char* strcpy(char *outDest, const char *inSrc);
extern char* strncpy(char *outDest, const char *inSrc, size_t inLength);
extern char* strdup(const char *inStr);

extern char *strtok(char *s, const char *delim);

extern int _strcmp(const char *inStr0, const char *inStr1);
extern int strncmp(const char *inStr0, const char *inStr1, size_t inLength);
extern int _stricmp(const char *s1, const char *s2);
extern int _strnicmp(const char *s1, const char *s2, size_t n);

extern size_t strspn(const char *s1, const char *s2);
extern size_t strcspn(const char *s1, const char *s2);

extern char *strpbrk(const char *s1, const char *s2);

extern char *strcat(char *outDest, const char *inSrc);
extern char *strncat(char *dst, const char *src, size_t n);

extern void *memchr(const void *inPtr, int inChar, size_t inLength);
extern void *memcpy(void *outDest, const void *inSrc, size_t inLength);
extern void *memmove(void *outDest, const void *inSrc, size_t inLength);
extern void *memset(void *outDest, int inValue, size_t inLength);
extern int  memcmp(const void *s1, const void *s2, size_t n);

#define strcmp(x, y) _strcmp(x, y)
#define stricmp(x, y) _stricmp(x, y)
#define strnicmp(x, y, z) _strnicmp(x, y, z)

#ifdef __cplusplus
}
#endif

#endif
