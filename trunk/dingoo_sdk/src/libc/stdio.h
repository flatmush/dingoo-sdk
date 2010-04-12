#ifndef __stdio_h__
#define __stdio_h__

#include <stddef.h>
#include <dingoo/fsys.h>

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
extern int    sprintf(char*, const char*, ...);

extern size_t fread(void*, size_t, size_t, FILE*);
extern size_t fwrite(const void*, size_t, size_t, FILE*);
extern int    fseek(FILE*, long int, int);

extern void   perror(const char* prefix);

//#define printf(...) fprintf(stdout, __VA_ARGS__)
//#define eprintf(...) fprintf(stderr, __VA_ARGS__)

extern FILE* _fopen(const char* filename, const char* mode);
extern void  _fclose(FILE* stream);

extern int      _fseek(FILE* stream, long int offset, int origin);
extern long int _ftell(FILE* stream);
extern int      _ferror(FILE* stream);
extern int      _feof(FILE* stream);
extern int      _fread(void* ptr, size_t size, size_t count, FILE* stream);
extern int      _fwrite(const void* ptr, size_t size, size_t count, FILE* stream);

#define fopen _fopen
#define fclose _fclose
#define fseek _fseek
#define ftell _ftell
#define ferror _ferror
#define feof _feof
#define fread _fread
#define fwrite _fwrite

extern FILE* fmemopen(void* buf, size_t size, const char* mode);

// TODO - Make a function for remove that parses paths correctly.
#define remove(filename) fsys_remove(filename)

int fgetc(FILE* stream);

#define getc fgetc

#endif
