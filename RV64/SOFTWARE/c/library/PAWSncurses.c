// SIMPLE CURSES LIBRARY
#include "curses.h"

WINDOW __curses_stdscr, *stdscr = &__curses_stdscr;
char __stdinout_init = FALSE, __sdcard_init = FALSE;

void __position_curses( unsigned short x, unsigned short y ) {
    tpu_move( x, y );
}

void __update_tpu( WINDOW *window ) {
    tpu_set( window -> cx + window -> x, window -> cy + window -> y, window -> background, window -> foreground );
}

void initscr( void ) {
    __curses_cell temp;

    stdscr -> y = 0; stdscr -> x = 0; stdscr -> w = COLS; stdscr -> h = LINES;
    stdscr -> cx = 0; stdscr-> cy = 0;
    stdscr -> background = BLACK; stdscr -> foreground = WHITE;
    stdscr -> scroll = 1; stdscr -> echo = 0; stdscr -> bold = 0; stdscr -> reverse = 0; stdscr -> autorefresh = 0;

    stdscr -> buffer = malloc( TPUSIZE * TPUCELLSIZE );
    if( stdscr -> buffer == NULL )
        exit;

    temp.cell.background = stdscr -> reverse ? stdscr -> foreground : stdscr -> background;
    temp.cell.foreground = stdscr -> reverse ? stdscr -> background : stdscr -> foreground;
    paws_memset32( (void *)( stdscr -> buffer ), temp.bitfield, stdscr -> w *  stdscr -> h * TPUCELLSIZE  );

    *TPU_CURSOR = TRUE; __update_tpu( stdscr );
    __stdinout_init = TRUE;
}

int endwin( void ) {
    return( true );
}

int __pnc_refresh( WINDOW *window ) {
    if( window == stdscr )
        paws_memcpy( (void *)TPUBUFFER, window -> buffer, window -> w *  window -> h * TPUCELLSIZE );
    else
        paws_memcpy_rectangle( (void *)&TPUBUFFER[ window -> y * COLS + window -> x ], window -> buffer, window -> w * TPUCELLSIZE, COLS * 4, 0, window -> h );
    return( true );
}

int __pnc_clear( WINDOW *window ) {
    __curses_cell temp;
    temp.cell.background = window -> reverse ? window -> foreground : window -> background;
    temp.cell.foreground = window -> reverse ? window -> background : window -> foreground;

    paws_memset32( (void *)( window -> buffer ), temp.bitfield, window -> w *  window -> h * TPUCELLSIZE  );
    if( window -> autorefresh )
        __pnc_refresh( window );
    return( true );
}

void __pnc_cbreak( WINDOW *window ) {
}

void __pnc_echo( WINDOW *window ) {
}

void __pnc_noecho( WINDOW *window ) {
}

void __pnc_scroll( WINDOW *window ) {
    window -> scroll = 1;
}

void __pnc_noscroll( WINDOW *window ) {
    window -> scroll = 0;
}

void __pnc_curs_set( WINDOW *window, int visibility ) {
    *TPU_CURSOR = visibility;
}

void __pnc_autorefresh( WINDOW *window, int flag ) {
   window -> autorefresh = flag;
}

int __pnc_start_color( WINDOW *window ) {
    for( unsigned short i = 0; i < 15; i++ ) {
        window -> foregroundcolours[i] = BLACK;
        window -> backgroundcolours[i] = BLACK;
    }
    window -> foregroundcolours[0] = BLACK;
    window -> foregroundcolours[1] = RED;
    window -> foregroundcolours[2] = GREEN;
    window -> foregroundcolours[3] = YELLOW;
    window -> foregroundcolours[4] = BLUE;
    window -> foregroundcolours[5] = MAGENTA;
    window -> foregroundcolours[6] = CYAN;
    window -> foregroundcolours[7] = WHITE;

    return( true );
}

bool __pnc_has_colors( WINDOW *window ) {
    return( true );
}

bool __pnc_can_change_color( WINDOW *window ) {
    return( true );
}

int __pnc_init_color( WINDOW *window, short color, short r, short g, short b ) {
    return( true );
}

int __pnc_init_pair( WINDOW *window, short pair, short f, short b ) {
    window -> foregroundcolours[ pair ] = f;
    window -> backgroundcolours[ pair ] = b;
    return( true );
}


int __pnc_move( WINDOW *window, int y, int x ) {
    window -> cx = ( unsigned short ) ( x < 0 ) ? 0 : ( x > window -> w - 1 ) ? window -> w - 1 : x;
    window -> cy = ( unsigned short ) ( y < 0 ) ? 0 : ( y > window -> h - 1 ) ? window -> h - 1 : y;
    __position_curses( window -> cx + window -> x, window -> cy + window -> y );
    return( true );
}

void __scroll( WINDOW *window ) {
    __curses_cell temp;
    temp.cell.background = window -> reverse ? window -> foreground : window -> background;
    temp.cell.foreground = window -> reverse ? window -> background : window -> foreground;

    paws_memcpy( (void *)window -> buffer, (void *)( &window -> buffer[ window -> w ] ), window -> w * ( window -> h - 1 ) *  TPUCELLSIZE );
    paws_memset32( (void *)&window -> buffer[ window -> w * ( window -> h - 1 ) ], temp.bitfield, window -> w * TPUCELLSIZE );
}

int __pnc_addch( WINDOW *window, unsigned char ch ) {
    short gonextline = 0;
    __curses_cell temp;
    switch( ch ) {
        case '\b': {
            // BACKSPACE
            if( window -> cx ) {
                window -> cx--;
            } else {
                if( window -> cy ) {
                    window -> cy--;
                    window -> cx = window -> w - 1;
                }
            }
            break;
        }
        case '\n': {
            // LINES  FEED
            window -> cx = 0;
            gonextline = 1;
            break;
        }
        case '\r': {
            // CARRIAGE RETURN
            window -> cx = 0;
            break;
        }
        case '\t': {
            // TAB
            window -> cx = ( 1 + window -> cx / 8 ) * 8;
            if( window -> cx >= window -> w ) {
                window -> cx = 0;
                gonextline = 1;
            }
            break;
        }

        default: {
            temp.cell.background = window -> reverse ? window -> foreground : window -> background;
            temp.cell.foreground = window -> reverse ? window -> background : window -> foreground;
            temp.cell.character = ( window -> bold ? TPUBOLD : 0 ) + ch;

            if( window -> autorefresh )
                TPUBUFFER[ window -> cy * COLS + window -> cx ] = temp.bitfield;
            window -> buffer[ window -> cy * window -> w + window -> cx ] = temp.bitfield;

            if( window -> cx == window -> w - 1 ) {
                window -> cx = 0;
                gonextline = 1;
            } else {
                window -> cx++;
            }
        }
    }

    // GO TO NEXT LINES , SCROLL/WRAP IF REQUIRED
    if( gonextline ) {
        if( window -> cy == window -> h - 1 ) {
            if( window -> scroll ) {
                __scroll( window );
                if( window -> autorefresh )
                    __pnc_refresh( window );
            } else {
                window -> cy = 0;
            }
        } else {
            window -> cy++;
        }
    }

    __position_curses( window -> cx + window -> x, window -> cy + window -> y );
    return( true );
}

int __pnc_mvaddch( WINDOW *window, int y, int x, unsigned char ch ) {
    (void)__pnc_move( window, y, x );
    return( __pnc_addch( window, ch ) );
}

void __curses_print_string( WINDOW *window, const char* s ) {
   for(const char* p = s; *p; ++p) {
      __pnc_addch( window, *p );
   }
}

int printw( const char *fmt,... ) {
    static char buffer[1024];
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 1023, fmt, args);
    va_end(args);

    __curses_print_string( stdscr, buffer );
    return( true );
}

int mvprintw( int y, int x, const char *fmt,... ) {
    static char buffer[1024];
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 1023, fmt, args);
    va_end(args);

    __pnc_move( stdscr, y, x );
    __curses_print_string( stdscr, buffer );

    return( true );
}

int __pnc_attron( WINDOW *window, int attrs ) {
    if( attrs & COLORS ) {
        window -> foreground = window -> foregroundcolours[ attrs & COLOR_PAIRS_MASK ];
        window -> background = window -> backgroundcolours[ attrs & COLOR_PAIRS_MASK ];
        __update_tpu( window );
    }

    if( attrs & A_NORMAL ) {
        window -> bold = 0;
        window -> reverse = 0;
    }

    if( attrs & A_BOLD ) window -> bold = 1;

    if( attrs & A_REVERSE ) window -> reverse = 1;

    return( true );
}

int __pnc_attroff( WINDOW *window, int attrs ) {
    if( attrs & A_BOLD ) window -> bold = 0;
    if( attrs & A_REVERSE )  window -> reverse = 0;

    return( true );
}

void __pnc_bkgdset( WINDOW *window, int attrs ) {
    window -> foreground = window -> foregroundcolours[ attrs & COLOR_PAIRS_MASK ];
    window -> background = window -> backgroundcolours[ attrs & COLOR_PAIRS_MASK ];
}

int __pnc_deleteln( WINDOW *window ) {
    // NEEDED
    return( true );
}

int __pnc_clrtoeol( WINDOW *window ) {
    __curses_cell temp;
    temp.cell.background = window -> reverse ? window -> foreground : window -> background;
    temp.cell.foreground = window -> reverse ? window -> background : window -> foreground;

    for( int x = window -> cx; x < window -> w; x ++ )
        window -> buffer[ window -> cy * window -> w + x ] = temp.bitfield;

    return( true );
}

int __pnc_clrtobot( WINDOW *window ) {
    __curses_cell temp;
    temp.cell.background = window -> reverse ? window -> foreground : window -> background;
    temp.cell.foreground = window -> reverse ? window -> background : window -> foreground;

    for( int i = window -> cy * window -> w + window -> cx; i < window -> h * window -> w; i ++ )
        window -> buffer[ i ] = temp;

    return( true );
}

int intrflush( WINDOW *win, bool bf ) {
    return( 0 );
}

int keypad( WINDOW *win, bool bf ) {
    return( 0 );
}
