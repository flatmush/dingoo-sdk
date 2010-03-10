#include <string.h>
#include <stdint.h>

int _strlen(const char* inStr) {
	uintptr_t i;
	for(i = 0; inStr[i] != '\0'; i++);
	return i;
}



char* strchr(char* inStr, int inChar) {
	if(inStr == NULL)
		return NULL;
	if((inChar < -128) || (inChar >= 256))
		return NULL;
	char tempChar = (inChar >= 128 ? (inChar - 256) : inChar);
	if(tempChar == '\0')
		return &inStr[strlen(inStr)];
	uintptr_t i;
	for(i = 0; inStr[i] != '\0'; i++) {
		if(inStr[i] == tempChar)
			return &inStr[i];
	}
	return NULL;
}

char* strstr(char* inStr, char* inFind) {
	if((inStr == NULL) || (inFind == NULL))
		return NULL;
	if(inFind[0] == '\0')
		return &inStr[strlen(inStr)];
	uintptr_t i, j;
	for(i = 0; inStr[i] != '\0'; i++) {
		if(inStr[i] == inFind[0]) {
			for(j = 0; inStr[i + j] == inFind[j]; j++) {
				if(inFind[j] == '\0')
					return &inStr[i];
			}
		}
	}
	return NULL;
}



char* strcpy(char* outDest, const char* inSrc) {
	if((outDest == NULL) || (inSrc == NULL))
		return NULL;
	size_t i;
	for(i = 0; inSrc[i] != '\0'; i++)
		outDest[i] = inSrc[i];
	outDest[i] = '\0';
	return outDest;
}

char* strncpy(char* outDest, const char* inSrc, size_t inLength) {
	if((outDest == NULL) || (inSrc == NULL))
		return NULL;
	size_t i;
	for(i = 0; (inSrc[i] != '\0') && (i < inLength); i++)
		outDest[i] = inSrc[i];
	outDest[i] = '\0';
	return outDest;
}



int _strcmp(const char* inStr0, const char* inStr1) {
	if(inStr0 == NULL) {
		if(inStr1 == NULL)
			return 0;
		return -inStr1[0];
	}
	if(inStr1 == NULL)
		return (unsigned char)inStr0[0];
	uintptr_t i;
	for(i = 0; (inStr0[i] != '\0') || (inStr1[i] != '\0'); i++) {
		if(inStr0[i] != inStr1[i])
			return (inStr0[i] - inStr1[i]);
	}
	return 0;
}

int strncmp(const char* inStr0, const char* inStr1, size_t inLength) {
	if(inStr0 == NULL) {
		if(inStr1 == NULL)
			return 0;
		return -inStr1[0];
	}
	if(inStr1 == NULL)
		return (unsigned char)inStr0[0];
	uintptr_t i;
	for(i = 0; (i < inLength) && ((inStr0[i] != '\0') || (inStr1[i] != '\0')); i++) {
		if(inStr0[i] != inStr1[i])
			return (inStr0[i] - inStr1[i]);
	}
	return 0;
}



void* memchr (void* inPtr, int inChar, size_t inLength) {
	if(inPtr == NULL)
		return NULL;
	if((inChar < -128) || (inChar >= 256))
		return NULL;
	unsigned char tempChar = (inChar < 0 ? (inChar + 256) : inChar);
	unsigned char* tempPtr = (unsigned char*)inPtr;
	uintptr_t i;
	for(i = 0; i < inLength; i++) {
		if(tempPtr[i] == tempChar)
			return &tempPtr[i];
	}
	return NULL;
}

void* _memcpy(void* outDest, const void* inSrc, size_t inLength) {
	if((outDest == NULL) || (inSrc == NULL))
		return NULL;
	size_t i;
	for(i = 0; i < inLength; i++)
		((uint8_t*)outDest)[i] = ((uint8_t*)inSrc)[i];
	// TODO - Optimize.
	return outDest;
}

void* memmove(void* outDest, const void* inSrc, size_t inLength) {
	if((uintptr_t)outDest < (uintptr_t)inSrc)
		return memcpy(outDest, inSrc, inLength);
	if((uintptr_t)outDest >= ((uintptr_t)inSrc + inLength))
		return memcpy(outDest, inSrc, inLength);
	size_t i;
	for(i = (inLength - 1); (i + 1) != 0; i--)
		((uint8_t*)outDest)[i] = ((uint8_t*)inSrc)[i];
	// TODO - Optimize, make memcpy do memmove anyway and just make this a wrapper of memcpy.
	return outDest;
}

void* _memset(void* outDest, int inValue, size_t inLength) {
	if(outDest == NULL)
		return NULL;
	if((inValue >= 256) || (inValue < -128))
		return NULL;
	size_t i;
	for(i = 0; i < inLength; i++)
		((uint8_t*)outDest)[i] = (uint8_t)inValue;
	// TODO - Optimize.
	return outDest;
}
