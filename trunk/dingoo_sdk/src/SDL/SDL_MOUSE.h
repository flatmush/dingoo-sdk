#ifndef __SDL_MOUSE_h__
#define __SDL_MOUSE_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include "SDL_TYPES.h"

typedef struct {

} SDL_Cursor;

extern void        SDL_WarpMouse(Uint16 x, Uint16 y);
extern SDL_Cursor* SDL_CreateCursor(Uint8* data, Uint8* mask, int w, int h, int hot_x, int hot_y);
extern void        SDL_FreeCursor(SDL_Cursor* cursor);
extern void*       SDL_SetCursor(SDL_Cursor* cursor);
extern SDL_Cursor* SDL_GetCursor();
extern int         SDL_ShowCursor(int toggle);

#ifdef __cplusplus
}
#endif

#endif
