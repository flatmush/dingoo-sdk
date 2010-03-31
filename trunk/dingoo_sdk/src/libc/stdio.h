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

#define fopen(filename, mode) _fopen((filename), (mode))
#define fclose(stream) _fclose(stream)
#define fseek(stream, offset, origin) _fseek((stream), (offset), (origin))
#define ftell(stream) _ftell(stream)
#define ferror(stream) _ferror(stream)
#define feof(stream) _feof(stream)
#define fread(ptr, size, count, stream) _fread((ptr), (size), (count), (stream))
#define fwrite(ptr, size, count, stream) _fwrite((ptr), (size), (count), (stream))

// TODO - Make a function for remove that parses paths correctly.
#define remove(filename) fsys_remove(filename)

#endif
