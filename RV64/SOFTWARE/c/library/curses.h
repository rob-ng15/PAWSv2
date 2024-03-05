// SIMPLE CURSES
#include <stdbool.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define A_NORMAL 512
#define A_BOLD 1024
#define A_STANDOUT 1024
#define A_WIDE 2048
#define A_TALL 4096
#define A_BLINK 8192
#define A_UNDERLINE 16384
#define A_REVERSE 32768
#define A_NOACTION 65536
#define A_DIM A_NORMAL
#define A_PROTECT A_NOACTION
#define A_INVIS A_NOACTION
#define A_ALTCHARSET A_NOACTION
#define A_CHARTEXT A_NOACTION

// COLOURS
#define COLORS 256
#define COLOR_PAIRS 256
#define COLOR_PAIRS_MASK (COLOR_PAIRS-1)
#define COLOR_PAIR(a) a|COLORS
#define COLOR_BLACK BLACK
#define COLOR_BLUE BLUE
#define COLOR_GREEN GREEN
#define COLOR_CYAN CYAN
#define COLOR_RED RED
#define COLOR_MAGENTA MAGENTA
#define COLOR_YELLOW YELLOW
#define COLOR_WHITE WHITE
#define COLOR_ORANGE ORANGE

// SIZES
#define COLS 80
#define LINES 60
#define TPUSIZE COLS*LINES
#define TPUCELLSIZE 4
#define TPUATTRBIT 24
#define TPUBACKBIT 16
#define TPUFOREBIT 8

// SPECIAL CHARACTERS
#define ACS_VLINE 0xb3
#define ACS_HLINE 0xc4
#define ACS_ULCORNER 0xda
#define ACS_URCORNER 0xbf
#define ACS_LLCORNER 0xc0
#define ACS_LRCORNER 0xd9
#define ACS_LTEE 0xc3
#define ACS_RTEE 0xb4
#define ACS_BTEE 0xc1
#define ACS_TTEE 0xc2
#define ACS_PLUS 0xc5

typedef unsigned char chtype;
typedef struct{
    int *buffer;
    int y,x,w,h;
    unsigned char backgroundcolours[COLOR_PAIRS], foregroundcolours[COLOR_PAIRS];
    unsigned char background, foreground;
    unsigned char scroll, echo, attributes, reverse, autorefresh;
    unsigned short cy,cx;
    void *parent;
} WINDOW;

typedef union curses_cell {
    unsigned int bitfield;
    struct {
        unsigned int character : 8;
        unsigned int foreground : 8;
        unsigned int background : 8;
        unsigned int attributes : 5;
        unsigned int pad : 3;
    } cell;
} __curses_cell;

extern void initscr( void );
extern int endwin( void );
extern int printw( const char *fmt,... );
extern int mvprintw( int y, int x, const char *fmt,... );
extern int intrflush( WINDOW *, bool );
extern int keypad( WINDOW *, bool );

extern WINDOW *newwin( int nlines, int ncols, int begin_y, int begin_x );
extern WINDOW *subwin( WINDOW *parent, int nlines, int ncols, int begin_y, int begin_x );
extern int delwin( WINDOW * );

extern int __pnc_refresh( WINDOW * );
extern int __pnc_clear( WINDOW * );
extern void __pnc_cbreak( WINDOW * );
extern void __pnc_echo( WINDOW * );
extern void __pnc_noecho( WINDOW * );
extern void __pnc_scroll( WINDOW * );
extern void __pnc_scrollok( WINDOW *, bool );
extern void __pnc_curs_set( WINDOW *, int );
extern void __pnc_autorefresh( WINDOW *, int );
extern int __pnc_start_color( WINDOW * );
extern bool __pnc_has_colors( WINDOW * );
extern bool __pnc_can_change_color( WINDOW * );
extern int __pnc_init_pair( WINDOW *, short pair, short f, short b );
extern int __pnc_init_color( WINDOW *, short color, short r, short g, short b );
extern int __pnc_move( WINDOW *, int y, int x );
extern int __pnc_addch( WINDOW *, char );
extern int __pnc_addstr( WINDOW *, char * );
extern int __pnc_mvaddch( WINDOW *, int y, int x, char );
extern int __pnc_mvaddstr( WINDOW *, int y, int x, char * );
extern int __pnc_attron( WINDOW *, int attrs );
extern int __pnc_attroff( WINDOW *, int attrs );
extern void __pnc_bkgdset( WINDOW *, int attrs );
extern int __pnc_deleteln( WINDOW * );
extern int __pnc_clrtoeol( WINDOW * );
extern int __pnc_clrtobot( WINDOW * );

extern int __pnc_getch( WINDOW * );
extern int __pnc_erasechar( void );
extern int __pnc_killchar( void );
extern int __pnc_raw( void );

extern WINDOW *stdscr, *curscr;

#define refresh() __pnc_refresh( stdscr )
#define clear() __pnc_clear( stdscr )
#define erase() __pnc_clear( stdscr )
#define cbreak() __pnc_cbreak( stdscr )
#define echo() __pnc_echo( stdscr )
#define noecho() __pnc_noecho( stdscr )
#define scroll() __pnc_scroll( stdscr )
#define curs_set(a) __pnc_curs_set( stdscr, a )
#define autorefresh(a) __pnc_autorefresh( stdscr, a )
#define start_color() __pnc_start_color( stdscr )
#define has_colors() __pnc_has_colors( stdscr )
#define can_change_color() __pnc_can_change_color( stdscr )
#define init_pair(a,b,c) __pnc_init_pair( stdscr, a, b, c )
#define init_color(a,b,c,d) __pnc_init_color( stdscr, a, b, c, d )
#define move(a,b) __pnc_move( stdscr, a, b )
#define addch(a) __pnc_addch( stdscr, a )
#define addstr(a) __pnc_addstr( stdscr, a )
#define mvaddch(a,b,c) __pnc_mvaddch( stdscr, a, b, c )
#define mvaddstr(a,b,c) __pnc_mvaddstr( stdscr, a, b, c )
#define attron(a) __pnc_attron( stdscr, a )
#define attroff(a) __pnc_attroff( stdscr, a )
#define bkgdset(a) __pnc_bkgdset( stdscr, a )
#define deleteln() __pnc_deleteln( stdscr )
#define clrtoeol() __pnc_clrtoeol( stdscr )
#define clrtobot() __pnc_clrtobot( stdscr )

#define wrefresh(w) __pnc_refresh( w )
#define wnoutrefresh(w) __pnc_refresh( w )
#define wclear(w) __pnc_clear( w )
#define werase(w) __pnc_clear( w )
#define wcbreak(w) __pnc_cbreak( w )
#define wecho(w) __pnc_echo( w )
#define wnoecho(w) __pnc_noecho( w )
#define wscroll(w) __pnc_scroll( w )
#define wcurs_set(w,a) __pnc_curs_set( w, a )
#define wautorefresh(w,a) __pnc_autorefresh( w, a )
#define wstart_color(w) __pnc_start_color( w )
#define whas_colors(w) __pnc_has_colors( w )
#define wcan_change_color(w) __pnc_can_change_color( w )
#define winit_pair(w,a,b,c) __pnc_init_pair( w, a, b, c )
#define winit_color(w,a,b,c,d) __pnc_init_color( w, a, b, c, d )
#define wmove(w,a,b) __pnc_move( w, a, b )
#define waddch(w,a) __pnc_addch( w, a )
#define waddstr(w,a) __pnc_addstr( w, a )
#define mvwaddch(w,a,b,c) __pnc_mvaddch( w, a, b, c )
#define mvwaddstr(w,a,b,c) __pnc_mvaddstr( w, a, b, c )
#define wattron(w,a) __pnc_attron( w, a )
#define wattroff(w,a) __pnc_attroff( w, a )
#define wbkgdset(w,a) __pnc_bkgdset( w, a )
#define wdeleteln(w) __pnc_deleteln( w )
#define wclrtoeol(w) __pnc_clrtoeol( w )
#define wclrtobot(w) __pnc_clrtobot( w )

#define scrollok(w,a) __pnc_scrollok( w, a )
#define noscroll() __pnc_scrollok( stdscr, false )
#define doupdate()

#define getch() __pnc_getch( stdscr )
#define wgetch(w) __pnc_getch( w )
#define erasechar() __pnc_erasechar()
#define killchar() __pnc_killchar()
#define raw() __pnc_raw()
#define nonl()

#define getyx(w,y,x) y=w->cy; x=w->cx;
#define getbegyx(w,y,x) y=w->y; x=w->x;
#define getmaxyx(w,y,x) y=w->h; x=w->w;

