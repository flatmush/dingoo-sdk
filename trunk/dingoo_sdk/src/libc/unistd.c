#include "unistd.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


typedef struct {
    FILE* stream;
    int   flags;
} _fd_t;

extern _fd_t* _fd_list;
extern int    _fd_count;



int close(int fildes) {
    if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
        errno = EINVAL;
        return -1;
    }
    fclose(_fd_list[fildes].stream);
    _fd_list[fildes].stream = NULL;
    return 0;
}



off_t lseek(int fildes, off_t offset, int whence) {
    if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
        errno = EINVAL;
        return -1;
    }
    if(fseek(_fd_list[fildes].stream, offset, whence) != 0)
        return (off_t)-1;
    return (off_t)ftell(_fd_list[fildes].stream); // TODO - Change to ftello when complete.
}



ssize_t read(int fildes, void* buf, size_t nbyte) {
    int tempRead = 0;
    if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
        errno = EINVAL;
        return -1;
    }
    if (nbyte == 0)
        return 0;
    tempRead = fread(buf, 1, nbyte, _fd_list[fildes].stream);
    return tempRead;
}

ssize_t write(int fildes, const void* buf, size_t nbyte) {
    if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
        errno = EINVAL;
        return -1;
    }
    int tempWrite = fwrite(buf, 1, nbyte, _fd_list[fildes].stream);
    if(tempWrite == 0)
        return -1;
    return tempWrite;
}

extern char* _app_path;

char *getcwd(char *buf, size_t size) {
    if (size == 0) {
        errno = EINVAL;
        return NULL;
    }

    char* lastMatch = strrchr(_app_path, '\\');
    if (lastMatch == NULL) {
        errno = EACCES; // For lack of a better one...
        return NULL;
    }

    int requiredSize = lastMatch - _app_path + 1;

    // The reason we're not using + 1 for \0 here is because _app_path contains a trailing \ that we don't want
    // We only want the trailing \ when at root (A:\, B:\)
    if (requiredSize == 3)
        requiredSize = 4; // include \ for root

    if (size < requiredSize) {
        errno = ERANGE;
        return NULL;
    }

    strncpy(buf, _app_path, requiredSize);
    buf[requiredSize - 1] = '\0';

    return buf;
}

/*
** chdir()
**
** Upon successful completion, a value of 0 is returned.  Otherwise, a value
** of -1 is returned
*/
int chdir(const char *path)
{
    /*
    ** NOTE this needs to be called after _app_path_init()
    ** first time this routine is called new memory will be allocated
    ** GameMain() is currently responsible for free'ing it.
    ** 
    ** if chdir() is called, it is likely to be called again, and again, .....
    ** allocate max path len so we don't waste time realloc()'ing and
    ** avoid memory fragmentation. For simplicity simply use a
    ** "called more than once" flag rather than use additional buffers
    */
    
    char * res_str=NULL;
    char abs_path_str[FILENAME_MAX];
    int tmp_len=0;
    struct stat st;
    
    /* first make a special handling for ".." */
    if (strcmp(path, "..") == 0) {
        strncpy(abs_path_str, _app_path, FILENAME_MAX-1);
        abs_path_str[FILENAME_MAX-1] = 0;
    tmp_len = strlen(abs_path_str)-2;
    for (; tmp_len >= 2; --tmp_len) {
        if (abs_path_str[tmp_len] == '\\') {
            abs_path_str[tmp_len+1] = 0;
                goto chdir_check_stat;
        }
    }
        /* change A:\ to b:\ and vice versa*/
        if (abs_path_str[0] == 'a') {
             abs_path_str[0] = 'b';
        } else {
             abs_path_str[0] = 'a';
        }
        goto chdir_check_stat;
    }
    
    /* attempt to sanitize the path */
    res_str = realpath(path, abs_path_str);
    if (res_str == NULL)
    {
        return -1;
        errno = EIO;
    }
    
    /*  ensure there is a (single) trailing slash on the path */
    tmp_len = strlen(abs_path_str);
    res_str = abs_path_str + (tmp_len-1); /* last char in string */
    
    /* remove slashes \ */
    while (res_str > abs_path_str && *res_str == '\\')
        res_str--;
    
    if (*res_str != '\\')
    {
        res_str++;
        *res_str = '\\';
        res_str++;
        *res_str = '\0';
    }
    
chdir_check_stat:
    /*
    ** make sure sanitized path is a directory
    ** NOTE stat() makes calls to _file_path() (which may use _app_path)
    */
    stat(abs_path_str, &st);
    if (S_ISDIR(st.st_mode))
    {
        strncpy(_app_path, abs_path_str, sizeof(abs_path_str));
        return 0;
    }
    else
    {
        errno = ENOTDIR;
        return -1;
    }
}

int isatty(int fildes)
{
    if((fildes < 0) || (fildes >= _fd_count) || (_fd_list[fildes].stream == NULL)) {
        errno = EBADF;
    }
    else
    {
        errno = ENOTTY;
    }
    return 0;
}
