#ifndef __fsys_h__
#define __fsys_h__

typedef void FSYS_FILE;

#define FSYS_SEEK_SET 0
#define FSYS_SEEK_CUR 1
#define FSYS_SEEK_END 2

extern FSYS_FILE* fsys_fopen(const char*, const char*);
extern int        fsys_fread(void*, size_t, size_t, FSYS_FILE*);
extern int        fsys_fclose(FSYS_FILE*);
extern int        fsys_fseek(FSYS_FILE*, long int, int);
extern long int   fsys_ftell(FSYS_FILE*);
extern int        fsys_ferror(FSYS_FILE*);
extern int        fsys_feof(FSYS_FILE*);
extern int        fsys_fwrite(const void*, size_t, size_t, FSYS_FILE*);

extern int        fsys_remove(const char*);

#endif
