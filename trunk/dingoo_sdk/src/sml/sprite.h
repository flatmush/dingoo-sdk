#ifndef __sprite_h__
#define __sprite_h__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint16_t   width, height;
	uint16_t   frame_count;
	uint16_t** frame_data;
} sprite;

extern sprite* sprite_create(uint16_t inWidth, uint16_t inHeight);
extern void    sprite_delete(sprite* inSprite);

extern sprite* sprite_frame_add(sprite* inSprite, uint16_t* inData);
extern sprite* sprite_frame_add_bitmap(sprite* inSprite, uint16_t* inBitmap, uint16_t inKey);

extern sprite* sprite_load(const char* inPath);
extern bool    sprite_save(sprite* inSprite, const char* inPath);

extern void    sprite_draw(sprite* inSprite, int16_t inX, int16_t inY, uint16_t inFrame);

#endif
