#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// TODO: Make it accept multiple tga files, and build a multi-frame sprite.

#define _min(x, y) ((x) < (y) ? (x) : (y))

typedef uint16_t gfx_color;
inline gfx_color gfx_color_rgb(uint8_t inRed, uint8_t inGreen, uint8_t inBlue) {
		return ((inRed & 0xF8) << 8) | ((inGreen & 0xFC) << 3) | ((inBlue & 0xF8) >> 3);
}

inline gfx_color gfx_color_rand(uint8_t inRed, uint8_t inGreen, uint8_t inBlue) {
	return gfx_color_rgb((inRed + (rand() % (256 - inRed))), (inGreen + (rand() % (256 - inGreen))), (inBlue + (rand() % (256 - inBlue))));
}

typedef struct {
	void*         address;
	uint16_t      width, height;
} gfx_texture;

typedef struct {
	uint16_t   width, height;
	uint16_t   frame_count;
	uint16_t** frame_data;
} sprite;

gfx_texture* gfx_tex_load_tga(const char* inPath) {
	if(inPath == NULL)
		return NULL;

	FILE* tempFile = fopen(inPath, "rb");
	if(tempFile == NULL)
		return NULL;

	uint8_t  tga_ident_size;
	uint8_t  tga_color_map_type;
	uint8_t  tga_image_type;
	uint16_t tga_color_map_start;
	uint16_t tga_color_map_length;
	uint8_t  tga_color_map_bpp;
	uint16_t tga_origin_x;
	uint16_t tga_origin_y;
	uint16_t tga_width;
	uint16_t tga_height;
	uint8_t  tga_bpp;
	uint8_t  tga_descriptor;

	fread(&tga_ident_size, 1, 1, tempFile);
	fread(&tga_color_map_type, 1, 1, tempFile);
	fread(&tga_image_type, 1, 1, tempFile);
	fread(&tga_color_map_start, 2, 1, tempFile);
	fread(&tga_color_map_length, 2, 1, tempFile);
	fread(&tga_color_map_bpp, 1, 1, tempFile);
	fread(&tga_origin_x, 2, 1, tempFile);
	fread(&tga_origin_y, 2, 1, tempFile);
	fread(&tga_width, 2, 1, tempFile);
	fread(&tga_height, 2, 1, tempFile);
	fread(&tga_bpp, 1, 1, tempFile);
	fread(&tga_descriptor, 1, 1, tempFile);

	bool upside_down = (tga_descriptor & 0x20) > 0;

	gfx_texture* tempTexture = (gfx_texture*)malloc(sizeof(gfx_texture) + (tga_width * tga_height * 2));
	if(tempTexture == NULL) {
		fclose(tempFile);
		return NULL;
	}
	tempTexture->address = (void*)((uintptr_t)tempTexture + sizeof(gfx_texture));
	tempTexture->width = tga_width;
	tempTexture->height = tga_height;

	uintptr_t i;
	uint8_t tempColor[3];
	uint16_t* tempTexPtr = tempTexture->address;
	for(i = 0; i < (tga_width * tga_height); i++) {
		fread(&tempColor[2], 1, 1, tempFile);
		fread(&tempColor[1], 1, 1, tempFile);
		fread(&tempColor[0], 1, 1, tempFile);
		tempTexPtr[upside_down ? i : ((tga_height - 1 - (i / tga_width)) * tga_width + i % tga_width)] = gfx_color_rgb(tempColor[0], tempColor[1], tempColor[2]);
	}
	fclose(tempFile);

	return tempTexture;
}

sprite* sprite_create(uint16_t inWidth, uint16_t inHeight) {
	sprite* tempSprite = (sprite*)malloc(sizeof(sprite));
	if(tempSprite == NULL)
		return NULL;

	tempSprite->width       = inWidth;
	tempSprite->height      = inHeight;
	tempSprite->frame_count = 0;
	tempSprite->frame_data  = (uint16_t**)((uintptr_t)tempSprite + sizeof(sprite));

	return tempSprite;
}

void sprite_delete(sprite* inSprite) {
	if(inSprite == NULL)
		return;
	free(inSprite);
}

sprite* sprite_frame_add(sprite* inSprite, uint16_t* inData) {
	if((inSprite == NULL) || (inData == NULL))
		return false;
	sprite* tempSprite = (sprite*)realloc(inSprite, (sizeof(sprite) + ((inSprite->frame_count + 1) << 2)));
	if(tempSprite == NULL)
		return NULL;
	tempSprite->frame_data = (uint16_t**)((uintptr_t)tempSprite + sizeof(sprite));
	tempSprite->frame_data[tempSprite->frame_count] = inData;
	tempSprite->frame_count += 1;
	return tempSprite;
}

uint16_t _rle_chunk(uint16_t* inBitmap, uint16_t inKey, uint32_t tempMax) {
	uint16_t i = 0;
	if(*inBitmap == inKey) {
		for(i = 1; (i < tempMax) && (inBitmap[i] == inKey); i++);
		return i;
	}
	for(i = 1; (i < tempMax) && (inBitmap[i] != inKey); i++);
	return (0x8000 | i);
}

sprite* sprite_frame_add_bitmap(sprite* inSprite, uint16_t* inBitmap, uint16_t inKey) {
	if((inSprite == NULL) || (inBitmap == NULL))
		return NULL;

	uint16_t* tempFrame = (uint16_t*)malloc(inSprite->width * inSprite->height * 4);
	if(tempFrame == NULL)
		return NULL;

	uint16_t* tempFramePtr  = tempFrame;
	uint16_t* tempBitmapPtr = inBitmap;
	uint16_t* tempBitmapEnd = &inBitmap[inSprite->width * inSprite->height];
	uint32_t  tempChunk;
	while(tempBitmapPtr < tempBitmapEnd) {
		tempChunk = _rle_chunk(tempBitmapPtr, inKey, _min(32767, (((uintptr_t)tempBitmapEnd - (uintptr_t)tempBitmapPtr) >> 1)));
		*(tempFramePtr++) = tempChunk;
		if((tempChunk & 0x8000) != 0) {
			memcpy(tempFramePtr, tempBitmapPtr, ((tempChunk & 0x7FFF) << 1));
			tempFramePtr += (tempChunk & 0x7FFF);
		}
		tempBitmapPtr += (tempChunk & 0x7FFF);
	}

	uintptr_t tempFrameLen = ((uintptr_t)tempFramePtr - (uintptr_t)tempFrame);
	uint16_t* tempFrameAlloc = (uint16_t*)realloc(tempFrame, tempFrameLen);
	if(tempFrameAlloc != NULL)
		tempFrame = tempFrameAlloc;

	sprite* tempOut = sprite_frame_add(inSprite, tempFrame);
	if(tempOut == NULL)
		free(tempFrame);
	return tempOut;
}

uintptr_t sprite_frame_len(sprite* inSprite, uint16_t inFrame) {
	if(inSprite == NULL)
		return 0;
	if(inFrame >= inSprite->frame_count)
		return 0;

	uint16_t* tempPtr = inSprite->frame_data[inFrame];
	uintptr_t tempLen, tempSize, tempChunkSize;
	uintptr_t tempSizeMax = (inSprite->width * inSprite->height);
	for(tempSize = 0, tempLen = 0; tempSize < tempSizeMax; tempLen++) {
		tempChunkSize = (tempPtr[tempLen] & 0x7FFF);
		tempSize += tempChunkSize;
		if((tempPtr[tempLen] & 0x8000) != 0)
			tempLen += tempChunkSize;
	}
	return tempLen;
}

bool sprite_save(sprite* inSprite, const char* inPath) {
	if((inSprite == NULL) || (inPath == NULL))
		return false;

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	char tempMagic[8] = { 'd', 's', 'p', 'r', 'i', 't', 'e', '\0' };
	uint16_t tempPad = 0;
	fwrite(tempMagic, 1, 8, tempFile);
	fwrite(&(inSprite->width), 2, 1, tempFile);
	fwrite(&(inSprite->height), 2, 1, tempFile);
	fwrite(&(inSprite->frame_count), 2, 1, tempFile);
	fwrite(&tempPad, 2, 1, tempFile);

	uintptr_t i;
	uint32_t tempFrameLen;
	for(i = 0; i < inSprite->frame_count; i++) {
		tempFrameLen = sprite_frame_len(inSprite, i);
		fwrite(&tempFrameLen, 4, 1, tempFile);
		fwrite(inSprite->frame_data[i], 2, tempFrameLen, tempFile);
	}

	fclose(tempFile);
	return true;
}

int main(int argc, char** argv) {
	if (argc <= 2) {
		printf("Usage: tga2sprite <outfile.spt> <infile1.tga> [infile2.tga ...]\n");
		return 1;
	}

	int nbrOfFiles = argc - 2;

	gfx_texture** files = malloc(sizeof(gfx_texture*) * nbrOfFiles);

	int i;
	for (i = 0; i < nbrOfFiles; i++)
	{
		files[i] = gfx_tex_load_tga(argv[2 + i]);

		if (files[i] == NULL) {
			printf("Unable to load file %s.\n", argv[2 + i]);
			return 1;
		}
	}

	int w = files[0]->width;
	int h = files[0]->height;

	for (i = 1; i < nbrOfFiles; i++) {
		if (files[i]->width != w || files[i]->height != h) {
			printf("All images must have the same width and height.\n");
			return 1;
		}
	}
	
	sprite* tempSprite;
	
	tempSprite = sprite_create(w, h);
	if (tempSprite == NULL) {
		printf("Could not allocate memory.\n");
		return 1;
	}

	for (i = 0; i < nbrOfFiles; i++) {
		tempSprite = sprite_frame_add_bitmap(tempSprite, files[i]->address, gfx_color_rgb(0xFF, 0x00, 0xFF));
	}

	if (!sprite_save(tempSprite, argv[1])) {
		printf("Error while saving file.\n");
		return 2;
	}

	printf("Saved as %s\n", argv[1]);
	return 0;
}
