/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

/** 
 *  @file begin_code.h
 *  This file sets things up for C dynamic library function definitions,
 *  static inlined functions, and structures aligned at 4-byte alignment.
 *  If you don't like ugly C preprocessor code, don't look at this file. :)
 */

/** 
 *  @file begin_code.h
 *  This shouldn't be nested -- included it around code only.
 */
#ifdef _begin_code_h
#error Nested inclusion of begin_code.h
#endif
#define _begin_code_h

/** 
 *  @def DECLSPEC
 *  Some compilers use a special export keyword
 */
#ifndef DECLSPEC
#define DECLSPEC
#endif

/** 
 *  @def SDLCALL
 *  By default SDL uses the C calling convention
 */
#ifndef SDLCALL
#define SDLCALL
#endif /* SDLCALL */

/**
 *  @file begin_code.h
 *  Force structure packing at 4 byte alignment.
 *  This is necessary if the header is included in code which has structure
 *  packing set to an alternate value, say for loading structures from disk.
 *  The packing is reset to the previous value in close_code.h 
 */

// ...

/**
 *  @def SDL_INLINE_OKAY
 *  Set up compiler-specific options for inlining functions
 */
#ifndef SDL_INLINE_OKAY
#define SDL_INLINE_OKAY
#endif /* SDL_INLINE_OKAY */
