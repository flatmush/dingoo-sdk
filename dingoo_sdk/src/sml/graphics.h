#ifndef __sml_graphics_h__
#define __sml_graphics_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <sml/display.h>

#define SML_COLOR_32BIT

#ifdef SML_COLOR_32BIT
typedef uint32_t gfx_color;

static const gfx_color COLOR_RED     = 0x00FF0000;
static const gfx_color COLOR_GREEN   = 0x0000FF00;
static const gfx_color COLOR_BLUE    = 0x000000FF;
static const gfx_color COLOR_YELLOW  = 0x00FFFF00;
static const gfx_color COLOR_MAGENTA = 0x00FF00FF;
static const gfx_color COLOR_TEAL    = 0x0000FFFF;
static const gfx_color COLOR_BLACK   = 0x00000000;
static const gfx_color COLOR_WHITE   = 0x00FFFFFF;
#else
typedef uint16_t gfx_color;

static const gfx_color COLOR_RED     = 0xf800;
static const gfx_color COLOR_GREEN   = 0x07e0;
static const gfx_color COLOR_BLUE    = 0x001f;
static const gfx_color COLOR_YELLOW  = 0xffe0;
static const gfx_color COLOR_MAGENTA = 0xf81f;
static const gfx_color COLOR_TEAL    = 0x07ff;
static const gfx_color COLOR_BLACK   = 0x0000;
static const gfx_color COLOR_WHITE   = 0xffff;
#endif

extern inline gfx_color gfx_color_rgb(uint8_t inRed, uint8_t inGreen, uint8_t inBlue);
extern inline gfx_color gfx_color_rand(uint8_t inRed, uint8_t inGreen, uint8_t inBlue);



typedef struct {
	void*         address;
	uint16_t      width, height;
} gfx_texture;

typedef struct {
	gfx_texture*  texture;
	gfx_color     colorKey;
	bool          colorize;
	gfx_color     fontColor;
} gfx_font;

extern gfx_texture* gfx_render_target;



extern bool         gfx_init(display* inDisplay);
extern void         gfx_term();

extern bool         gfx_render_target_set(gfx_texture* inTexture);
extern void         gfx_render_target_clear(gfx_color inColor);
extern void         gfx_render_target_swap();

extern gfx_texture* gfx_tex_allocate(uint32_t width, uint32_t height);
extern gfx_texture* gfx_tex_load_tga(const char* inPath);
extern gfx_texture* gfx_tex_load_tga_from_buffer(uint8_t* buffer, size_t size);
extern bool         gfx_tex_save_tga(const char* inPath, gfx_texture* inTexture);
extern void         gfx_tex_delete(gfx_texture* inTexture);

extern void         gfx_point_draw(int16_t inX, int16_t inY, gfx_color inColor);
extern void         gfx_line_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, gfx_color inColor);
extern void         gfx_rect_draw(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, gfx_color inColor);
extern void         gfx_rect_fill_draw(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, gfx_color inColor);
extern void         gfx_tri_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, int16_t inX2, int16_t inY2, gfx_color inColor);
extern void         gfx_tri_fill_draw(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, int16_t inX2, int16_t inY2, gfx_color inColor);

extern void         gfx_tex_draw(int16_t inX, int16_t inY, gfx_texture* inTexture);

extern gfx_font*    gfx_font_load(const char* inPath, gfx_color inKey);
extern gfx_font*    gfx_font_load_from_buffer(uint8_t* buffer, size_t size, gfx_color inKey);
extern void         gfx_font_delete(gfx_font* inFont);

extern void         gfx_font_color_set(gfx_font* inFont, gfx_color inColor);
extern void         gfx_font_color_clear(gfx_font* inFont);

extern uint16_t     gfx_font_width(gfx_font* inFont, char* inString);
extern uint16_t     gfx_font_height(gfx_font* inFont);
extern void         gfx_font_print_char(int16_t inX, int16_t inY, gfx_font* inFont, char inChar);
extern void         gfx_font_print(int16_t inX, int16_t inY, gfx_font* inFont, char* inString);
extern void         gfx_font_print_center(int16_t inY, gfx_font* inFont, char* inString);
extern void         gfx_font_print_center_ex(int16_t inY, int16_t areaWidth, int16_t offsetX, gfx_font* inFont, char* inString);
extern void         gfx_font_print_fromright(int16_t inX, int16_t inY, gfx_font* inFont, char* inString);

#ifdef __cplusplus
}
#endif

#endif
