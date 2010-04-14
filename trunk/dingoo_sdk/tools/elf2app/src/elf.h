#ifndef __elf_h__
#define __elf_h__

#include <stdbool.h>
#include <stdint.h>



typedef struct {
	char*    name;
	uint32_t offset;
} elf_symbol_entry;

typedef struct {
	uint32_t           size;
	elf_symbol_entry** data;
} elf_symbol_table;



extern elf_symbol_table* elf_symbol_table_create();
extern void              elf_symbol_table_delete(elf_symbol_table* inTable);

extern bool              elf_symbol_table_add(elf_symbol_table* inTable, elf_symbol_entry* inEntry);

extern elf_symbol_entry* elf_symbol_entry_parse(const char* inLine);

extern elf_symbol_table* elf_symbol_table_read(const char* inPath);

extern uint32_t          elf_symbol_table_find_offset(elf_symbol_table* inTable, const char* inName);

extern uint32_t          elf_bss_size(const char* inPath);

#endif
