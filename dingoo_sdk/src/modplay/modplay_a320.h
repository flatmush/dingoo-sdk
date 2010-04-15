#ifndef __modplaya320_h__
#define __modplaya320_h__

#include "modplay.h"

extern void modplay_init(MODFILE* mod_buf);
extern void modplay_term(MODFILE* mod_buf);

extern int modplay_loadmod(char *inFile, MODFILE* mod_buf);
extern int modplay_loadmod_from_buffer(unsigned int len, unsigned char *pMod, MODFILE* mod_buf);
extern int modplay_loadxm_from_buffer(unsigned int len, unsigned char *pMod, MODFILE* mod_buf);

#endif
