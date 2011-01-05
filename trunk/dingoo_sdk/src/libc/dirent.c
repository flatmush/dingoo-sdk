#include "dirent.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dingoo/fsys.h>



typedef struct {
	fsys_file_info_t info;
	int              type;
	char*            path;
	bool             eod;
	struct dirent    cur_entry;
	bool             was_read;
	long             offset;
} _dir_t;


extern char* _app_path; /* from entry_point.c */
extern char* _file_path(const char* inPath); /* from stdio.c  */



DIR* opendir(const char* name) {
	char* tempPath = NULL;
	_dir_t* tempDir = NULL;
	int tmplen = 0;
	char *tmpstrptr=NULL;


	tempPath = _file_path(name);
	if(tempPath == NULL)
		return NULL;

	tempDir = (_dir_t*)malloc(sizeof(_dir_t));
	if(tempDir == NULL) {
		free(tempPath);
		return NULL;
	}

	tempDir->path   = tempPath;
	tempDir->eod    = false;
	tempDir->offset = 0;
	tempDir->type   = -1; /* All object types */

	char tempMask[strlen(tempDir->path) + 3];
    /* FIXME I'm not keen on stack allocations mid function this is C not C++! */
    
	/* Make sure there are no trailing slashes fsys_findfirst() won't find paths with multiple slashes */
	tmplen = strlen(tempMask);
	strcpy(tempMask, tempDir->path);
	tmplen = strlen(tempMask);
	tmpstrptr = &tempMask[tmplen-1];
	while (*tmpstrptr == '\\') *tmpstrptr-- = '\0';
	strcat(tempMask, "\\*");

    
	if(fsys_findfirst(tempMask, tempDir->type, &(tempDir->info)) != 0) {
		tempDir->eod = true;
	}

	if(!tempDir->eod) {
		/* TODO hide attributes & FSYS_ATTR_DISKLABEL ?*/
		tempDir->cur_entry.d_ino  = tempDir->info.handle;
		strcpy(tempDir->cur_entry.d_name, tempDir->info.name);
		tempDir->cur_entry.d_type = tempDir->info.attributes;
	}
	tempDir->was_read = false;

	return (DIR*)tempDir;
}

int closedir(DIR* dir) {
	_dir_t* tempDir = (_dir_t*)dir;

	if(dir == NULL)
		return -1;

	if(!tempDir->eod)
		fsys_findclose(&(tempDir->info));
	if(tempDir->path != NULL)
		free(tempDir->path);
	free(tempDir);

	return 0;
}



struct dirent* readdir(DIR* dir) {
	_dir_t* tempDir = (_dir_t*)dir;

	if(dir == NULL)
		return NULL;

	if(tempDir->eod)
		return NULL;

	if(!tempDir->was_read) {
		tempDir->was_read = true;
		return &(tempDir->cur_entry);
	}

	if(fsys_findnext(&(tempDir->info)) != 0) {
		tempDir->eod = true;
		return NULL;
	}

	tempDir->offset++;
	/* TODO hide attributes & FSYS_ATTR_DISKLABEL ?*/
	tempDir->cur_entry.d_ino  = tempDir->info.handle;
	strcpy(tempDir->cur_entry.d_name, tempDir->info.name);
	tempDir->cur_entry.d_type = tempDir->info.attributes;

	return &(tempDir->cur_entry);
}



/*
** rewinddir(), seekdir(), telldir() have not been tested
** rewinddir() almost certainly does not work
** Code commented out to ensure potential problems are
** detected at build time rather than runtime
*/

#ifdef DINGOO_BUILD_COMMENTED_OUT_CODE
void rewinddir(DIR* dir) {
	if(dir == NULL)
		return;

	_dir_t* tempDir = (_dir_t*)dir;
	if(!tempDir->was_read)
		return;

	if(!tempDir->eod)
		fsys_findclose(&(tempDir->info));

	tempDir->eod    = false;
	tempDir->offset = 0;
	tempDir->type   = FSYS_FIND_DIRECTORY;

	char tempMask[strlen(tempDir->path) + 3];
	sprintf(tempMask, "%s/*", tempDir->path);
	if(fsys_findfirst(tempMask, tempDir->type, &(tempDir->info)) != 0) {
		tempDir->type = FSYS_FIND_FILE;
		if(fsys_findfirst(tempMask, tempDir->type, &(tempDir->info)) != 0) {
			tempDir->eod = true;
		}
	}

	if(!tempDir->eod) {
		tempDir->cur_entry.d_ino  = tempDir->info.handle;
		strcpy(tempDir->cur_entry.d_name, tempDir->info.name);
	}
	tempDir->was_read = false;
}

void seekdir(DIR* dir, long offset) {
	if(dir == NULL)
		return;

	_dir_t* tempDir = (_dir_t*)dir;
	if(tempDir->offset == offset)
		return;
	if(tempDir->offset > offset)
		rewinddir(dir);
	while(tempDir->offset < offset)
		readdir(dir);
}

long telldir(DIR* dir) {
	if(dir == NULL)
		return -1;
	_dir_t* tempDir = (_dir_t*)dir;
	return tempDir->offset;
}
#endif /* DINGOO_BUILD_COMMENTED_OUT_CODE */
