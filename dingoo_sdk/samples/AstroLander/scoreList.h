#ifndef __scoreList_h__
#define __scoreList_h__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint32_t total;
	uint32_t level;
	uint32_t difficulty;
	char*    name;
} score;

typedef struct {
	score**   scores;
	uint32_t  count;
} score_list;



extern score*      scoreCreate(uint32_t inTotal, uint32_t inLevel, uint32_t inDifficulty, const char* inName);
extern void        scoreDelete(score* inScore);

extern score_list* scoreListCreate();
extern void        scoreListDelete(score_list* inList);

extern bool        scoreListAdd(score_list* inList, score* inScore);

extern score_list* scoreListLoad(const char* inPath);
extern bool        scoreListSave(const char* inPath, score_list* inList);

extern void        scoreListDraw(score_list* inList, uint32_t inOffset);

#endif
