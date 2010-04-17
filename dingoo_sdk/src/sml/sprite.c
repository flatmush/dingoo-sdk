#include <sml/sprite.h>
#include <sml/graphics.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _min(x, y) ((x) < (y) ? (x) : (y))



sprite* sprite_create(uint16_t inWidth, uint16_t inHeight) {
	sprite* tempSprite = (sprite*)malloc(sizeof(sprite));
	if(tempSprite == NULL)
		return NULL;

	tempSprite->width       = inWidth;
	tempSprite->height      = inHeight;
	tempSprite->frame_count = 0;
	tempSprite->frame_data  = NULL;

	return tempSprite;
}

void sprite_delete(sprite* inSprite) {
	if(inSprite == NULL)
		return;

	if(inSprite->frame_data != NULL) {
		uintptr_t i;
		for(i = 0; i < inSprite->frame_count; i++)
			free(inSprite->frame_data[i]);
	}

	free(inSprite);
}



sprite* sprite_frame_add(sprite* inSprite, uint16_t* inData) {
	if((inSprite == NULL) || (inData == NULL))
		return false;
	sprite* tempSprite = (sprite*)realloc(inSprite, (sizeof(sprite) + ((inSprite->frame_count + 1) * sizeof(uint16_t*))));
	if(tempSprite == NULL)
		return NULL;
	tempSprite->frame_data = (uint16_t**)((uintptr_t)tempSprite + sizeof(sprite));
	tempSprite->frame_data[tempSprite->frame_count] = inData;
	tempSprite->frame_count += 1;
	return tempSprite;
}

uint16_t _rle_chunk(uint16_t* inBitmap, gfx_color inKey, uint32_t tempMax) {
	uint16_t i = 0;
	if(*inBitmap == inKey) {
		for(i = 1; (i < tempMax) && (inBitmap[i] == inKey); i++);
		return i;
	}
	for(i = 1; (i < tempMax) && (inBitmap[i] != inKey); i++);
	return (0x8000 | i);
}

sprite* sprite_frame_add_bitmap(sprite* inSprite, uint16_t* inBitmap, gfx_color inKey) {
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



sprite* _sprite_load(FILE* tempFile) {
	if(tempFile == NULL)
		return NULL;

	char tempMagic[8];
	fread(tempMagic, 1, 8, tempFile);
	if(strcmp(tempMagic, "dsprite") != 0) {
		fclose(tempFile);
		return NULL;
	}

	uint16_t tempWidth, tempHeight, tempFrames;
	fread(&tempWidth, 2, 1, tempFile);
	fread(&tempHeight, 2, 1, tempFile);
	fread(&tempFrames, 2, 1, tempFile);

	fseek(tempFile, 2, SEEK_CUR);

	sprite* tempSprite = sprite_create(tempWidth, tempHeight);
	if(tempSprite == NULL) {
		fclose(tempFile);
		return NULL;
	}

	uintptr_t i;
	uint32_t  tempFrameLen;
	uint16_t* tempFrame;
	sprite* tempSpriteAdd;
	for(i = 0; i < tempFrames; i++) {
		fread(&tempFrameLen, 4, 1, tempFile);
		tempFrame = (uint16_t*)malloc(tempFrameLen << 1);
		if(tempFrame == NULL) {
			sprite_delete(tempSprite);
			fclose(tempFile);
			return NULL;
		}
		if(fread(tempFrame, 2, tempFrameLen, tempFile) != (int)tempFrameLen) {
			free(tempFrame);
			sprite_delete(tempSprite);
			fclose(tempFile);
			return NULL;
		}
		tempSpriteAdd = sprite_frame_add(tempSprite, tempFrame);
		if(tempSpriteAdd == NULL) {
			free(tempFrame);
			sprite_delete(tempSprite);
			fclose(tempFile);
			return NULL;
		}
		tempSprite = tempSpriteAdd;
	}

	fclose(tempFile);

	return tempSprite;
}

sprite* sprite_load(const char* inPath) {
	if(inPath == NULL)
		return NULL;

	FILE* tempFile = fopen(inPath, "rb");
	return _sprite_load(tempFile);
}

sprite* sprite_load_from_buffer(uint8_t* buffer, size_t size) {
	if(buffer == NULL)
		return NULL;

	FILE* tempFile = fmemopen(buffer, size, "rb");
	return _sprite_load(tempFile);
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

sprite* sprite_load_from_tga(const char* inPath, gfx_color inKey) {
	gfx_texture* tempTexture = gfx_tex_load_tga(inPath);

	if (tempTexture == NULL)
		return NULL;

	sprite* tempSprite = sprite_create(tempTexture->width, tempTexture->height);

	if (tempSprite == NULL) {
		free(tempTexture);
		return NULL;
	}

	tempSprite = sprite_frame_add_bitmap(tempSprite, tempTexture->address, inKey);

	free(tempTexture);
	return tempSprite;
}

sprite* sprite_load_from_tga_buffer(uint8_t* tgaFileData, size_t bufferSize, gfx_color inKey) {
	gfx_texture* tempTexture = gfx_tex_load_tga_from_buffer(tgaFileData, bufferSize);

	if (tempTexture == NULL)
		return NULL;

	sprite* tempSprite = sprite_create(tempTexture->width, tempTexture->height);

	if (tempSprite == NULL) {
		free(tempTexture);
		return NULL;
	}

	tempSprite = sprite_frame_add_bitmap(tempSprite, tempTexture->address, inKey);

	free(tempTexture);
	return tempSprite;
}



void sprite_draw(sprite* inSprite, int16_t inX, int16_t inY, uint16_t inFrame) {
	if(inSprite == NULL)
		return;
	if(inFrame >= inSprite->frame_count)
		return;

	if((inX >= gfx_render_target->width) || (inY >= gfx_render_target->height))
		return;
	if(((inX + inSprite->width) < 0) || ((inY + inSprite->height) < 0))
		return;

	uint16_t* tempPtr = inSprite->frame_data[inFrame];
	uint16_t tempX, tempY;
	uintptr_t i, tempLen, tempChunkSize;
	for(tempLen = 0, tempX = 0, tempY = 0; tempY < inSprite->height;) {
		tempChunkSize = (tempPtr[tempLen] & 0x7FFF);
		if((tempPtr[tempLen] & 0x8000) != 0) {
			tempLen++;
			for(i = 0; i < tempChunkSize; i++) {
				gfx_point_draw((inX + tempX), (inY + tempY), (gfx_color)tempPtr[tempLen]);
				tempLen++;
				tempX++;
				if(tempX >= inSprite->width) {
					tempX = 0;
					tempY++;
				}
			}
			continue;
		}
		tempLen++;
		tempX += tempChunkSize;
		while(tempX >= inSprite->width) {
			tempX -= inSprite->width;
			tempY++;
		}
	}
}
