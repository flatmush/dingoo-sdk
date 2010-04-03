#ifndef __game_h__
#define __game_h__

#include <stdbool.h>
#include <stdint.h>

#include <sml/timer.h>
#include <sml/display.h>
#include <sml/graphics.h>

#include "ship.h"
#include "level.h"
#include "background.h"
#include "scoreList.h"



#define GAME_MODE_SPLASH 1
#define GAME_MODE_MENU   2
#define GAME_MODE_GAME   3

#define GAME_MODE_GAME_PLAY     1
#define GAME_MODE_GAME_PAUSE    2
#define GAME_MODE_GAME_LANDED   3
#define GAME_MODE_GAME_CRASHED  4
#define GAME_MODE_GAME_GAMEOVER 5

#define GAME_DIFFICULTY_EASY   1
#define GAME_DIFFICULTY_NORMAL 2
#define GAME_DIFFICULTY_HARD   3
#define GAME_DIFFICULTY_ULTRA  4



extern void  gameScreenshot();
extern void  gameDialog(char* inMessage);

extern void gameRespawn();
extern void gameDifficultyNext();
extern void gameDifficultyPrev();
extern void gameLevelNext();
extern void gameRestart();



extern gfx_texture* gameFont;
extern gfx_texture* gameSplash;

extern uint8_t         gameMode;
extern uint8_t         gameModeSub;
extern ship_land_score gameLandingScore;

extern uint32_t    gameTime;
extern uint32_t    gameLives;
extern uint32_t    gameScore;
extern uint32_t    gameLevelNo;
extern uint32_t    gameDifficulty;
extern char        gameDifficultyStrings[4][32];

extern level*      gameLevel;
extern ship*       gameShip;
extern background* gameBackground;
extern score_list* gameScores;
extern uintptr_t   gameScoreListOffset;

extern display* gameDisplay;
extern timer*   gameTimer;
extern uint32_t gameTickRate;
extern bool     gameRunning;

#endif
