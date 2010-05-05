#ifndef __sml_sprite_h__
#define __sml_sprite_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <sml/graphics.h>

typedef struct {
	uint16_t   width, height;
	uint16_t   frame_count;
	uint16_t** frame_data;
} sprite;

extern sprite* sprite_create(uint16_t inWidth, uint16_t inHeight);
extern void    sprite_delete(sprite* inSprite);

extern sprite* sprite_frame_add(sprite* inSprite, uint16_t* inData);
extern sprite* sprite_frame_add_bitmap(sprite* inSprite, uint16_t* inBitmap, gfx_color inKey);

extern sprite* sprite_load(const char* inPath);
extern sprite* sprite_load_from_buffer(uint8_t* buffer, size_t size);
extern bool    sprite_save(sprite* inSprite, const char* inPath);
extern sprite* sprite_load_from_tga(const char* inPath, gfx_color inKey);
extern sprite* sprite_load_from_tga_buffer(uint8_t* tgaFileData, size_t bufferSize, gfx_color inKey);

extern void    sprite_draw(sprite* inSprite, int16_t inX, int16_t inY, uint16_t inFrame);

#ifdef __cplusplus
}
#endif

#endif
