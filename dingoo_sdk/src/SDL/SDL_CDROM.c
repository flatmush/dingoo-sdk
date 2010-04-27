#include "SDL.h"

int SDL_CDNumDrives() {
	return 0;
}

const char* SDL_CDName(int drive) {
	return NULL;
}

SDL_CD* SDL_CDOpen(int drive) {
	return NULL;
}

CDstatus SDL_CDStatus(SDL_CD* cdrom) {
	return CD_ERROR;
}

int SDL_CDPlay(SDL_CD* cdrom, int start, int length) {
	return -1;
}

int SDL_CDPlayTracks(SDL_CD* cdrom, int start_track, int start_frame, int ntracks, int nframes) {
	return -1;
}

int SDL_CDPause(SDL_CD* cdrom) {
	return -1;
}

int SDL_CDResume(SDL_CD* cdrom) {
	return -1;
}

int SDL_CDStop(SDL_CD* cdrom) {
	return -1;
}

int SDL_CDEject(SDL_CD* cdrom) {
	return -1;
}

void SDL_CDClose(SDL_CD* cdrom) {
	free(cdrom);
}
