#include "elf.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>



elf_symbol_table* elf_symbol_table_create() {
	elf_symbol_table* tempTable = (elf_symbol_table*)malloc(sizeof(elf_symbol_table));
	if(tempTable == NULL)
		return NULL;
	tempTable->size = 0;
	tempTable->data = NULL;
	return tempTable;
}

void elf_symbol_table_delete(elf_symbol_table* inTable) {
	if(inTable == NULL)
		return;

	uintptr_t i;

	if(inTable->data != NULL) {
		for(i = 0; i < inTable->size; i++)
			free(inTable->data[i]);
		free(inTable->data);
	}

	free(inTable);
}



bool elf_symbol_table_add(elf_symbol_table* inTable, elf_symbol_entry* inEntry) {
	if((inTable == NULL) || (inEntry == NULL))
		return false;

	elf_symbol_entry** tempRealloc = (elf_symbol_entry**)realloc(inTable->data, (sizeof(elf_symbol_entry*) * (inTable->size + 1)));
	if(tempRealloc == NULL)
		return false;

	inTable->data = tempRealloc;
	inTable->data[inTable->size] = inEntry;
	inTable->size++;

	return true;

}

elf_symbol_entry* elf_symbol_entry_parse(const char* inLine) {
	uint32_t tempOffset;
	char     tempName[2048];

	tempName[0] = '\0';
	if(sscanf(inLine, "%" SCNx32 " %*c %*c .dingoo %*x %s", &tempOffset, tempName) < 0)
		return NULL;
	if(!isalnum(tempName[0]) && (tempName[0] != '_'))
		return NULL;

	uintptr_t i;
	for(i = 1; (isalnum(tempName[i]) || (tempName[i] == '_')); i++);
	tempName[i] = '\0';


	elf_symbol_entry* tempEntry = (elf_symbol_entry*)malloc(sizeof(elf_symbol_entry) + strlen(tempName) + 1);
	if(tempEntry == NULL)
		return NULL;

	tempEntry->name = (char*)((uintptr_t)tempEntry + sizeof(elf_symbol_entry));
	strcpy(tempEntry->name, tempName);
	tempEntry->offset = tempOffset;

	return tempEntry;
}

elf_symbol_table* elf_symbol_table_read(const char* inPath) {
	char tempString[512];
	sprintf(tempString, "mipsel-linux-objdump --adjust-vma=0x80a00000 -t %s", inPath);

	FILE* tempFile = popen(tempString, "r");
	if(tempFile == NULL)
		return NULL;

	elf_symbol_table* tempTable = elf_symbol_table_create();
	if(tempTable == NULL) {
		fclose(tempFile);
		return NULL;
	}

	while(!feof(tempFile)) {
		fgets(tempString, 512, tempFile);
		if(strncmp(tempString, "SYMBOL TABLE:", 13) == 0)
			break;
	}

	while(!feof(tempFile)) {
		fgets(tempString, 512, tempFile);
		if(tempString[0] == '\n')
			break;
		elf_symbol_table_add(tempTable, elf_symbol_entry_parse(tempString));
	}

	fclose(tempFile);
	return tempTable;
}



uint32_t elf_symbol_table_find_offset(elf_symbol_table* inTable, const char* inName) {
	if((inTable == NULL) || (inTable->data == NULL))
		return 0;

	uintptr_t i;
	for(i = 0; i < inTable->size; i++) {
		if(strcmp(inTable->data[i]->name, inName) == 0)
			return inTable->data[i]->offset;
	}
	return 0;
}



uint32_t elf_bss_size(const char* inPath) {
	char tempString[512];
	sprintf(tempString, "mipsel-linux-objdump --adjust-vma=0x80a00000 -h %s.elf", inPath);
	FILE* tempFile = popen(tempString, "r");
	if(tempFile == NULL)
		return 0;

	uint32_t tempSize;
	while(!feof(tempFile)) {
		fgets(tempString, 512, tempFile);
		if(sscanf(tempString, " %*u %s %" SCNx32, tempString, &tempSize) >= 0) {
			if(strcmp(tempString, ".bss") == 0) {
				fclose(tempFile);
				return tempSize;
			}
		}
	}
	fclose(tempFile);

	return 0;
}
