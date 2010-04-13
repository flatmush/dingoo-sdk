#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

extern int main(int, char**);

extern char* __to_locale_ansi(wchar_t*);

extern bool _memsys_init();
extern void _memsys_cleanup();

/*char  __to_locale_ansi_string[256];

char* __to_locale_ansi(wchar_t* inInput) {
	uintptr_t i;
	char* tempString = (char*)inInput;
	for(i = 0; (i < 255) && (tempString[i] != '\0'); i += 2)
		__to_locale_ansi_string[i] = tempString[i];
	tempString[i] = '\0';
	return tempString;
}*/

char* _app_path = NULL;

extern int   _fread(void* ptr, size_t size, size_t count, FILE* stream);
extern int   _fwrite(const void* ptr, size_t size, size_t count, FILE* stream);
extern int   _fseek(FILE* stream, long int offset, int origin);
extern void* _memcpy(void* outDest, const void* inSrc, size_t inLength);
extern void* _memset(void* outDest, int inValue, size_t inLength);

void* dl_patch(void* inOld, void* inNew) {
	if(((uintptr_t)inNew & 3) != 0)
		return NULL;
	if(((uintptr_t)inNew & 0xF0000000) != ((uintptr_t)inOld & 0xF0000000))
		return NULL;
	void* tempOld = *((void**)inOld);
	*((uint32_t*)inOld) = (0x08000000 | (((uint32_t)inNew >> 2) & 0x03FFFFFF));
	return tempOld;
}

char* _app_path_init(const char* inPath) {
	uintptr_t i, j;
	for(i = 0, j = 0; inPath[i] != '\0'; i++) {
		if((inPath[i] == '\\') || (inPath[i] == '/'))
			j = i + 1;
	}
	_app_path = (char*)malloc(j + 1);
	if(_app_path == NULL)
		return NULL;
	strncpy(_app_path, inPath, j);
	return _app_path;
}

int GameMain(char* respath) {
	// Patch functions to use our own fixed versions
	void* __fread  = dl_patch(&fread, &_fread);
	void* __fwrite = dl_patch(&fwrite, &_fwrite);
	void* __fseek  = dl_patch(&fseek, &_fseek);
	void* __memcpy = dl_patch(&memcpy, &_memcpy);
	void* __memset = dl_patch(&memset, &_memset);

	if(!_memsys_init())
		return EXIT_FAILURE;
	char* tempAnsiPath = __to_locale_ansi((wchar_t*)respath);
	uintptr_t tempAnsiLen;
	for(tempAnsiLen = 0; tempAnsiPath[tempAnsiLen] != '\0'; tempAnsiLen++);
	char* tempPath = malloc(tempAnsiLen + 1);
	if(tempPath == NULL)
		return EXIT_FAILURE;
	strcpy(tempPath, __to_locale_ansi((wchar_t*)respath));
	_app_path_init(tempPath);
	int tempOut = main(1, &tempPath);
	free(_app_path);
	free(tempPath);
	_memsys_cleanup();

	// Revert patches.
	dl_patch(&fread, __fread);
	dl_patch(&fwrite, __fwrite);
	dl_patch(&fseek, __fseek);
	dl_patch(&memcpy, __memcpy);
	dl_patch(&memset, __memset);

	return tempOut;
}
