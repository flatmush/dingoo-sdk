#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

bool file_read(const char* inPath, char** outData, uintptr_t* outLength) {
	if((outData == NULL) || (outLength == NULL))
		return false;

	FILE* tempFile = fopen(inPath, "rb");
	if(tempFile == NULL)
		return false;

	fseek(tempFile, 0, SEEK_END);
	uintptr_t tempLen = ftell(tempFile);
	char* tempData = (char*)malloc(tempLen + 1);
	if(tempData == NULL) {
		fclose(tempFile);
		return false;
	}

	fseek(tempFile, 0, SEEK_SET);
	if(fread(tempData, 1, tempLen, tempFile) < tempLen) {
		fclose(tempFile);
		free(tempData);
		return false;
	}
	tempData[tempLen] = '\0';
	fclose(tempFile);

	*outData = tempData;
	*outLength = tempLen;
	return true;
}

bool file_write(const char* inPath, char* inData, uintptr_t inLength) {
	if(inData == NULL)
		return false;

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	fwrite(inData, 1, inLength, tempFile);
	fclose(tempFile);
	return true;
}

bool fix_file(const char* inFile, const char inDrive) {
	char* tempFile;
	uintptr_t tempLen;

	if(!file_read(inFile, &tempFile, &tempLen))
		return false;

	char* tempFind = tempFile;
	uintptr_t tempLenRem;
	uintptr_t tempFixed = 0;
	while(true) {
		tempLenRem = tempLen - ((uintptr_t)tempFind - (uintptr_t)tempFile);
		tempFind = (char*)memchr(tempFind, 'd', tempLenRem);
		if(tempFind == NULL)
			break;
		if(strncmp(tempFind, "d/user/daniel", 13) == 0) {
			*tempFind = inDrive;
			tempFixed++;
			printf("\tBad path string %lu replaced in file '%s'.\n", tempFixed, inFile);
		}
		tempFind++;
	}

	if(!file_write(inFile, tempFile, tempLen)) {
		free(tempFile);
		return false;
	}

	free(tempFile);
	return true;
}

int main(int argc, char** argv) {

	char tempDrive = (argc < 3 ? 'c' : *(argv[1]));
	printf("INFO: Changing bad drive letters to '%c'.\n", tempDrive);

	char* tempToolPath = getenv("MIPSTOOLS");
	if(tempToolPath == NULL) {
		printf("ERROR: MIPSTOOLS environment variable not set.\n");
		return EXIT_FAILURE;
	}

	char tempPath[strlen(tempToolPath) + FILENAME_MAX + 2];

	sprintf(tempPath, "%s/bin/mipsel-linux-gcc.exe", tempToolPath);
	printf("INFO: Fixing '%s'.\n", tempPath);
	if(fix_file(tempPath, tempDrive))
		printf("\t'%s' fixed.\n", tempPath);
	else
		printf("\t'%s' not fixed.\n", tempPath);

	sprintf(tempPath, "%s/bin/mipsel-linux-gcc-4.1.2.exe", tempToolPath);
	printf("INFO: Fixing '%s'.\n", tempPath);
	if(fix_file(tempPath, tempDrive))
		printf("\t'%s' fixed.\n", tempPath);
	else
		printf("\t'%s' not fixed.\n", tempPath);

	sprintf(tempPath, "%s/bin/mipsel-linux-g++.exe", tempToolPath);
	printf("INFO: Fixing '%s'.\n", tempPath);
	if(fix_file(tempPath, tempDrive))
		printf("\t'%s' fixed.\n", tempPath);
	else
		printf("\t'%s' not fixed.\n", tempPath);

	sprintf(tempPath, "%s/bin/mipsel-linux-c++.exe", tempToolPath);
	printf("INFO: Fixing '%s'.\n", tempPath);
	if(fix_file(tempPath, tempDrive))
		printf("\t'%s' fixed.\n", tempPath);
	else
		printf("\t'%s' not fixed.\n", tempPath);

	sprintf(tempPath, "%s/bin/mipsel-linux-cpp.exe", tempToolPath);
	printf("INFO: Fixing '%s'.\n", tempPath);
	if(fix_file(tempPath, tempDrive))
		printf("\t'%s' fixed.\n", tempPath);
	else
		printf("\t'%s' not fixed.\n", tempPath);

	sprintf(tempPath, "%s/libexec/gcc/mipsel-linux/4.1.2/cc1.exe", tempToolPath);
	printf("INFO: Fixing '%s'.\n", tempPath);
	if(fix_file(tempPath, tempDrive))
		printf("\t'%s' fixed.\n", tempPath);
	else
		printf("\t'%s' not fixed.\n", tempPath);

	sprintf(tempPath, "%s/libexec/gcc/mipsel-linux/4.1.2/cc1plus.exe", tempToolPath);
	printf("INFO: Fixing '%s'.\n", tempPath);
	if(fix_file(tempPath, tempDrive))
		printf("\t'%s' fixed.\n", tempPath);
	else
		printf("\t'%s' not fixed.\n", tempPath);

	sprintf(tempPath, "%s/libexec/gcc/mipsel-linux/4.1.2/collect2.exe", tempToolPath);
	printf("INFO: Fixing '%s'.\n", tempPath);
	if(fix_file(tempPath, tempDrive))
		printf("\t'%s' fixed.\n", tempPath);
	else
		printf("\t'%s' not fixed.\n", tempPath);

	printf("INFO: All files fixed.\n");
	return EXIT_SUCCESS;
}
