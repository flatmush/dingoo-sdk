#include <string.h>
#include <stdint.h>

#include <jz4740/dma.h>



int strlen(const char* inStr) {
	uintptr_t i;
	for(i = 0; inStr[i] != '\0'; i++);
	return i;
}



const char* strchr(const char* inStr, int inChar) {
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

const char* strrchr(const char* inStr, int inChar) {
	if(inStr == NULL)
		return NULL;
	if((inChar < -128) || (inChar >= 256))
		return NULL;
	char tempChar = (inChar >= 128 ? (inChar - 256) : inChar);
	if(tempChar == '\0')
		return &inStr[strlen(inStr)];
	uintptr_t i;
	const char* tempOut = NULL;
	for(i = 0; inStr[i] != '\0'; i++) {
		if(inStr[i] == tempChar)
			tempOut = &inStr[i];
	}
	return tempOut;
}

const char* strstr(const char* inStr, const char* inFind) {
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

char* strncpy(char* outDest, const const char* inSrc, size_t inLength) {
	if((outDest == NULL) || (inSrc == NULL))
		return NULL;
	size_t i;
	for(i = 0; (inSrc[i] != '\0') && (i < inLength); i++)
		outDest[i] = inSrc[i];
	outDest[i] = '\0';
	return outDest;
}



char* strcat(char* outDest, const char* inSrc) {
	if((outDest == NULL) || (inSrc == NULL))
		return outDest;

	uintptr_t i, j;
	for(i = 0, j = strlen(outDest); inSrc[i] != '\0'; i++, j++)
		outDest[j] = inSrc[i];
	outDest[j] = '\0';
	return outDest;
}



int _strcmp(const char* inStr0, const char* inStr1) {
	if(inStr0 == NULL) {
		if(inStr1 == NULL)
			return 0;
		return -((unsigned char)inStr1[0]);
	}
	if(inStr1 == NULL)
		return (unsigned char)inStr0[0];
	uintptr_t i;
	for(i = 0; (inStr0[i] == inStr1[i]); i++) {
		if(inStr0[i] == '\0')
			return 0;
	}
	return ((unsigned char)inStr0[i] - (unsigned char)inStr1[i]);
}

int strncmp(const char* inStr0, const char* inStr1, size_t inLength) {
	if(inStr0 == NULL) {
		if(inStr1 == NULL)
			return 0;
		return -((unsigned char)inStr1[0]);
	}
	if(inStr1 == NULL)
		return (unsigned char)inStr0[0];
	uintptr_t i;
	for(i = 0; (i < inLength); i++) {
		if(inStr0[i] != inStr1[i])
			return ((unsigned char)inStr0[i] - (unsigned char)inStr1[i]);
		if(inStr0[i] == '\0')
			return 0;
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

	/*if(inLength > 16384) {
		uintptr_t tempChannel = dma_free();
		if(tempChannel < 6) {
			dma_copy(tempChannel, outDest, inSrc, inLength);
			dma_wait(tempChannel, 0);
			return outDest;
		}
	}*/

	size_t i, j;

	/*if((((uintptr_t)outDest | (uintptr_t)inSrc) & 15) == 0) {
		register uint32_t a, b, c, d;
		i = 0;
		for(i = 0, j = 0; i < (inLength >> 4); i++, j += 4) {
			a = ((uint32_t*)inSrc)[j + 0];
			b = ((uint32_t*)inSrc)[j + 1];
			c = ((uint32_t*)inSrc)[j + 2];
			d = ((uint32_t*)inSrc)[j + 3];
			((uint32_t*)outDest)[j + 0] = a;
			((uint32_t*)outDest)[j + 1] = b;
			((uint32_t*)outDest)[j + 2] = c;
			((uint32_t*)outDest)[j + 3] = d;
		}
		for(i <<= 4; i < inLength; i++)
			((uint8_t*)outDest)[i] = ((uint8_t*)inSrc)[i];
	} else if((((uintptr_t)outDest | (uintptr_t)inSrc) & 7) == 0) {
		register uint32_t a, b;
		for(i = 0, j = 0; i < (inLength >> 3); i++, j += 2) {
			a = ((uint32_t*)inSrc)[j + 0];
			b = ((uint32_t*)inSrc)[j + 1];
			((uint32_t*)outDest)[j + 0] = a;
			((uint32_t*)outDest)[j + 1] = b;
		}
		for(i <<= 3; i < inLength; i++)
			((uint8_t*)outDest)[i] = ((uint8_t*)inSrc)[i];
	} else if((((uintptr_t)outDest | (uintptr_t)inSrc) & 3) == 0) {
		for(i = 0; i < (inLength >> 2); i++)
			((uint32_t*)outDest)[i] = ((uint32_t*)inSrc)[i];
		for(i <<= 2; i < inLength; i++)
			((uint8_t*)outDest)[i] = ((uint8_t*)inSrc)[i];
	} else if((((uintptr_t)outDest | (uintptr_t)inSrc) & 1) == 0) {
		for(i = 0; i < (inLength >> 1); i++)
			((uint16_t*)outDest)[i] = ((uint16_t*)inSrc)[i];
		if((i << 1) < inLength)
			((uint8_t*)outDest)[(i << 1) + 1] = ((uint8_t*)inSrc)[(i << 1) + 1];
	} else */{
		for(i = 0; i < inLength; i++)
			((uint8_t*)outDest)[i] = ((uint8_t*)inSrc)[i];
	}

	// TODO - Optimize further for dest aligned copies.

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
