#include "game.h"

bool Game::loadGame()
{
	music = NULL;
	soundBounce = NULL;
	splash = NULL;
	bigFont = NULL;
	smallFont = NULL;
	textTitle = NULL;
	textSubTitle = NULL;
	textStart = NULL;
	textScore = NULL;
	textPaused = NULL;

	splash = loadImage("splash.png");
	bigFont = TTF_OpenFont("Share-TechMono.ttf", 28);
	smallFont = TTF_OpenFont("Share-TechMono.ttf", 14);

	if (splash == NULL || bigFont == NULL || smallFont == NULL)
	{
		unloadGame();
		return false;
	}

	colorWhite.r = 0xFF;
	colorWhite.g = 0xFF;
	colorWhite.b = 0xFF;

	textTitle = TTF_RenderText_Solid(bigFont, "Simple Pong Sample", colorWhite);
	textSubTitle = TTF_RenderText_Solid(smallFont, "for Dingoo-SDK", colorWhite);
	textStart = TTF_RenderText_Solid(smallFont, "Press A to start", colorWhite);
	textPaused = TTF_RenderText_Solid(smallFont, "*** PAUSED ***", colorWhite);

	if (textTitle == NULL || textSubTitle == NULL || textStart == NULL || textPaused == NULL)
	{
		unloadGame();
		return false;
	}

	soundBounce = Mix_LoadWAV("bounce.wav");
	music = Mix_LoadMUS("music.ogg"); // Don't NULL check music, because music is optional

	if (soundBounce == NULL)
	{
		unloadGame();
		return false;
	}

	colorIntWhite = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
	colorIntBlack = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	state = STATE_SPLASH;
	tempTime = 0;
	seconds = 0;

	player1Y = player2Y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
	player1Score = player2Score = 0;

	ball.init();
	ball.setSound(soundBounce);

	if (music != NULL)
	{
		Mix_VolumeMusic(MIX_MAX_VOLUME >> 1);
		Mix_PlayMusic(music, -1);
	}

	return true;
}

void Game::unloadGame()
{
	if (music != NULL)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
	}

	if (soundBounce != NULL)
	{
		Mix_HaltChannel(-1);
		Mix_FreeChunk(soundBounce);
	}

	if (splash != NULL)
		SDL_FreeSurface(splash);

	if (bigFont != NULL)
		TTF_CloseFont(bigFont);

	if (smallFont != NULL)
		TTF_CloseFont(smallFont);

	if (textTitle != NULL)
		SDL_FreeSurface(textTitle);

	if (textSubTitle != NULL)
		SDL_FreeSurface(textSubTitle);

	if (textStart != NULL)
		SDL_FreeSurface(textStart);

	if (textScore != NULL)
		SDL_FreeSurface(textScore);
}

void Game::update()
{
	// Dingoo sound driver volume control, function declarations in game.h
	if (controlJustPressed(CONTROL_L))
		dingooSoundVolumeDecrease();
	else if (controlJustPressed(CONTROL_R))
		dingooSoundVolumeIncrease();

	switch (state)
	{
		case STATE_SPLASH:
			updateSplash();
			break;
		case STATE_GAME:
			updateGame();
			break;
		default:
			break;
	}
}

void Game::draw()
{
	switch (state)
	{
		case STATE_SPLASH:
			drawSplash();
			break;
		case STATE_GAME:
			drawGame();
			break;
		default:
			break;
	}
}

void Game::updateSplash()
{
	if (controlJustPressed(CONTROL_SELECT))
		quitGame();
	if (controlJustPressed(CONTROL_A))
		startNewGame();
}

void Game::drawSplash()
{
	drawSurface(0, 0, splash, screen);
	drawSurface((SCREEN_WIDTH - textTitle->w) / 2, 30, textTitle, screen);
	drawSurface((SCREEN_WIDTH - textSubTitle->w) / 2, 80, textSubTitle, screen);
	drawSurface((SCREEN_WIDTH - textStart->w) / 2, 200, textStart, screen);
	SDL_Flip(screen);
}

void Game::updateGame()
{
	if (controlJustPressed(CONTROL_SELECT))
	{
		state = STATE_SPLASH;
		return;
	}

	if (controlJustPressed(CONTROL_START))
		paused = !paused;

	if (!paused)
	{
		if (controlPressed(CONTROL_UP))
			player1Y -= 4;
		if (controlPressed(CONTROL_DOWN))
			player1Y += 4;
		if (controlPressed(CONTROL_X))
			player2Y -= 4;
		if (controlPressed(CONTROL_B))
			player2Y += 4;

		if (player1Y < 0)
			player1Y = 0;
		if (player2Y < 0)
			player2Y = 0;
		if ((player1Y + PADDLE_HEIGHT) > (SCREEN_HEIGHT))
			player1Y = SCREEN_HEIGHT - PADDLE_HEIGHT;
		if ((player2Y + PADDLE_HEIGHT) > (SCREEN_HEIGHT))
			player2Y = SCREEN_HEIGHT - PADDLE_HEIGHT;

		ball.update(player1Y, player2Y, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_OFFSET);
		if (ball.hasPlayerWon() == 1)
			playerScored(1);
		else if (ball.hasPlayerWon() == 2)
			playerScored(2);
	}
}

void Game::drawGame()
{
	SDL_FillRect(screen, NULL, colorIntBlack);
	
	if (textScore != NULL)
		drawSurface((SCREEN_WIDTH - textScore->w) / 2, 5, textScore, screen);

	fillRect(PADDLE_OFFSET, player1Y, PADDLE_WIDTH, PADDLE_HEIGHT, colorIntWhite);
	fillRect(SCREEN_WIDTH - (PADDLE_WIDTH + PADDLE_OFFSET), player2Y, PADDLE_WIDTH, PADDLE_HEIGHT, colorIntWhite);

	ball.draw();

	if (paused)
		drawSurface((SCREEN_WIDTH - textPaused->w) / 2, 90, textPaused, screen);

	SDL_Flip(screen);
}

void Game::startNewGame()
{
	state = STATE_GAME;
	player1Y = player2Y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
	player1Score = player2Score = 0;
	textScore = renderText(textScore, bigFont, "0 | 0", colorWhite);
	ball.reset();
	paused = false;
}

void Game::playerScored(int player)
{
	if (player == 1)
		player1Score++;
	else
		player2Score++;

	char tempString[20];
	sprintf(tempString, "%d | %d", player1Score, player2Score);
	textScore = renderText(textScore, bigFont, tempString, colorWhite);

	ball.reset();
}
