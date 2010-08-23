#ifndef _GAME_H_
#define _GAME_H_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "global.h"
#include "graphics.h"
#include "ball.h"
#include "controls.h"

// If you want to access the sound driver's volume control
extern "C" {
	extern void dingooSoundVolumeIncrease();
	extern void dingooSoundVolumeDecrease();
}

enum GameState { STATE_SPLASH, STATE_GAME };

#define PADDLE_WIDTH 15
#define PADDLE_HEIGHT 50
#define PADDLE_OFFSET 5

class Game {
	Mix_Music* music;
	Mix_Chunk* soundBounce;
	SDL_Surface* splash;
	TTF_Font *bigFont;
	TTF_Font *smallFont;
	SDL_Surface* textTitle;
	SDL_Surface* textSubTitle;
	SDL_Surface* textStart;
	SDL_Surface* textScore;
	SDL_Surface* textPaused;
	SDL_Color colorWhite;
	Uint32 colorIntWhite;
	Uint32 colorIntBlack;
	GameState state;
	int tempTime;
	int seconds;
	int player1Y;
	int player2Y;
	int player1Score;
	int player2Score;
	bool paused;
	Ball ball;

	void updateSplash();
	void drawSplash();
	void updateGame();
	void drawGame();
	void startNewGame();
	void playerScored(int player);
public:
	bool loadGame();
	void unloadGame();
	void update();
	void draw();
};

#endif
