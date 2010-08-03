#ifndef __libc_stdio_h__
#define __libc_stdio_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <dingoo/fsys.h>
#include <stdarg.h>

typedef FSYS_FILE FILE;
typedef long int fpos_t;

#define EOF -1

#define SEEK_SET FSYS_SEEK_SET
#define SEEK_CUR FSYS_SEEK_CUR
#define SEEK_END FSYS_SEEK_END

#define FILENAME_MAX FSYS_FILENAME_MAX

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

extern int    fprintf(FILE*, const char*, ...);
extern int    printf(const char*, ...);
int    sprintf(char*, const char*, ...);

extern int vsnprintf (char *str, size_t count, const char *fmt, va_list args);
extern int snprintf(char *str,size_t count,const char *fmt,...);
extern int vasprintf(char **ptr, const char *format, va_list ap);
extern int asprintf(char **ptr, const char *format, ...);

extern void   perror(const char* prefix);

//#define printf(...) fprintf(stdout, __VA_ARGS__)
//#define eprintf(...) fprintf(stderr, __VA_ARGS__)

extern FILE* fopen(const char* filename, const char* mode);
extern int  fclose(FILE* stream);

extern int      fseek(FILE* stream, long int offset, int origin);
extern long int ftell(FILE* stream);
extern int      ferror(FILE* stream);
extern int      feof(FILE* stream);
extern int      fread(void* ptr, size_t size, size_t count, FILE* stream);
extern int      fwrite(const void* ptr, size_t size, size_t count, FILE* stream);

extern FILE* fmemopen(void* buf, size_t size, const char* mode);

// TODO - Make a function for remove that parses paths correctly.
extern int remove(const char* filename);

extern int   getc(FILE* stream);
extern int   getchar();
extern char* gets(char* s);

extern int   fgetc(FILE* stream);
extern int   fputc(int c, FILE* stream);

extern char* fgets(char* s, int n, FILE* stream);
extern int   fputs(const char* str, FILE* stream);

extern int ungetc(int _c, FILE *_stream);

#define getc fgetc
#define putc fputc
#define getchar() fgetc(stdin)

#ifdef __cplusplus
}
#endif

#endif
