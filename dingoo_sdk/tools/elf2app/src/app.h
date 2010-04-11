#ifndef __app_h__
#define __app_h__

#include <stdbool.h>
#include <stdint.h>


typedef struct {
	uintptr_t offset;
	char*     name;
} app_import_entry;

typedef struct {
	uintptr_t offset;
	char*     name;
} app_export_entry;

typedef struct {
	uintptr_t          import_count;
	app_import_entry** import_data;
	uintptr_t          export_count;
	app_export_entry** export_data;
	uintptr_t          bin_size;
	void*              bin_data;
	uintptr_t          bin_entry;
	uintptr_t          bin_bss;
} app;



extern app* app_create(void* inData, uintptr_t inSize, uintptr_t inEntry, uintptr_t inBSS);
extern void app_delete(app* inApp);

extern bool app_import_add(app* inApp, char* inName, uintptr_t inOffset);
extern bool app_export_add(app* inApp, char* inName, uintptr_t inOffset);

extern bool app_save(app* inApp, const char* inPath);

#endif
