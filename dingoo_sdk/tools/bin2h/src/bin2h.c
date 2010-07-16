#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

void printUsage() {
	printf("Usage: bin2h <bin> <name>\n");
}

int main(int argc, char** argv) {
	if(argc < 3) {
		printUsage();
		return EXIT_FAILURE;
	}

	char tempHeaderPath[256];
	sprintf(tempHeaderPath, "%s.h", argv[2]);

	FILE* tempOutFile = fopen(tempHeaderPath, "wb");
	if(tempOutFile == NULL) {
		printf("Error: Couldn't open \"%s\".\n", tempHeaderPath);
		return EXIT_FAILURE;
	}

	// get only name without path
	char* name = argv[2] + strlen(argv[2]);
	while(name > argv[2] && *name != '/' && *name != '\\')
		--name;
	if(name != argv[2])
		++name;

	fprintf(tempOutFile, "#ifndef __bin2h_%s_h__\n", name);
	fprintf(tempOutFile, "#define __bin2h_%s_h__\n", name);
	fprintf(tempOutFile, "\n");

	fprintf(tempOutFile, "#ifdef __cplusplus\n");
	fprintf(tempOutFile, "extern \"C\" {\n");
	fprintf(tempOutFile, "#endif\n");
	fprintf(tempOutFile, "\n");
	fprintf(tempOutFile, "#include <stdint.h>\n");
	fprintf(tempOutFile, "\n");

	FILE* tempInFile = fopen(argv[1], "rb");
	if(tempInFile == NULL) {
		printf("Error: Couldn't open \"%s\".\n", argv[1]);
		fclose(tempOutFile);
		return EXIT_FAILURE;
	}

	fseek(tempInFile, 0, SEEK_END);
	unsigned long int tempSize = ftell(tempInFile);
	fseek(tempInFile, 0, SEEK_SET);

	fprintf(tempOutFile, "uintptr_t %s_size = %lu;\n", name, tempSize);
	fprintf(tempOutFile, "uint8_t   %s[%lu] = {\n", name, tempSize);

	unsigned char tempByte;
	unsigned int tempHex;
	unsigned long int i;
	for(i = 0; i < tempSize; i++) {
		if(i) {
			fprintf(tempOutFile, ", ");
			if((i & 7) == 0)
				fprintf(tempOutFile, "\n");
		}
		fread(&tempByte, 1, 1, tempInFile);
		tempHex = tempByte;
		fprintf(tempOutFile, "0x%02X", tempHex);
	}

	fprintf(tempOutFile, "\n");
	fprintf(tempOutFile, "};\n");

	fclose(tempInFile);

	fprintf(tempOutFile, "\n");
	fprintf(tempOutFile, "#ifdef __cplusplus\n");
	fprintf(tempOutFile, "}\n");
	fprintf(tempOutFile, "#endif\n");

	fprintf(tempOutFile, "\n");
	fprintf(tempOutFile, "#endif\n");

	fclose(tempOutFile);

	return EXIT_SUCCESS;
}

#ifdef __cplusplus
}
#endif

