#ifndef __particle_h__
#define __particle_h__

#include <sml/fixmath.h>

#include <fgl/fgl.h>



typedef struct {
	uintptr_t   age;
	fix16_t     x, y;
	fix16_t     vel_x, vel_y;
	void*       next;
	void*       prev;
} particle;

typedef struct {
	particle*   list;
	bool        (*tick_func)(particle* inParticle);
	void        (*draw_func)(particle* inParticle, int32_t inOffX, int32_t inOffY);
} particle_system;



extern particle_system* particle_system_create();
extern void             particle_system_delete(particle_system* inSystem);

extern bool particle_system_add(particle_system* inSystem, particle* inParticle);
extern void particle_system_tick(particle_system* inSystem);
extern void particle_system_draw(particle_system* inSystem, int32_t inOffX, int32_t inOffY);

extern particle* particle_create(uintptr_t inAge, fix16_t inX, fix16_t inY, fix16_t inVelX, fix16_t inVelY);
extern void      particle_delete(particle* inParticle);

#endif
