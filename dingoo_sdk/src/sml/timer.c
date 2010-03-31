#include <sml/timer.h>
#include <stdlib.h>



timer* timer_create() {
	timer* tempTimer = (timer*)malloc(sizeof(timer));
	tempTimer->start = (uint32_t)clock();
	tempTimer->last  = tempTimer->start;
	return tempTimer;
}

void timer_delete(timer* inTimer) {
	free(inTimer);
}

uint32_t timer_delta(timer* inTimer) {
	if(inTimer == NULL)
		return 0;
	uint32_t tempTime = (uint32_t)clock();
	uint32_t tempOut  = (tempTime - inTimer->last);
	inTimer->last     = tempTime;
	return tempOut;
}

uint32_t timer_total(timer* inTimer) {
	if(inTimer == NULL)
		return 0;
	return ((uint32_t)clock() - inTimer->start);
}
