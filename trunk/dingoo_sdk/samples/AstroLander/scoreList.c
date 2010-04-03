#include "scoreList.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sml/graphics.h>

#include "game.h"



score* scoreCreate(uint32_t inTotal, uint32_t inLevel, uint32_t inDifficulty, const char* inName) {
	if(inName == NULL)
		return NULL;
	score* tempScore = (score*)malloc(sizeof(score) + 16);
	if(tempScore == NULL)
		return NULL;
	tempScore->total = inTotal;
	tempScore->level = inLevel;
	tempScore->difficulty = inDifficulty;
	tempScore->name = (char*)((uintptr_t)tempScore + sizeof(score));
	strncpy(tempScore->name, inName, 15);
	return tempScore;
}

void scoreDelete(score* inScore) {
	free(inScore);
}



score_list* scoreListCreate() {
	score_list* tempList = (score_list*)malloc(sizeof(score_list));
	if(tempList == NULL)
		return NULL;
	tempList->count = 0;
	tempList->scores = NULL;
	return tempList;
}

void scoreListDelete(score_list* inList) {
	if(inList == NULL)
		return;
	uint32_t i;
	for(i = 0; i < inList->count; i++)
		scoreDelete(inList->scores[i]);
	free(inList);
}



bool scoreListAdd(score_list* inList, score* inScore) {
	if((inList == NULL) || (inScore == NULL))
		return false;
	score** tempList;
	if(inList->scores == NULL)
		tempList = (score**)malloc((inList->count + 1) * sizeof(score*));
	else
		tempList = (score**)realloc(inList->scores, ((inList->count + 1) * sizeof(score*)));
	if(tempList == NULL)
		return false;
	inList->scores = tempList;

	uintptr_t i, j;
	for(i = 0; (i < inList->count) && (inList->scores[i]->total >= inScore->total); i++);
	for(j = inList->count; j > i; j--)
		inList->scores[j] = inList->scores[j - 1];
	inList->scores[i] = inScore;
	inList->count++;
	return true;
}



score_list* scoreListLoad(const char* inPath) {
	score_list* tempList = scoreListCreate();
	if(tempList == NULL)
		return NULL;

	if(inPath == NULL)
		return tempList;

	FILE* tempFile = fopen(inPath, "rb");
	if(tempFile == NULL)
		return tempList;

	uint32_t tempTotal;
	uint32_t tempDifficulty;
	uint32_t tempLevel;
	char     tempName[16];
	while(!feof(tempFile)) {
		fread(&tempTotal, 4, 1, tempFile);
		fread(&tempDifficulty, 4, 1, tempFile);
		fread(&tempLevel, 4, 1, tempFile);
		fread(tempName, 4, 4, tempFile);
		scoreListAdd(tempList, scoreCreate(tempTotal, tempLevel, tempDifficulty, tempName));
	}
	fclose(tempFile);
	return tempList;
}

bool scoreListSave(const char* inPath, score_list* inList) {
	if((inPath == NULL) || (inList == NULL))
		return false;

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	uintptr_t i;
	for(i = 0; i < inList->count; i++) {
		fwrite(&inList->scores[i]->total, 4, 1, tempFile);
		fwrite(&inList->scores[i]->difficulty, 4, 1, tempFile);
		fwrite(&inList->scores[i]->level, 4, 1, tempFile);
		fwrite(inList->scores[i]->name, 4, 4, tempFile);
	}
	fclose(tempFile);
	return true;
}



void scoreListDraw(score_list* inList, uint32_t inOffset) {
	if(inList == NULL)
		return;

	uint32_t tempWidth = (gfx_font_width(gameFont, "000. 0123456789ABCDE MEDIUM 100 00000000") + 16);
	uint32_t tempHeight = ((gfx_font_height(gameFont) + 2) * 12) + 14;
	uint32_t tempX = (gameDisplay->width - tempWidth) >> 1;
	uint32_t tempY = (gameDisplay->height - tempHeight) >> 1;

	gfx_rect_fill_draw(tempX, tempY, tempWidth, tempHeight, gfx_color_rgb(0x7F, 0x7F, 0x7F));
	gfx_rect_draw(tempX, tempY, tempWidth, tempHeight, gfx_color_rgb(0xFF, 0xFF, 0xFF));

	tempY += 8;
	gfx_font_print_center(tempY, gameFont, "High Scores");
	tempY += (gfx_font_height(gameFont) + 2) << 1;

	uintptr_t i;
	char tempString[64];
	for(i = inOffset; (i < (inOffset + 10)) && (i < inList->count) ; i++) {
		switch(inList->scores[i]->difficulty) {
			case GAME_DIFFICULTY_EASY:
				sprintf(tempString, "% 3lu. %- 15s EASY   % 3lu % 8lu", (i + 1), inList->scores[i]->name, inList->scores[i]->level, inList->scores[i]->total);
				break;
			case GAME_DIFFICULTY_NORMAL:
				sprintf(tempString, "% 3lu. %- 15s MEDIUM % 3lu % 8lu", (i + 1), inList->scores[i]->name, inList->scores[i]->level, inList->scores[i]->total);
				break;
			case GAME_DIFFICULTY_HARD:
				sprintf(tempString, "% 3lu. %- 15s HARD   % 3lu % 8lu", (i + 1), inList->scores[i]->name, inList->scores[i]->level, inList->scores[i]->total);
				break;
			case GAME_DIFFICULTY_ULTRA:
				sprintf(tempString, "% 3lu. %- 15s ULTRA  % 3lu % 8lu", (i + 1), inList->scores[i]->name, inList->scores[i]->level, inList->scores[i]->total);
				break;
		}
		gfx_font_print_center(tempY, gameFont, tempString);
		tempY += gfx_font_height(gameFont) + 2;
	}
}
