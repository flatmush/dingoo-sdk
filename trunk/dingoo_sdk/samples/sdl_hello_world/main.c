/*
** -*- coding: ascii -*-
** vim:ts=4:sw=4:softtabstop=4:smarttab:expandtab
**
** SDL - Simple Hello World
**
**  Updates screen *all* the time, even if there is no change to the screen
**  Show Approximate Frames Per Second (~100fps on A320)
**  Quit on any key press
**  Should also respond to system events such as; power off,
**  usb cable plugged in, quit (press-and-hold-X), etc.
**  use SDL_putenv("DINGOO_IGNORE_OS_EVENTS=1"); to disable this.
** 
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>

#include <SDL/SDL.h>

#include "SFont.h"

#include "defaultfont_8x8_2bits.h"


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_DEPTH 16


int main(int argc, char *argv[]) 
{
    SDL_Surface* screen =NULL;
    bool done = false; 
    SDL_Event evt; 
    int FPS = 0; 
    int pastFPS = 0; 
    int past = 0; 
    int currentTime = 0;
    static char buffer[20] = {0};
    
    SDL_Surface *font_bitmap_surface=NULL;
    SFont_Font* Font;
    
    
    
    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        exit (1);
    }
    
    SDL_ShowCursor(SDL_DISABLE); /* There is no mouse */

    /* Initialize the screen / window */
    /*screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SDL_SWSURFACE|SDL_FULLSCREEN);  SDL_FULLSCREEN not needed on Dingoo, useful on desktop */
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SDL_SWSURFACE);
    if (screen == NULL)
    {
        SDL_Quit();
        exit (2);
    }
    
    SDL_WM_SetCaption ("SDL MultiMedia Application", NULL); /* Not needed on Dingoo */

    font_bitmap_surface = get_default_data_font();
    Font = SFont_InitFont(font_bitmap_surface);
    if(!Font) {
        fprintf(stderr, "An error occured while setting up font.");
        exit(1);
    }
    
    while(!done) 
    {
        if( SDL_PollEvent(&evt) ) 
        { 
            switch (evt.type)
            {
                        case SDL_KEYDOWN:
                             /* quit on any key press */
                             done = true; 
                             break;
                        case SDL_QUIT:
                             done = true; 
                             break;
            }
        }

        currentTime = SDL_GetTicks();
#define LOCK_FPS_TO_60FPS
#undef LOCK_FPS_TO_60FPS
#ifdef LOCK_FPS_TO_60FPS
        /* Alternative is to sleep, however sleep may not have the expected granularity */
        if ( currentTime - past >= 16 ) 
#endif /* LOCK_FPS_TO_60FPS */
        { 
            past = SDL_GetTicks(); /* this should be done before redrawing screen */

            #define ALWAYS_FLIP
            #ifdef ALWAYS_FLIP
            SFont_Write(screen, Font, 0, 20, "Hello World");
            SDL_UpdateRect(screen, 0, 0, 0, 0);
            #endif /* ALWAYS_FLIP */
            FPS++; 
        }

        if ( currentTime - pastFPS >= 1000 )
        { 
            sprintf( buffer, "%d FPS", FPS );
            SDL_FillRect( SDL_GetVideoSurface(), NULL, 0 ); /* rather brute force wipe out area first... */
            SFont_Write (screen, Font, 0,0, buffer);
            SDL_UpdateRect(screen, 0, 0, 0, 0); /* SDL_UpdateRect() is a way to only mark part of the screen as updated for performance */
            #ifndef ALWAYS_FLIP
            SDL_Flip( screen );
            #endif /* ALWAYS_FLIP */
        
            FPS = 0;
            pastFPS = currentTime;
        }
    }

    SFont_FreeFont(Font);
    SDL_Quit();

    return 0; 
}
