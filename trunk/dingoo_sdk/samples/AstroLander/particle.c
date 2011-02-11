#include "particle.h"
#include <stdlib.h>



particle_system* particle_system_create() {
	particle_system* tempPartSys = (particle_system*)malloc(sizeof(particle_system));
	if(tempPartSys == NULL)
		return NULL;

	tempPartSys->list       = NULL;
	tempPartSys->tick_func  = NULL;
	tempPartSys->draw_func  = NULL;

	return tempPartSys;
}

void particle_system_delete(particle_system* inSystem) {
	if(inSystem == NULL)
		return;
	particle* tempParticle = inSystem->list;
	if(tempParticle != NULL) {
		while(tempParticle->prev != NULL)
			tempParticle = (particle*)tempParticle->prev;
		particle* tempDelete;
		do {
			tempDelete = tempParticle;
			tempParticle = (particle*)tempParticle->next;
			particle_delete(tempDelete);
		} while(tempParticle != NULL);
	}
	free(inSystem);
}



bool particle_system_add(particle_system* inSystem, particle* inParticle) {
	if((inSystem == NULL) || (inParticle == NULL))
		return false;

	if(inSystem->list != NULL) {
		while(inSystem->list->prev != NULL)
			inSystem->list = inSystem->list->prev;
	}

	inParticle->next = (void*)inSystem->list;
	if(inSystem->list != NULL)
		inSystem->list->prev = (void*)inParticle;
	inSystem->list = inParticle;
	return true;
}

void particle_system_tick(particle_system* inSystem) {
	if((inSystem == NULL) || (inSystem->tick_func == NULL))
		return;

	particle* tempParticle = inSystem->list;
	if(tempParticle == NULL)
		return;
	while(tempParticle->prev != NULL)
		tempParticle = (particle*)tempParticle->prev;
	inSystem->list = tempParticle;

	do {
		particle* tempNext = (particle*)tempParticle->next;
		if(!inSystem->tick_func(tempParticle)) {
			particle* tempPrev = (particle*)tempParticle->prev;
			if(tempPrev != NULL)
				tempPrev->next = tempNext;
			if(tempNext != NULL)
				tempNext->prev = tempPrev;
			if(tempParticle == inSystem->list)
				inSystem->list = (tempPrev == NULL ? tempNext : tempPrev);
			particle_delete(tempParticle);
		}
		tempParticle = tempNext;
	} while(tempParticle != NULL);
}

void particle_system_draw(particle_system* inSystem, int32_t inOffX, int32_t inOffY) {
	if((inSystem == NULL) || (inSystem->draw_func == NULL))
		return;

	particle* tempParticle = inSystem->list;
	if(tempParticle == NULL)
		return;
	while(tempParticle->prev != NULL)
		tempParticle = (particle*)tempParticle->prev;
	inSystem->list = tempParticle;

	do {
		inSystem->draw_func(tempParticle, inOffX, inOffY);
		tempParticle = (particle*)tempParticle->next;
	} while(tempParticle != NULL);
}



particle* particle_create(uintptr_t inAge, fix16_t inX, fix16_t inY, fix16_t inVelX, fix16_t inVelY) {
	particle* tempParticle = (particle*)malloc(sizeof(particle));
	if(tempParticle == NULL)
		return NULL;

	tempParticle->age = inAge;
	tempParticle->x = inX;
	tempParticle->y = inY;
	tempParticle->vel_x = inVelX;
	tempParticle->vel_y = inVelY;
	tempParticle->next = NULL;
	tempParticle->prev = NULL;

	return tempParticle;
}

void particle_delete(particle* inParticle) {
	free(inParticle);
}
