#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

char* app_path = NULL;
char* app_name = NULL;

bool app_path_init(const char* inPath) {
	app_path = malloc(strlen(inPath) + 1);
	if(app_path == NULL)
		return false;
	app_path = strcpy(app_path, inPath);

	uintptr_t i, j;
	for(i = 0, j = 0; inPath[i] != '\0'; i++) {
		if((inPath[i] == '\\') || (inPath[i] == '/'))
			j = i;
	}

	app_path[j] = '\0';
	app_name = &app_path[j + 1];
	return true;
}

int main(int argc, char** argv) {
	if(!app_path_init(argv[0])) {
		printf("Error: Couldn't calculate application path/name.\n");
		return EXIT_FAILURE;
	}

	if(argc != 2) {
		printf("USAGE: %s <basename>\n", app_name);
		return EXIT_FAILURE;
	}

	char tempHeaderPath[strlen(app_path) + 12];
	sprintf(tempHeaderPath, "%s/Header.bin", app_path);
	FILE* tempFile = fopen(tempHeaderPath, "rb");
	if(tempFile == NULL) {
		printf("Error: Couldn't open \"Header.bin\".\n");
		return EXIT_FAILURE;
	}
	fseek(tempFile, 0, SEEK_END);
	uintptr_t tempHeaderLen = ftell(tempFile);
	fseek(tempFile, 0, SEEK_SET);
	uint32_t tempHeader[tempHeaderLen >> 2];
	fread(tempHeader, 1, tempHeaderLen, tempFile);
	fclose(tempFile);

	char tempCommand[256];
	sprintf(tempCommand, "mipsel-linux-objcopy -O binary -j .dingoo %s.elf %s.bin", argv[1], argv[1]);
	system(tempCommand);

	char tempPath[strlen(argv[1]) + 5];
	sprintf(tempPath, "%s.bin", argv[1]);
	tempFile = fopen(tempPath, "rb");
	if(tempFile == NULL) {
		printf("Error: Couldn't open \"%s.bin\".\n", argv[1]);
		return EXIT_FAILURE;
	}
	fseek(tempFile, 0, SEEK_END);
	uintptr_t tempBodyLen = ftell(tempFile);
	fseek(tempFile, 0, SEEK_SET);
	uint8_t tempBody[tempBodyLen];
	fread(tempBody, 1, tempBodyLen, tempFile);
	fclose(tempFile);

	tempHeader[27] = (uint32_t)tempBodyLen;
	tempHeader[31] = (uint32_t)tempBodyLen;

	sprintf(tempPath, "%s.app", argv[1]);
	tempFile = fopen(tempPath, "wb");
	if(tempFile == NULL) {
		printf("Error: Couldn't open \"%s.app\".\n", argv[1]);
		return EXIT_FAILURE;
	}
	fwrite(tempHeader, 1, tempHeaderLen, tempFile);
	fwrite(tempBody, 1, tempBodyLen, tempFile);
	fclose(tempFile);
	return EXIT_SUCCESS;
}



