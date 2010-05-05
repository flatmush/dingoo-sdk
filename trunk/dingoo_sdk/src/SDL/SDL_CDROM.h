#ifndef __SDL_CDROM_h__
#define __SDL_CDROM_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include "SDL_TYPES.h"

#define SDL_INIT_CDROM (1 << 3)

#define SDL_MAX_TRACKS 0

typedef enum {
  CD_TRAYEMPTY,
  CD_STOPPED,
  CD_PLAYING,
  CD_PAUSED,
  CD_ERROR = -1
} CDstatus;

typedef struct{
	Uint8  id;
	Uint8  type;
	Uint32 length;
	Uint32 offset;
} SDL_CDtrack;

typedef struct{
	int         id;
	CDstatus    status;
	int         numtracks;
	int         cur_track;
	int         cur_frame;
	SDL_CDtrack track[SDL_MAX_TRACKS + 1];
} SDL_CD;

extern int         SDL_CDNumDrives();
extern const char* SDL_CDName(int drive);
extern SDL_CD*     SDL_CDOpen(int drive);
extern CDstatus    SDL_CDStatus(SDL_CD* cdrom);
extern int         SDL_CDPlay(SDL_CD* cdrom, int start, int length);
extern int         SDL_CDPlayTracks(SDL_CD* cdrom, int start_track, int start_frame, int ntracks, int nframes);
extern int         SDL_CDPause(SDL_CD* cdrom);
extern int         SDL_CDResume(SDL_CD* cdrom);
extern int         SDL_CDStop(SDL_CD* cdrom);
extern int         SDL_CDEject(SDL_CD* cdrom);
extern void        SDL_CDClose(SDL_CD* cdrom);

#ifdef __cplusplus
}
#endif

#endif
