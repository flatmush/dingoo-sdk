#include "modplay.h"

void modplay_init(MODFILE* mod_buf) {
  MODFILE_Init(mod_buf);
}

void modplay_term(MODFILE* mod_buf) {
  MODFILE_Stop(mod_buf);
  MODFILE_Free(mod_buf);
}
  
int modplay_loadmod(char *inFile, MODFILE* mod_buf) {
  int retmod = MODFILE_Load(inFile, mod_buf);
  if (retmod>=0) {
    mod_buf->musicvolume = 64;
    mod_buf->sfxvolume = 64;
    MODFILE_Start(mod_buf);
    /*
     * mod       - A pointer to the MODFILE structure of
     *             which the output format shall be changed
     * freq      - Output frequency. Common values are
     *             11025Hz, 22050Hz and 44100Hz
     * channels  - 1 for mono and 2 for stereo
     * bits      - 8 or 16 are valid values
     * mixsigned - TRUE if the output stream shall consist
     *             of signed values
    */ 
    MODFILE_SetFormat(mod_buf, 22050, 1, 16, 1);
    
    return 1;
  }
  
  return 0;
}
  
int modplay_loadmod_from_buffer(unsigned int len, unsigned char *pMod, MODFILE* mod_buf) {
  
  int retmod = MODFILE_Set(pMod, len, mod_buf);
  if (retmod>=0) {
    mod_buf->musicvolume = 64;
    mod_buf->sfxvolume = 64;
    MODFILE_Start(mod_buf);
    /*
     * mod       - A pointer to the MODFILE structure of
     *             which the output format shall be changed
     * freq      - Output frequency. Common values are
     *             11025Hz, 22050Hz and 44100Hz
     * channels  - 1 for mono and 2 for stereo
     * bits      - 8 or 16 are valid values
     * mixsigned - TRUE if the output stream shall consist
     *             of signed values
    */ 
    MODFILE_SetFormat(mod_buf, 22050, 1, 16, 1);
    
    return 1;
  }
  
  return 0;
}

int modplay_loadxm_from_buffer(unsigned int len, unsigned char *pMod, MODFILE* mod_buf) {

  int retmod = MODFILE_SetXM(pMod, len, mod_buf);
  if (retmod>=0) {
    mod_buf->musicvolume = 64;
    mod_buf->sfxvolume = 64;
    MODFILE_Start(mod_buf);
    /*
     * mod       - A pointer to the MODFILE structure of
     *             which the output format shall be changed
     * freq      - Output frequency. Common values are
     *             11025Hz, 22050Hz and 44100Hz
     * channels  - 1 for mono and 2 for stereo
     * bits      - 8 or 16 are valid values
     * mixsigned - TRUE if the output stream shall consist
     *             of signed values
    */ 
    MODFILE_SetFormat(mod_buf, 22050, 1, 16, 1);
    
    return 1;
  }
  
  return 0;
}
