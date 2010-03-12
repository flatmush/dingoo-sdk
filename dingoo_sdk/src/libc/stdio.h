#ifndef __stdio_h__
#define __stdio_h__

#include <stddef.h>
#include <dingoo/fsys.h>

typedef FSYS_FILE FILE;
typedef long int fpos_t;

#define SEEK_SET FSYS_SEEK_SET
#define SEEK_CUR FSYS_SEEK_CUR
#define SEEK_END FSYS_SEEK_END

#define FILENAME_MAX FSYS_FILENAME_MAX

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

//#define printf(...) fprintf(stdout, __VA_ARGS__)
//#define eprintf(...) fprintf(stderr, __VA_ARGS__)

extern FILE* _fopen(char* filename, char* mode);
#define fopen(filename, mode) _fopen(filename, mode)

extern void _fclose(FILE* stream);
#define fclose(stream) _fclose(stream)

#define remove(filename) fsys_remove(filename)

extern int    fprintf(FILE*, const char*, ...);
extern int    printf(const char*, ...);
extern int    sprintf(char*, const char*, ...);

extern size_t fread(void*, size_t, size_t, FILE*);
extern size_t fwrite(const void*, size_t, size_t, FILE*);
extern int    fseek(FILE*, long int, int);

extern void   perror(const char* prefix);

#endif
