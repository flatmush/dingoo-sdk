#include "graphics.h"

SDL_Surface *screen = NULL;

SDL_Surface* loadImage(char* filename)
{
    SDL_Surface* temp = IMG_Load(filename);
	if (temp == NULL)
		return NULL;

	SDL_Surface* ret = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    return ret;
}

void drawSurface(int x, int y, SDL_Surface* source, SDL_Surface* dest)
{
	SDL_Rect coords;
	coords.x = x;
	coords.y = y;

	SDL_BlitSurface(source, NULL, dest, &coords);
}

// This function is here because it's important to free the old surface,
// when you generate new text, or your game will run out of memory and crash.
SDL_Surface* renderText(SDL_Surface* surface, TTF_Font* font, char* text, SDL_Color &color)
{
	if (surface != NULL)
		SDL_FreeSurface(surface);

	surface = TTF_RenderText_Solid(font, text, color);

	return surface;
}

void fillRect(int x, int y, int w, int h, int color) {
	SDL_Rect rect = {x,y,w,h};
	SDL_FillRect(screen, &rect, color);
}
