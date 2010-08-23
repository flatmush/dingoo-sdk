#ifndef _BALL_H_
#define _BALL_H_

#include <SDL.h>
#include <SDL_mixer.h>
#include "global.h"
#include "graphics.h"

class Ball {
	int x;
	int y;
	int size;
	int speed;
	int preStartTimer;
	int nextSpeedTimer;
	int color;
	int dirX;
	int dirY;
	Mix_Chunk* sound;
public:
	Ball();

	void init();
	void reset();
	void update(int player1Y, int player2Y, int paddleWidth, int paddleHeight, int paddleOffset);
	void draw();
	int hasPlayerWon();
	void setSound(Mix_Chunk* sound);
	bool collidesWithPaddle(int px, int py, int pw, int ph);
};

#endif
