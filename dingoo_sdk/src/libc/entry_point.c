#include <stdlib.h>
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

int GameMain(char* respath) {
	if(!_memsys_init())
		return EXIT_FAILURE;
	char* tempAnsiPath = __to_locale_ansi((wchar_t*)respath);
	uintptr_t tempAnsiLen;
	for(tempAnsiLen = 0; tempAnsiPath[tempAnsiLen] != '\0'; tempAnsiLen++);
	char* tempPath = malloc(tempAnsiLen + 1);
	if(tempPath == NULL)
		return EXIT_FAILURE;
	strcpy(tempPath, __to_locale_ansi((wchar_t*)respath));
	int tempOut = main(1, &tempPath);
	free(tempPath);
	_memsys_cleanup();
	return tempOut;
}
