#ifndef __fsys_h__
#define __fsys_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <dingoo/entry.h>

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
extern int        fsys_flush_cache(FSYS_FILE*);
extern void       fsys_clearerr(FSYS_FILE*);

extern int        fsys_removeW(const wchar_t*);
#define fsys_remove(inFile) fsys_removeW(__to_unicode_le(inFile))

extern int        fsys_mkdir(const char*);


#define FSYS_FILENAME_MAX 544

typedef struct {
	unsigned int  handle;
	unsigned int  size;
	unsigned int  attributes;
	unsigned int  time;
	short         padding;
	char          name[FSYS_FILENAME_MAX];
    /*
    ** The whole filename field is not the actual filename. There seem to be
    ** parts of the filename in unicode(?) or something as well. But the
    ** extra data is always separated by 0, so string operations on filename
    ** works without problems.
    ** 
    ** 
    ** #define IsDir(fd) (fd.attributes&0x10)
    ** #define IsDir(fd) (fd.attributes & FSYS_FIND_DIRECTORY)
    ** #define IsFile(fd) (!IsDir(fd))
    */
} __attribute__((__packed__)) fsys_file_info_t;

#define FSYS_FIND_FILE      0x00
#define FSYS_FIND_DIRECTORY 0x10
#define FSYS_FIND_DIR FSYS_FIND_DIRECTORY

/* fsys_file_info_t.attributes masks; based on observation on contents of my SD card in Dingo A320 */
#define FSYS_ATTR_DISKLABEL 0x08
#define FSYS_ATTR_DIR       0x10
#define FSYS_ATTR_FILE      0x20

/* fstat/stat like macros for use with find tools */
#define FSYS_ISDIR(mode) (((mode) & FSYS_ATTR_DIR) == FSYS_ATTR_DIR)
#define FSYS_ISFILE(mode) (((mode) & FSYS_ATTR_FILE) == FSYS_ATTR_FILE)


/*
**  fsys_findfirst(
**                  path and search pattern, e.g. "a:\\*", "a:\\*.mp3"
**                  mask or some kind? FSYS_FIND_FILE finds files only, FSYS_FIND_DIRECTORY finds dirs only, -1 (for all, including disk/volume labels), 
**                  result struct
**                )
**  returns 0 on success and -1 on failure
*/
extern int        fsys_findfirst(const char*, int, fsys_file_info_t*);
extern int        fsys_findnext(fsys_file_info_t*);
extern int        fsys_findclose(fsys_file_info_t*);

#ifdef __cplusplus
}
#endif

#endif
