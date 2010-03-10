#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#undef printf

FILE*  stdin  = (FILE*)((uintptr_t)NULL + 1);
FILE*  stdout = (FILE*)((uintptr_t)NULL + 2);
FILE*  stderr = (FILE*)((uintptr_t)NULL + 3);
FILE* _serial = (FILE*)((uintptr_t)NULL + 4);

FILE* _fopen(char* filename, char* mode) {
	if(strcmp(filename, "COM0") == 0)
		return _serial;
	if(strchr(filename, ':') == NULL) {
		// Relative path.
	}
	return (FILE*)fsys_fopen(filename, mode);
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



/*int _fseek(FILE* stream, long int, int) {

}

long int _ftell(FILE* stream) {

}



int _ferror(FILE* stream) {

}

int _feof(FILE* stream) {


}



int _fread(void*, size_t, size_t, FILE*) {

}

int _fwrite(const void*, size_t, size_t, FILE* stream) {

}*/



void perror(const char* prefix) {
	//_fprintf(stderr, "%s: %s", prefix, strerror(errno));
}
