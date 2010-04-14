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



extern char* _app_path;
extern char* _file_path(const char* inPath);



DIR* opendir(const char* name) {
	char* tempPath = _file_path(name);
	if(tempPath == NULL)
		return NULL;

	_dir_t* tempDir = (_dir_t*)malloc(sizeof(_dir_t));
	if(tempDir == NULL) {
		free(tempPath);
		return NULL;
	}

	tempDir->path   = tempPath;
	tempDir->eod    = false;
	tempDir->offset = 0;
	tempDir->type   = FSYS_FIND_DIRECTORY;

	char tempMask[strlen(tempDir->path) + 3];
	sprintf(tempMask, "%s\\*", tempDir->path);
	if(fsys_findfirst(tempMask, tempDir->type, &(tempDir->info)) != 0) {
		tempDir->type = FSYS_FIND_FILE;
		if(fsys_findfirst(tempMask, tempDir->type, &(tempDir->info)) != 0) {
			tempDir->eod = true;
		}
	}

	if(!tempDir->eod) {
		tempDir->cur_entry.d_ino  = tempDir->info.handle;
		tempDir->cur_entry.d_name = tempDir->info.name;
	}
	tempDir->was_read = false;

	return (DIR*)tempDir;
}

int closedir(DIR* dir) {
	if(dir == NULL)
		return -1;

	_dir_t* tempDir = (_dir_t*)dir;
	if(!tempDir->eod)
		fsys_findclose(&(tempDir->info));
	if(tempDir->path != NULL)
		free(tempDir->path);
	free(tempDir);

	return 0;
}



struct dirent* readdir(DIR* dir) {
	if(dir == NULL)
		return NULL;

	_dir_t* tempDir = (_dir_t*)dir;
	if(tempDir->eod)
		return NULL;

	if(!tempDir->was_read) {
		tempDir->was_read = true;
		return &(tempDir->cur_entry);
	}

	if(fsys_findnext(&(tempDir->info)) != 0) {
		fsys_findclose(&(tempDir->info));
		if(tempDir->type == FSYS_FIND_FILE) {
			tempDir->eod = true;
			return NULL;
		} else {
			tempDir->type = FSYS_FIND_FILE;
			char tempMask[strlen(tempDir->path) + 3];
			sprintf(tempMask, "%s\\*", tempDir->path);
			if(fsys_findfirst(tempMask, tempDir->type, &(tempDir->info)) != 0) {
				tempDir->eod = true;
				return NULL;
			}
		}
	}

	tempDir->offset++;
	tempDir->cur_entry.d_ino  = tempDir->info.handle;
	tempDir->cur_entry.d_name = tempDir->info.name;

	return &(tempDir->cur_entry);
}



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
		tempDir->cur_entry.d_name = tempDir->info.name;
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
