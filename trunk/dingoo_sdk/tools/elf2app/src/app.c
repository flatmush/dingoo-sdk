#include "app.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



typedef struct {
	char     ident[4];
	uint8_t  unknown[20];
	uint8_t  padding[8];
} __attribute__((__packed__)) _app_ccdl;

_app_ccdl _app_ccdl_default = {
	{ 'C', 'C', 'D', 'L' },
	{
		0x00, 0x00, 0x01, 0x00,
		0x01, 0x00, 0x02, 0x00,
		0x04, 0x00, 0x00, 0x00,
		0x20, 0x09, 0x06, 0x24,
		0x19, 0x24, 0x42, 0x00
	},
	{
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	}
};



typedef struct {
	char     ident[4];
	uint32_t unknown;
	uint32_t offset;
	uint32_t size;
	uint8_t  padding[16];
} __attribute__((__packed__)) _app_impt;

_app_impt _app_impt_default = {
	{ 'I', 'M', 'P', 'T' },
	0x00000008,
	0,
	0,
	{
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	}
};



typedef struct {
	char     ident[4];
	uint32_t unknown;
	uint32_t offset;
	uint32_t size;
	uint8_t  padding[16];
} __attribute__((__packed__)) _app_expt;

_app_expt _app_expt_default = {
	{ 'E', 'X', 'P', 'T' },
	0x00000009,
	0,
	0,
	{
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	}
};



typedef struct {
	char     ident[4];
	uint32_t unknown0;
	uint32_t offset;
	uint32_t size;
	uint32_t unknown1;
	uint32_t entry;
	uint32_t origin;
	uint32_t prog_size;
} __attribute__((__packed__)) _app_rawd;

_app_rawd _app_rawd_default = {
	{ 'R', 'A', 'W', 'D' },
	0x00000001,
	0,
	0,
	0x00000000,
	0x00000000,
	0x80A00000,
	0
};



typedef struct {
	uint32_t str_offset;
	uint32_t unknown[2];
	uint32_t offset;
} __attribute__((__packed__)) _app_impt_entry;

_app_impt_entry _app_impt_entry_default = {
	0, { 0, 0x00020000 }, 0
};



typedef struct {
	uint32_t str_offset;
	uint32_t unknown[2];
	uint32_t offset;
} __attribute__((__packed__)) _app_expt_entry;

_app_expt_entry _app_expt_entry_default = {
	0, { 0, 0x00020000 }, 0
};




uintptr_t _app_strlen(const char* inString) {
	uintptr_t tempLen = strlen(inString);
	tempLen += (4 - (tempLen & 3));
	return tempLen;
}

app* app_create(void* inData, uint32_t inSize, uint32_t inEntry, uint32_t inBSS) {
	app* tempApp = (app*)malloc(sizeof(app));
	if(tempApp == NULL)
		return NULL;

	tempApp->import_count = 0;
	tempApp->import_data  = NULL;

	tempApp->export_count = 0;
	tempApp->export_data  = NULL;

	tempApp->bin_size  = inSize;
	tempApp->bin_data  = inData;
	tempApp->bin_entry = inEntry;
	tempApp->bin_bss   = inBSS;

	return tempApp;
}


void app_delete(app* inApp) {
	if(inApp == NULL)
		return;

	uintptr_t i;

	if(inApp->import_data != NULL) {
		for(i = 0; i < inApp->import_count; i++)
			free(inApp->import_data[i]);
		free(inApp->import_data);
	}

	if(inApp->export_data != NULL) {
		for(i = 0; i < inApp->export_count; i++)
			free(inApp->export_data[i]);
		free(inApp->export_data);
	}

	if(inApp->bin_data != NULL)
		free(inApp->bin_data);

	free(inApp);
}



bool app_import_add(app* inApp, const char* inName, uint32_t inOffset) {
	if((inApp == NULL) || (inName == NULL))
		return false;

	app_import_entry* tempEntry = (app_import_entry*)malloc(sizeof(app_import_entry) + strlen(inName) + 1);
	if(tempEntry == NULL)
		return false;
	tempEntry->offset = inOffset;
	tempEntry->name = (char*)((uintptr_t)tempEntry + sizeof(app_import_entry));
	strcpy(tempEntry->name, inName);

	app_import_entry** tempRealloc = (app_import_entry**)realloc(inApp->import_data, (sizeof(app_import_entry*) * (inApp->import_count + 1)));
	if(tempRealloc == NULL) {
		free(tempEntry);
		return false;
	}

	inApp->import_data = tempRealloc;
	inApp->import_data[inApp->import_count] = tempEntry;
	inApp->import_count++;

	return true;
}

bool app_export_add(app* inApp, const char* inName, uint32_t inOffset) {
	if((inApp == NULL) || (inName == NULL))
		return false;

	app_export_entry* tempEntry = (app_export_entry*)malloc(sizeof(app_export_entry) + strlen(inName) + 1);
	if(tempEntry == NULL)
		return false;
	tempEntry->offset = inOffset;
	tempEntry->name = (char*)((uintptr_t)tempEntry + sizeof(app_export_entry));
	strcpy(tempEntry->name, inName);

	app_export_entry** tempRealloc = (app_export_entry**)realloc(inApp->export_data, (sizeof(app_export_entry*) * (inApp->export_count + 1)));
	if(tempRealloc == NULL) {
		free(tempEntry);
		return false;
	}

	inApp->export_data = tempRealloc;
	inApp->export_data[inApp->export_count] = tempEntry;
	inApp->export_count++;

	return true;
}



void _fprint_string(const char* inString, FILE* inStream) {
	uintptr_t tempLen = strlen(inString);
	fwrite(inString, 1, tempLen, inStream);
	uintptr_t i;
	for(i = 0; i < (4 - (tempLen & 3)); i++)
		fwrite(&inString[tempLen], 1, 1, inStream);
}

bool app_save(app* inApp, const char* inPath) {
	if((inApp == NULL) || (inPath == NULL))
		return false;

	uintptr_t i;

	_app_ccdl tempCCDL = _app_ccdl_default;
	_app_impt tempIMPT = _app_impt_default;
	_app_expt tempEXPT = _app_expt_default;
	_app_rawd tempRAWD = _app_rawd_default;

	uint32_t tempPadding[4] = { 0, 0, 0, 0 };

	tempIMPT.offset = sizeof(_app_ccdl) + sizeof(_app_impt) + sizeof(_app_expt) + sizeof(_app_rawd);

	tempIMPT.size = (sizeof(_app_impt_entry) * (inApp->import_count + 1));
	for(i = 0; i < inApp->import_count; i++)
		tempIMPT.size += _app_strlen(inApp->import_data[i]->name);
	uintptr_t tempIPad = (16 - (tempIMPT.size & 15)) & 15;
	tempIMPT.size += tempIPad;


	tempEXPT.offset = tempIMPT.offset + tempIMPT.size;
	tempEXPT.size = (sizeof(_app_expt_entry) * (inApp->export_count + 1));
	for(i = 0; i < inApp->export_count; i++)
		tempEXPT.size += _app_strlen(inApp->export_data[i]->name);
	uintptr_t tempEPad = (16 - (tempEXPT.size & 15)) & 15;
	tempEXPT.size += tempEPad;

	tempRAWD.offset = tempEXPT.offset + tempEXPT.size;
	tempRAWD.size   = (inApp->bin_size + 15) & 0xFFFFFFF0;
	tempRAWD.origin = 0x80A00000;
	tempRAWD.entry  = inApp->bin_entry;
	tempRAWD.prog_size = tempRAWD.size + inApp->bin_bss;

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	fwrite(&tempCCDL, sizeof(_app_ccdl), 1, tempFile);
	fwrite(&tempIMPT, sizeof(_app_impt), 1, tempFile);
	fwrite(&tempEXPT, sizeof(_app_expt), 1, tempFile);
	fwrite(&tempRAWD, sizeof(_app_rawd), 1, tempFile);
	// TODO - Consider extra field.



	// Write Import Table
	_app_impt_entry tempIHeader = { inApp->import_count, { 0, 0 }, 0 };
	fwrite(&tempIHeader, sizeof(_app_impt_entry), 1, tempFile);
	_app_impt_entry tempIEntry = _app_impt_entry_default;
	for(i = 0; i < inApp->import_count; i++) {
		tempIEntry.offset = inApp->import_data[i]->offset;
		fwrite(&tempIEntry, sizeof(_app_impt_entry), 1, tempFile);
		tempIEntry.str_offset += _app_strlen(inApp->import_data[i]->name);
	}

	// Write Import Strings
	for(i = 0; i < inApp->import_count; i++)
		_fprint_string(inApp->import_data[i]->name, tempFile);

	// Write Import Whitespace
	fwrite(tempPadding, 1, tempIPad, tempFile);



	// Write Export Table
	_app_expt_entry tempEHeader = { inApp->export_count, { 0, 0 }, 0 };
	fwrite(&tempEHeader, sizeof(_app_expt_entry), 1, tempFile);
	_app_expt_entry tempEEntry = _app_expt_entry_default;
	for(i = 0; i < inApp->export_count; i++) {
		tempEEntry.offset = inApp->export_data[i]->offset;
		fwrite(&tempEEntry, sizeof(_app_expt_entry), 1, tempFile);
		tempEEntry.str_offset += _app_strlen(inApp->export_data[i]->name);
	}

	// Write Export Strings
	for(i = 0; i < inApp->export_count; i++)
		_fprint_string(inApp->export_data[i]->name, tempFile);

	// Write Export Whitespace
	fwrite(tempPadding, 1, tempEPad, tempFile);



	// Write Binary Data
	fwrite(inApp->bin_data, 1, inApp->bin_size, tempFile);
	fwrite(tempPadding, 1, ((16 - (inApp->bin_size & 15)) & 15), tempFile);

	// TODO - Append res data.

	fclose(tempFile);
	return true;
}
