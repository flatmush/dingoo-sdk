#ifndef __string_h__
#define __string_h__

#include <stddef.h>

#define strlen(inStr) _strlen(inStr)
extern int   _strlen(const char* inStr);

extern char* strchr(char* inStr, int inChar);
extern char* strstr(char* inStr, char* inFind);

extern char* strcpy(char* outDest, const char* inSrc);
extern char* strncpy(char* outDest, const char* inSrc, size_t inLength);

extern int   _strcmp(const char* inStr0, const char* inStr1);
extern int   strncmp(const char* inStr0, const char* inStr1, size_t inLength);

extern char* strcat(char* outDest, const char* inSrc);

extern void* memchr (void* inPtr, int inChar, size_t inLength);
extern void* _memcpy(void* outDest, const void* inSrc, size_t inLength);
extern void* memmove(void* outDest, const void* inSrc, size_t inLength);
extern void* _memset(void* outDest, int inValue, size_t inLength);
extern int   memcmp (const void *s1, const void *s2, size_t n);

#define strcmp(inStr0, inStr1) _strcmp(inStr0, inStr1)
#define memcpy(outDest, inSrc, inLength) _memcpy(outDest, inSrc, inLength)
#define memset(outDest, inValue, inLength) _memset(outDest, inValue, inLength)

#endif
