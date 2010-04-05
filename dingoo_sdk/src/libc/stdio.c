#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#undef fread
#undef fwrite

FILE*  stdin  = (FILE*)((uintptr_t)NULL + 1);
FILE*  stdout = (FILE*)((uintptr_t)NULL + 2);
FILE*  stderr = (FILE*)((uintptr_t)NULL + 3);
FILE* _serial = (FILE*)((uintptr_t)NULL + 4);



typedef enum {
	_file_type_file,
	_file_type_mem
} _file_type_e;

typedef struct {
	_file_type_e type;
	void*        data;
} _file_t;

typedef struct {
	void*     base;
	uintptr_t size;
	uintptr_t offset;
	bool      read, write;
	bool      alloc;
} _file_mem_t;



extern char* _app_path;



char* _file_path(const char* inPath) {
	if(inPath == NULL)
		return NULL;

	uintptr_t i, j;
	for(i = 0, j = 0; inPath[i] != '\0'; i++) {
		if((inPath[i] == '/') || (inPath[i] == '\\'))
			j = i;
	}

	char tempPath[strlen(_app_path) + strlen(inPath) + 3];
	if((inPath[0] == '/') || (inPath[0] == '\\')) {
		// TODO - Fix unix style absolute paths.
		return NULL;
	}

	if(inPath[1] == ':')
		strcpy(tempPath, inPath);
	else
		sprintf(tempPath, "%s/%s", _app_path, inPath);

	for(i = 0, j = 0; tempPath[i] != '\0'; i++) {
		if((tempPath[i] == '/') || (tempPath[i] == '\\')) {
			if(tempPath[i + 1] == '.') {
				if((tempPath[i + 2] == '/') || (tempPath[i + 2] == '\\'))
					strcpy(&tempPath[i], &tempPath[i + 2]);
				else if((tempPath[i + 2] == '.') && ((tempPath[i + 3] == '/') || (tempPath[i + 3] == '\\'))) {
					strcpy(&tempPath[j], &tempPath[i + 3]);
					i = j;
				} else j = i;
			} else j = i;
		}
	}

	char* tempOut = (char*)malloc(strlen(tempPath) + 1);
	if(tempOut != NULL)
		strcpy(tempOut, tempPath);
	return tempOut;
}



FILE* _fopen(const char* filename, const char* mode) {
	if(strcmp(filename, "COM0") == 0)
		return _serial;

	char* tempPath = _file_path(filename);
	if(tempPath == NULL)
		return (FILE*)fsys_fopen(filename, mode);

	_file_t* tempFile = (_file_t*)malloc(sizeof(_file_t));
	if(tempFile == NULL) {
		free(tempPath);
		return NULL;
	}

	tempFile->type = _file_type_file;
	tempFile->data = (void*)fsys_fopen(tempPath, mode);

	free(tempPath);
	if(tempFile->data == NULL) {
		free(tempFile);
		return NULL;
	}

	return (FILE*)tempFile;
}

void _fclose(FILE* stream) {
	if(stream == stdin)
		return;
	if(stream == stdout)
		return;
	if(stream == stderr)
		return;
	if(stream == _serial)
		return;

	_file_t* tempFile = (_file_t*)stream;

	if(tempFile->type == _file_type_file)
		fsys_fclose((FILE*)tempFile->data);

	if(tempFile->type == _file_type_mem) {
		_file_mem_t* tempFileMem = (_file_mem_t*)tempFile->data;
		if(tempFileMem->alloc)
			free(tempFileMem->base);
	}

	free(tempFile);
}



int _fseek(FILE* stream, long int offset, int origin) {
	if((stream == stdin) || (stream == stdout) || (stream == stderr))
		return -1;
	if(stream == _serial)
		return -1;

	_file_t* tempFile = (_file_t*)stream;

	if(tempFile->type == _file_type_file)
		return fsys_fseek((FILE*)tempFile->data, offset, origin);

	if(tempFile->type == _file_type_mem) {
		_file_mem_t* tempFileMem = (_file_mem_t*)tempFile->data;

		intptr_t tempOffset;
		switch(origin) {
			case SEEK_SET:
				tempOffset = offset;
				break;
			case SEEK_CUR:
				tempOffset = tempFileMem->offset + offset;
				break;
			case SEEK_END:
				tempOffset = tempFileMem->size + offset;
				break;
			default:
				return -1;
		}

		if(tempOffset < 0)
			return -1;
		if((uintptr_t)tempOffset > tempFileMem->size)
			return -1;

		tempFileMem->offset = (uintptr_t)tempOffset;
		return 0;
	}

	return -1;
}

long int _ftell(FILE* stream) {
	if((stream == stdin) || (stream == stdout) || (stream == stderr))
		return -1;
	if(stream == _serial)
		return -1;

	_file_t* tempFile = (_file_t*)stream;

	if(tempFile->type == _file_type_file)
		return fsys_ftell((FILE*)tempFile->data);

	if(tempFile->type == _file_type_mem) {
		_file_mem_t* tempFileMem = (_file_mem_t*)tempFile->data;
		return tempFileMem->offset;
	}

	return -1;
}



int _ferror(FILE* stream) {
	if((stream == stdin) || (stream == stdout) || (stream == stderr))
		return 0;
	if(stream == _serial)
		return 0;

	_file_t* tempFile = (_file_t*)stream;

	if(tempFile->type == _file_type_file)
		return fsys_ferror((FILE*)tempFile->data);

	return 0;
}

int _feof(FILE* stream) {
	if((stream == stdin) || (stream == stdout) || (stream == stderr))
		return 0;
	if(stream == _serial)
		return 0;

	_file_t* tempFile = (_file_t*)stream;

	if(tempFile->type == _file_type_file)
		return fsys_feof((FILE*)tempFile->data);
	if(tempFile->type == _file_type_mem) {
		_file_mem_t* tempFileMem = (_file_mem_t*)tempFile->data;
		return (tempFileMem->offset >= tempFileMem->size);
	}

	return 0;
}



int _fread(void* ptr, size_t size, size_t count, FILE* stream) {
	if((stream == stdout) || (stream == stderr))
		return 0;
	if(stream == stdin) {
		// TODO - Implement stdin somehow.
		return 0;
	}
	if(stream == _serial)
		return fread(NULL, size, count, stream);

	_file_t* tempFile = (_file_t*)stream;

	if(tempFile->type == _file_type_file)
		return fsys_fread(ptr, size, count, (FILE*)tempFile->data);

	if(tempFile->type == _file_type_mem) {
		_file_mem_t* tempFileMem = (_file_mem_t*)tempFile->data;

		if(!tempFileMem->write)
			return -1;

		uintptr_t i, j;
		for(i = 0, j = 0; i < count; i++, j += size) {
			if((tempFileMem->offset + size) > tempFileMem->size)
				break;
			memcpy((void*)((uintptr_t)ptr + j), (void*)((uintptr_t)tempFileMem->base + tempFileMem->offset), size);
			tempFileMem->offset += size;
		}

		return i;
	}

	return 0;
}

int _fwrite(const void* ptr, size_t size, size_t count, FILE* stream) {
	if(stream == stdin)
		return 0;
	if(stream == stdout) {
		// TODO - Implement stdout.
		return 0;
	}
	if(stream == stderr) {
		// TODO - Implement stderr.
		return 0;
	}
	if(stream == _serial)
		return fwrite(NULL, size, count, stream);

	_file_t* tempFile = (_file_t*)stream;

	if(tempFile->type == _file_type_file)
		return fsys_fwrite(ptr, size, count, (FILE*)tempFile->data);

	if(tempFile->type == _file_type_mem) {
		_file_mem_t* tempFileMem = (_file_mem_t*)tempFile->data;

		if(!tempFileMem->read)
			return -1;

		uintptr_t i, j;
		for(i = 0, j = 0; i < count; i++, j += size) {
			if((tempFileMem->offset + size) > tempFileMem->size)
				break;
			memcpy((void*)((uintptr_t)tempFileMem->base + tempFileMem->offset), (void*)((uintptr_t)ptr + j), size);
			tempFileMem->offset += size;
		}

		return i;
	}

	return 0;
}



FILE* fmemopen(void* buf, size_t size, const char* mode) {
	if(mode == NULL)
		return NULL;

	_file_t* tempFile = (_file_t*)malloc(sizeof(_file_t) + sizeof(_file_mem_t));
	if(tempFile == NULL)
		return NULL;

	_file_mem_t* tempFileMem = (_file_mem_t*)((uintptr_t)tempFile + sizeof(_file_t));

	tempFile->type = _file_type_mem;
	tempFile->data = (void*)tempFileMem;

	tempFileMem->alloc = (buf == NULL);

	if(tempFileMem->alloc) {
		buf = malloc(size);
		if(buf == NULL) {
			free(tempFile);
			return NULL;
		}
	}

	tempFileMem->base   = buf;
	tempFileMem->size   = size;
	tempFileMem->offset = 0;
	tempFileMem->read   = (strchr(mode, 'r') != NULL);
	tempFileMem->write  = (strchr(mode, 'w') != NULL);

	if(strchr(mode, 'a') != NULL) {
		uintptr_t i;
		for(i = 0; i < tempFileMem->size; i++) {
			if(*((uint8_t*)((uintptr_t)tempFileMem->base + i)) == '\0') {
				tempFileMem->offset = i;
				break;
			}
		}
	}

	return (FILE*)tempFile;
}



void perror(const char* prefix) {
	//_fprintf(stderr, "%s: %s", prefix, strerror(errno));
}
