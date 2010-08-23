#include "ball.h"

Ball::Ball()
{
	size = 10;
}

void Ball::init()
{
	color = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
	reset();
}

void Ball::reset()
{
	x = (SCREEN_WIDTH - size) / 2;
	y = (SCREEN_HEIGHT - size) / 2;
	speed = 3;
	preStartTimer = FPS * 3; // 3 seconds
	nextSpeedTimer = FPS * 10;

	// rand() is sometimes not very reliable
	// The current rand() implementation available is very bad for this
	// situation (% 2), so we'll just use a custom randomizer
	uint32_t randval = SDL_GetTicks();
	dirX = (((randval = randval * 214013L + 2531011L) >> 16) & 0x7fff) % 2;
	dirY = (((randval = randval * 214013L + 2531011L) >> 16) & 0x7fff) % 2;
	if (dirX == 0)
		dirX = -1;
	if (dirY == 0)
		dirY = -1;
}

void Ball::update(int player1Y, int player2Y, int paddleWidth, int paddleHeight, int paddleOffset)
{
	if (preStartTimer > 0)
	{
		preStartTimer--;
		return;
	}

	if (nextSpeedTimer > 0)
	{
		nextSpeedTimer--;
	}
	else
	{
		nextSpeedTimer = FPS * 10;
		speed++;
	}

	for (int i = 0; i < speed; i++) // for more precise collision handling
	{
		x += dirX;
		y += dirY;

		// Check top and bottom boundaries
		if (y <= 0)
			dirY = 1;

		if (y + size >= SCREEN_HEIGHT)
			dirY = -1;

		// Check collisions with paddles
		if (collidesWithPaddle(paddleOffset, player1Y, paddleWidth, paddleHeight))
		{
			if (dirX == -1)
				Mix_PlayChannel(-1, sound, 0);
			dirX = 1;
		}
		else if (collidesWithPaddle(SCREEN_WIDTH - paddleOffset - paddleWidth, player2Y, paddleWidth, paddleHeight))
		{
			if (dirX == 1)
				Mix_PlayChannel(-1, sound, 0);
			dirX = -1;
		}
	}
}

void Ball::draw()
{
	fillRect(x, y, size, size, color);
}

int Ball::hasPlayerWon()
{
	if (x < 0)
		return 2; // Player 2 won

	if (x + size > SCREEN_WIDTH)
		return 1; // Player 1 won

	return 0; // No one has won
}

void Ball::setSound(Mix_Chunk* sound)
{
	this->sound = sound;
}

bool Ball::collidesWithPaddle(int px, int py, int pw, int ph)
{
	if (x > px + pw)
		return false;
	if (x + size < px)
		return false;
	if (y > py + ph)
		return false;
	if (y + size < py)
		return false;

	return true;
}
