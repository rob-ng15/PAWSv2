// SIMPLE CURSES
#include <stdbool.h>

#define COLORS 256
#define A_NOACTION 2048
#define A_NORMAL 256
#define A_BOLD 512
#define A_STANDOUT 512
#define A_UNDERLINE A_NOACTION
#define A_REVERSE 1024
#define A_BLINK A_NOACTION
#define A_DIM A_NORMAL
#define A_PROTECT A_NOACTION
#define A_INVIS A_NOACTION
#define A_ALTCHARSET A_NOACTION
#define A_CHARTEXT A_NOACTION
#define COLOR_PAIRS 256
#define COLOR_PAIRS_MASK (COLOR_PAIRS-1)
#define COLOR_PAIR(a) a|COLORS

// COLOURS
#define COLOR_BLACK BLACK
#define COLOR_BLUE BLUE
#define COLOR_GREEN GREEN
#define COLOR_CYAN CYAN
#define COLOR_RED RED
#define COLOR_MAGENTA MAGENTA
#define COLOR_YELLOW YELLOW
#define COLOR_WHITE WHITE
#define COLOR_ORANGE ORANGE

#define COLS 80
#define LINES 60
#define TPUSIZE COLS*LINES
#define TPUCELLSIZE 4
#define TPUBACKBIT 17
#define TPUFOREBIT 9
#define TPUBOLD 256

typedef unsigned char chtype;
typedef struct{
    int *buffer;
    int y,x,w,h;
    unsigned char backgroundcolours[COLOR_PAIRS], foregroundcolours[COLOR_PAIRS];
    unsigned char background, foreground;
    unsigned char scroll, echo, bold, reverse, autorefresh;
    unsigned short cy,cx;
} WINDOW;

typedef union curses_cell {
    unsigned int bitfield;
    struct {
        unsigned int character : 9;
        unsigned int foreground : 8;
        unsigned int background : 8;
        unsigned int pad : 7;
    } cell;
} __curses_cell;

extern void initscr( void );
extern int endwin( void );
extern int printw( const char *fmt,... );
extern int mvprintw( int y, int x, const char *fmt,... );
extern int intrflush( WINDOW *, bool );
extern int keypad( WINDOW *, bool );

extern int __pnc_refresh( WINDOW * );
extern int __pnc_clear( WINDOW * );
extern void __pnc_cbreak( WINDOW * );
extern void __pnc_echo( WINDOW * );
extern void __pnc_noecho( WINDOW * );
extern void __pnc_scroll( WINDOW * );
extern void __pnc_noscroll( WINDOW * );
extern void __pnc_curs_set( WINDOW *, int );
extern void __pnc_autorefresh( WINDOW *, int );
extern int __pnc_start_color( WINDOW * );
extern bool __pnc_has_colors( WINDOW * );
extern bool __pnc_can_change_color( WINDOW * );
extern int __pnc_init_pair( WINDOW *, short pair, short f, short b );
extern int __pnc_init_color( WINDOW *, short color, short r, short g, short b );
extern int __pnc_move( WINDOW *, int y, int x );
extern int __pnc_getyx( WINDOW *, int *y, int *x );
extern int __pnc_addch( WINDOW *, unsigned char ch );
extern int __pnc_mvaddch( WINDOW *, int y, int x, unsigned char ch );
extern int __pnc_attron( WINDOW *, int attrs );
extern int __pnc_attroff( WINDOW *, int attrs );
extern void __pnc_bkgdset( WINDOW *, int attrs );
extern int __pnc_deleteln( WINDOW * );
extern int __pnc_clrtoeol( WINDOW * );
extern int __pnc_clrtobot( WINDOW * );
extern WINDOW *stdscr;

#define refresh() __pnc_refresh( stdscr )
#define clear() __pnc_clear( stdscr )
#define cbreak() __pnc_cbreak( stdscr )
#define echo() __pnc_echo( stdscr )
#define noecho() __pnc_noecho( stdscr )
#define scroll() __pnc_scroll( stdscr )
#define noscroll() __pnc_noscroll( stdscr )
#define curs_set(a) __pnc_curs_set( stdscr, a )
#define autorefresh(a) __pnc_autorefresh( stdscr, a )
#define start_color() __pnc_start_color( stdscr )
#define has_colors() __pnc_has_colors( stdscr )
#define can_change_color() __pnc_can_change_color( stdscr )
#define init_pair(a,b,c) __pnc_init_pair( stdscr, a, b, c )
#define init_color(a,b,c,d) __pnc_init_color( stdscr, a, b, c, d )
#define move(a,b) __pnc_move( stdscr, a, b )
#define addch(a) __pnc_addch( stdscr, a )
#define mvaddch(a,b,c) __pnc_mvaddch( stdscr, a, b, c )
#define attron(a) __pnc_attron( stdscr, a )
#define attroff(a) __pnc_attroff( stdscr, a )
#define bkgdset(a) __pnc_bkgdset( stdscr, a )
#define deleteln() __pnc_deleteln( stdscr )
#define clrtoeol() __pnc_clrtoeol( stdscr )
#define clrtobot() __pnc_clrtobot( stdscr )

#define getyx(w,y,x) y=w->cy; x=w->cx;
#define getbegyx(w,y,x) y=w->y; x=w->x;
#define getmaxyx(w,y,x) y=w->h; x=w->w;

