#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "global.h"
#include "game.h"
#include "graphics.h"
#include "controls.h"

bool SDLIsInit = false;
bool TTFIsInit = false;
bool IMGIsInit = false;
bool MIXIsInit = false;
bool MIXIsOpen = false;

SDL_Event event;
Game game;

bool quit = false;

bool init()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
        return false;

	SDLIsInit = true;

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

    if (screen == NULL)
        return false;

    if (TTF_Init() < 0)
        return false;

	TTFIsInit = true;

	if (IMG_Init(IMG_INIT_PNG) < 0)
		return false;

	IMGIsInit = true;

	if (Mix_Init(MIX_INIT_MOD | MIX_INIT_OGG) < 0)
		return false;

	MIXIsInit = true;

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0)
		return false;

	MIXIsOpen = true;

    return true;
}

void cleanup()
{
	if (MIXIsOpen)
		Mix_CloseAudio();

	if (MIXIsInit)
		Mix_Quit();

	if (IMGIsInit)
		IMG_Quit();

	if (TTFIsInit)
		TTF_Quit();

	if (screen != NULL)
		SDL_FreeSurface(screen);

	if (SDLIsInit)
		SDL_Quit();
}

void quitGame()
{
	quit = true;
}

int main( int argc, char* argv[] )
{
    if(!init())
    {
		cleanup();
        return 1;
    }

	int currentTime = SDL_GetTicks();
	double accumulator = 0;
	double frameTime = 1000.0/(double)FPS;
	int newTime;
	int deltaTime;

	for (int i = 0; i < 13; i++)
	{
		controlState[i].pressed = false;
		controlState[i].changed = false;
	}

	if (!game.loadGame())
		quit = true;

	bool shouldDraw = false;

	while (!quit)
	{
		newTime = SDL_GetTicks();
		deltaTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += deltaTime;

		while (accumulator >= frameTime)
		{
			accumulator -= frameTime;
			
			// UPDATE

			for (int i = 0; i < 13; i++)
			{
				controlState[i].changed = false;
			}

			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					// Use this to quit on system events such as holding X
					// It's not a good idea to use in this game though, since player 2 uses X
					//quit = true;
				}
				else if (event.type == SDL_KEYDOWN)
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_UP: controlSetPressed(CONTROL_UP, true); break;
						case SDLK_DOWN: controlSetPressed(CONTROL_DOWN, true); break;
						case SDLK_LEFT: controlSetPressed(CONTROL_LEFT, true); break;
						case SDLK_RIGHT: controlSetPressed(CONTROL_RIGHT, true); break;
						case SDLK_LCTRL: controlSetPressed(CONTROL_A, true); break;
						case SDLK_LALT: controlSetPressed(CONTROL_B, true); break;
						case SDLK_SPACE: controlSetPressed(CONTROL_X, true); break;
						case SDLK_LSHIFT: controlSetPressed(CONTROL_Y, true); break;
						case SDLK_RETURN: controlSetPressed(CONTROL_START, true); break;
						case SDLK_ESCAPE: controlSetPressed(CONTROL_SELECT, true); break;
						case SDLK_TAB: controlSetPressed(CONTROL_L, true); break;
						case SDLK_BACKSPACE: controlSetPressed(CONTROL_R, true); break;
						case SDLK_PAUSE: controlSetPressed(CONTROL_POWER, true); break;
						default: break;
					}
				}
				else if (event.type == SDL_KEYUP)
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_UP: controlSetPressed(CONTROL_UP, false); break;
						case SDLK_DOWN: controlSetPressed(CONTROL_DOWN, false); break;
						case SDLK_LEFT: controlSetPressed(CONTROL_LEFT, false); break;
						case SDLK_RIGHT: controlSetPressed(CONTROL_RIGHT, false); break;
						case SDLK_LCTRL: controlSetPressed(CONTROL_A, false); break;
						case SDLK_LALT: controlSetPressed(CONTROL_B, false); break;
						case SDLK_SPACE: controlSetPressed(CONTROL_X, false); break;
						case SDLK_LSHIFT: controlSetPressed(CONTROL_Y, false); break;
						case SDLK_RETURN: controlSetPressed(CONTROL_START, false); break;
						case SDLK_ESCAPE: controlSetPressed(CONTROL_SELECT, false); break;
						case SDLK_TAB: controlSetPressed(CONTROL_L, false); break;
						case SDLK_BACKSPACE: controlSetPressed(CONTROL_R, false); break;
						case SDLK_PAUSE: controlSetPressed(CONTROL_POWER, false); break;
						default: break;
					}
				}
			}

			game.update();
			shouldDraw = true;
		}

		// DRAW
		if (shouldDraw)
			game.draw();
		else
			SDL_Delay(1);

		if (quit)
			game.unloadGame();
	}

    cleanup();
    return 0;
}
