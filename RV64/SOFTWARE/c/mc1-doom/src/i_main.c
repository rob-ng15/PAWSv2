// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

#ifdef MC1_SDK
#include <mc1/newlib_integ.h>
#endif

#include "doomdef.h"

#include "m_argv.h"
#include "d_main.h"

int
main
( int           argc,
  char**        argv )
{
#ifdef MC1_SDK
    mc1newlib_init(MC1NEWLIB_ALL & ~MC1NEWLIB_CONSOLE);
#endif

    myargc = argc;
    myargv = argv;

    D_DoomMain ();

#ifdef MC1_SDK
    mc1newlib_terminate();
#endif

    return 0;
}
