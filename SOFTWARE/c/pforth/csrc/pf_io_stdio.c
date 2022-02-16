/* $Id$ */
/***************************************************************
** I/O subsystem for PForth for common systems.
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
****************************************************************
** 941004 PLB Extracted IO calls from pforth_main.c
***************************************************************/

#include "pf_all.h"
#include <PAWSlibrary.h>

/* Default portable terminal I/O. */
int  sdTerminalOut( char c )
{
    return addch(c);
}
/* We don't need to echo because getchar() echos. */
int  sdTerminalEcho( char c )
{
    return sdTerminalOut( c );
}
int  sdTerminalIn( void )
{
    int c = ps2_inputcharacter();
    return ( c == 13 ) ? '\n' : c;
}
int  sdQueryTerminal( void )
{
    return ps2_character_available();
}

int  sdTerminalFlush( void )
{
#ifdef PF_NO_FILEIO
    return -1;
#else
    return fflush(PF_STDOUT);
#endif
}

void sdTerminalInit( void )
{
    ps2_keyboardmode(PS2_KEYBOARD); initscr(); start_color(); autorefresh( TRUE );
    attron( COLOR_PAIR(3) | A_BOLD );
    printw( "PForth by Phil Burk\n" );
    printw( "Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom\n" );
    printw( "Source Code https://github.com/philburk/pforth\n" );
    printw( "Documentation http://www.softsynth.com/pforth/\n\n" );

    attron( COLOR_PAIR(1) | A_BOLD );
    printw( "Ported to PAWSv2 by Rob S\n" );
    printw( "TO load the PForth system:" );
    attron( COLOR_PAIR(2) | A_BOLD );
    printw( "loadsys\n\n" );
    attron( COLOR_PAIR(7) | A_NORMAL );
}
void sdTerminalTerm( void )
{
}

