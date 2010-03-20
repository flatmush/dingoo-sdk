#ifndef __graphics_h__
#define __graphics_h__

#include <stdint.h>
#include <stdbool.h>
#include <sml/display.h>

typedef uint16_t gfx_color;
extern inline gfx_color gfx_color_rgb(uint8_t inRed, uint8_t inGreen, uint8_t inBlue);
extern inline gfx_color gfx_color_rand(uint8_t inRed, uint8_t inGreen, uint8_t inBlue);

typedef struct {
	void*         address;
	uint16_t      width, height;
} gfx_texture;

extern gfx_texture* gfx_render_target;



extern bool         gfx_init(display* inDisplay);
extern void         gfx_term();

extern bool         gfx_render_target_set(gfx_texture* inTexture);
extern void         gfx_render_target_clear(gfx_color inColor);
extern void         gfx_render_target_swap();

extern gfx_texture* gfx_tex_load_tga(const char* inPath);
extern gfx_texture* gfx_tex_load_tga_from_buffer(uint8_t* buffer);
extern bool         gfx_tex_save_tga(const char* inPath, gfx_texture* inTexture);

extern void         gfx_point_draw(int16_t inX, int16_t inY, gfx_color inColor);
extern void         gfx_line_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, gfx_color inColor);
extern void         gfx_rect_draw(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, gfx_color inColor);
extern void         gfx_rect_fill_draw(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, gfx_color inColor);
extern void         gfx_tri_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, int16_t inX2, int16_t inY2, gfx_color inColor);
extern void         gfx_tri_fill_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, int16_t inX2, int16_t inY2, gfx_color inColor);

extern void         gfx_tex_draw(int16_t inX, int16_t inY, gfx_texture* inTexture);

extern uint16_t     gfx_font_width(gfx_texture* inFont, char* inString);
extern uint16_t     gfx_font_height(gfx_texture* inFont);
extern void         gfx_font_print_char(int16_t inX, int16_t inY, gfx_texture* inFont, char inChar);
extern void         gfx_font_print(int16_t inX, int16_t inY, gfx_texture* inFont, char* inString);
extern void         gfx_font_print_center(int16_t inY, gfx_texture* inFont, char* inString);
extern void         gfx_font_print_fromright(int16_t inX, int16_t inY, gfx_texture* inFont, char* inString);

#endif
