#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#include "app.h"
#include "elf.h"



elf_symbol_table* _sym = NULL;
app*              _app = NULL;



bool _label_fix(char* inLabel) {
	uintptr_t i;
	for(i = 0; (isalnum(inLabel[i]) || (inLabel[i] == '_')); i++);
	inLabel[i] = '\0';
	return (i > 0);
}

void _import(const char* inName) {
	uint32_t tempOffset = elf_symbol_table_find_offset(_sym, inName);
	if(tempOffset == 0)
		return;
	//printf("Imported %s at 0x%"PRIX32"\n", inName, tempOffset);
	app_import_add(_app, inName, tempOffset);
}

void _export(const char* inName) {
	uint32_t tempOffset = elf_symbol_table_find_offset(_sym, inName);
	if(tempOffset == 0)
		return;
	//printf("Exported %s at 0x%"PRIX32"\n", inName, tempOffset);
	app_export_add(_app, inName, tempOffset);
}



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

	//printf("Status: Creating program binary.\n");
	char tempCommand[256];
	sprintf(tempCommand, "mipsel-linux-objcopy -O binary -j .dingoo %s.elf %s.bin", argv[1], argv[1]);
	system(tempCommand);

	//printf("Status: Reading program binary.\n");
	char tempPath[1024];
	sprintf(tempPath, "%s.bin", argv[1]);
	FILE* tempFile = fopen(tempPath, "rb");
	if(tempFile == NULL) {
		printf("Error: Couldn't open \"%s.bin\".\n", argv[1]);
		return EXIT_FAILURE;
	}
	fseek(tempFile, 0, SEEK_END);
	uintptr_t tempBodyLen = ftell(tempFile);
	fseek(tempFile, 0, SEEK_SET);
	uint8_t* tempBody = (uint8_t*)malloc(tempBodyLen);
	if(tempBody == NULL) {
		printf("Error: Couldn't allocate memory of binary data, out of memory.\n");
		fclose(tempFile);
		return EXIT_FAILURE;
	}
	fread(tempBody, 1, tempBodyLen, tempFile);
	fclose(tempFile);

	//printf("Status: Reading elf symbol table.\n");
	sprintf(tempPath, "%s.elf", argv[1]);
	_sym = elf_symbol_table_read(tempPath);
	if(_sym == NULL) {
		printf("Error: Couldn't read elf symbol table.\n");
		free(tempBody);
		return EXIT_FAILURE;
	}


	//printf("Status: Creating app structure.\n");
	_app = app_create(tempBody, tempBodyLen, elf_symbol_table_find_offset(_sym, "dl_main"), elf_bss_size(argv[1]));
	if(_app == NULL) {
		printf("Error: Couldn't create app struct.\n");
		free(tempBody);
		elf_symbol_table_delete(_sym);
		return EXIT_FAILURE;
	}

	//printf("Status: Reading import list.\n");
	sprintf(tempPath, "%s/import", app_path);
	tempFile = fopen(tempPath, "r");
	if(tempFile != NULL) {
		while(!feof(tempFile)) {
			fgets(tempPath, 256, tempFile);
			if(_label_fix(tempPath))
				_import(tempPath);
		}
		fclose(tempFile);
	}

	//printf("Status: Reading export list.\n");
	sprintf(tempPath, "%s/export", app_path);
	tempFile = fopen(tempPath, "r");
	if(tempFile != NULL) {
		while(!feof(tempFile)) {
			fgets(tempPath, 256, tempFile);
			if(_label_fix(tempPath))
				_export(tempPath);
		}
		fclose(tempFile);
	}

	elf_symbol_table_delete(_sym);

	//printf("Status: Writing app file.\n");
	sprintf(tempPath, "%s.app", argv[1]);
	if(!app_save(_app, tempPath)) {
		printf("Error: Couldn't save app file.\n");
		app_delete(_app);
		return EXIT_FAILURE;
	}

	app_delete(_app);

	return EXIT_SUCCESS;
}



