#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#undef fread
#undef fwrite

FILE*  stdin  = (FILE*)((uintptr_t)NULL + 1);
FILE*  stdout = (FILE*)((uintptr_t)NULL + 2);
FILE*  stderr = (FILE*)((uintptr_t)NULL + 3);
FILE* _serial = (FILE*)((uintptr_t)NULL + 4);



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
	FILE* tempOut = (FILE*)fsys_fopen(tempPath, mode);
	free(tempPath);
	return tempOut;
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

	fsys_fclose(stream);
}



int _fseek(FILE* stream, long int offset, int origin) {
	if((stream == stdin) || (stream == stdout) || (stream == stderr))
		return -1;
	if(stream == _serial)
		return -1;
	return fsys_fseek(stream, offset, origin);
}

long int _ftell(FILE* stream) {
	return fsys_ftell(stream);
}



int _ferror(FILE* stream) {
	if((stream == stdin) || (stream == stdout) || (stream == stderr))
		return 0;
	if(stream == _serial)
		return 0;
	return fsys_ferror(stream);
}

int _feof(FILE* stream) {
	if((stream == stdin) || (stream == stdout) || (stream == stderr))
		return 0;
	if(stream == _serial)
		return 0;
	return fsys_feof(stream);
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
	return fsys_fread(ptr, size, count, stream);
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
	return fsys_fwrite(ptr, size, count, stream);
}



void perror(const char* prefix) {
	//_fprintf(stderr, "%s: %s", prefix, strerror(errno));
}
