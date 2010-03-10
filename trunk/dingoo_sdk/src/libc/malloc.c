#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define _VALLOC_PAGE_BOUNDRY 4096

#undef malloc
#undef realloc
#undef free



typedef struct {
	void* ptr[256];
	void* next;
} _alloc_block;

_alloc_block* _alloc_list = NULL;

inline uint8_t _alloc_block_hash(void* inPtr) {
	uintptr_t tempPtr = (uintptr_t)inPtr;
	tempPtr ^= (tempPtr >> 8);
	tempPtr ^= (tempPtr >> 16);
	tempPtr ^= (tempPtr >> 24);
	return (uint8_t)(tempPtr & 0xFF);
}

_alloc_block* _alloc_block_create() {
	_alloc_block* tempBlock = (_alloc_block*)malloc(sizeof(_alloc_block));
	if(tempBlock == NULL)
		return NULL;
	uintptr_t i;
	for(i = 0; i < 256; i++)
		tempBlock->ptr[i] = NULL;
	tempBlock->next = NULL;
	return tempBlock;
}

bool _alloc_block_add(_alloc_block* inBlock, void* inPtr) {
	if(inBlock == NULL)
		return false;

	uint8_t tempHash = _alloc_block_hash(inPtr);
	if(inBlock->ptr[tempHash] == NULL) {
		inBlock->ptr[tempHash] = inPtr;
		return true;
	}

	if(inBlock->next == NULL)
		inBlock->next = _alloc_block_create();
	return _alloc_block_add(inBlock->next, inPtr);
}

void** _alloc_block_find(_alloc_block* inBlock, void* inPtr) {
	if(inBlock == NULL)
		return NULL;
	uint8_t tempHash = _alloc_block_hash(inPtr);
	if(inBlock->ptr[tempHash] == inPtr)
		return &(inBlock->ptr[tempHash]);
	if(inBlock->next != NULL)
		return _alloc_block_find(inBlock->next, inPtr);
	return NULL;
}

bool _alloc_block_delete(_alloc_block* inBlock, void* inPtr) {
	void** tempFind = _alloc_block_find(inBlock, inPtr);
	if(tempFind == NULL)
		return false;
	*tempFind = NULL;
	return true;
}

void _alloc_block_cleanup(_alloc_block* inBlock) {
	if(inBlock == NULL)
		return;
	_alloc_block_cleanup(inBlock->next);
	uintptr_t i;
	for(i = 0; i < 256; i++)
		free(inBlock->ptr[i]);
	free(inBlock);
}



typedef struct {
	void* align;
	void* real;
	uintptr_t bound;
	uintptr_t size;
} _align_block_entry;

typedef struct {
	_align_block_entry entry[256];
	void* next;
} _align_block;

_align_block* _align_list = NULL;

_align_block* _align_block_create() {
	_align_block* tempBlock = (_align_block*)malloc(sizeof(_align_block));
	if(tempBlock == NULL)
		return NULL;
	uintptr_t i;
	for(i = 0; i < 256; i++)
		tempBlock->entry[i].align = NULL;
	tempBlock->next = NULL;
	return tempBlock;
}

bool _align_block_add(_align_block* inBlock, void* inAlign, void* inReal, uintptr_t inBoundary, uintptr_t inSize) {
	if(inBlock == NULL)
		return false;

	uint8_t tempHash = _alloc_block_hash(inAlign);
	if(inBlock->entry[tempHash].align == NULL) {
		inBlock->entry[tempHash].align = inAlign;
		inBlock->entry[tempHash].real  = inReal;
		inBlock->entry[tempHash].bound = inBoundary;
		inBlock->entry[tempHash].size  = inSize;
		return true;
	}

	if(inBlock->next == NULL)
		inBlock->next = _align_block_create();
	return _align_block_add(inBlock->next, inAlign, inReal, inBoundary, inSize);
}

_align_block_entry* _align_block_find(_align_block* inBlock, void* inAlign) {
	if(inBlock == NULL)
		return NULL;
	uint8_t tempHash = _alloc_block_hash(inAlign);
	if(inBlock->entry[tempHash].align == inAlign)
		return &(inBlock->entry[tempHash]);
	if(inBlock->next != NULL)
		return _align_block_find(inBlock->next, inAlign);
	return NULL;
}

void _align_block_cleanup(_align_block* inBlock) {
	if(inBlock == NULL)
		return;
	_align_block_cleanup(inBlock->next);
	uintptr_t i;
	for(i = 0; i < 256; i++)
		free(inBlock->entry[i].real);
	free(inBlock);
}



void* _malloc(size_t size) {
	void* tempAlloc = malloc(size);
	if(tempAlloc == NULL)
		return NULL;
	if(_alloc_block_add(_alloc_list, tempAlloc))
		return tempAlloc;
	free(tempAlloc);
	return NULL;
}

void* _realloc(void* ptr, size_t size) {
	void* tempAlloc;

	_align_block_entry* tempAlign = _align_block_find(_align_list, ptr);
	if(tempAlign != NULL) {
		tempAlloc = memalign(tempAlign->bound, size);
		if(tempAlloc == NULL)
			return NULL;
		memcpy(tempAlloc, ptr, tempAlign->size);
		_free(ptr);
		return tempAlloc;
	}

	tempAlloc = realloc(ptr, size);
	if(tempAlloc == NULL)
		return NULL;
	if(tempAlloc == ptr)
		return tempAlloc;

	_alloc_block_delete(_alloc_list, ptr);
	_alloc_block_add(_alloc_list, tempAlloc);
	return tempAlloc;
}

void _free(void* ptr) {
	if(ptr == NULL)
		return;

	_align_block_entry* tempAlign = _align_block_find(_align_list, ptr);
	if(tempAlign != NULL) {
		free(tempAlign);
		tempAlign->align = NULL;
		return;
	}

	free(ptr);
	_alloc_block_delete(_alloc_list, ptr);
}



void* calloc(size_t num, size_t size) {
	unsigned char* tempOut = (unsigned char*)_malloc(num * size);
	if(tempOut == NULL)
		return NULL;
	memset(tempOut, 0x00, (num * size));
	return (void*)tempOut;
}

void* memalign(size_t align, size_t size) {
	if((align == 0) || ((align & (align -1)) != 0)) {
		errno = EINVAL;
		return NULL;
	}

	void* tempOut = _malloc(size);
	if(tempOut == NULL)
		return NULL;
	if(((uintptr_t)tempOut & (align - 1)) == 0)
		return tempOut;
	_free(tempOut);

	void* tempAlloc = malloc(size + (align - 1));
	if(tempAlloc == NULL)
		return NULL;
	tempOut = (void*)(((uintptr_t)tempAlloc + (align - 1)) & (align - 1));
	if(_align_block_add(_align_list, tempOut, tempAlloc, align, size))
		return tempOut;
	free(tempAlloc);

	return NULL;
}

void* valloc(size_t size) {
	return memalign(_VALLOC_PAGE_BOUNDRY, size); // TODO - Work out actual page boundary.
}



bool _memsys_init() {
	_alloc_list = _alloc_block_create();
	if(_alloc_list == NULL)
		return false;
	_align_list = _align_block_create();
	if(_align_list == NULL) {
		free(_alloc_list);
		return false;
	}
	return true;
}

void _memsys_cleanup() {
	_alloc_block_cleanup(_alloc_list);
	_align_block_cleanup(_align_list);
}
