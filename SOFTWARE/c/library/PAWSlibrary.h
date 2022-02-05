#ifndef __PAWSLIBRARY__
#include "PAWSdefinitions.h"
#define FAT_PRINTF_NOINC_STDIO
#ifdef feof
#undef feof
#endif
#include "fat_io_lib/fat_filelib.h"
#define FILE            FL_FILE

// MEMORY
extern unsigned char *MEMORYTOP;

// RISC-V CSR FUNCTIONS
//extern unsigned int CSRisa( void );
//extern unsigned long CSRcycles( void );
//extern unsigned long CSRinstructions( void );
//extern unsigned long CSRtime( void );

// SMT START AND STOP
//extern void SMTSTOP( void );
//extern void SMTSTART( unsigned int );
//extern unsigned char SMTSTATE( void );

// MINI DMA ENGINE
extern void *paws_memcpy( void *restrict destination, const void *restrict source, size_t count );
extern void *paws_memset( void *restrict destination, int value, size_t count );

// UART INPUT / OUTPUT
extern void uart_outputcharacter(char);
extern void uart_outputstring( char *);
extern char uart_inputcharacter( void );
//extern unsigned char uart_character_available( void );

// PS/2
//extern char ps2_character_available( void );
extern short ps2_inputcharacter( void );
//extern void ps2_keyboardmode( unsigned char );

// BASIC I/O
//extern void set_leds( unsigned char );
//extern unsigned char get_buttons( void );

// TIMERS AND PSEUDO RANDOM NUMBER GENERATOR
//extern float frng( void );
extern unsigned short rng( unsigned short );
extern void sleep1khz( unsigned short, unsigned char );
extern void set_timer1khz( unsigned short, unsigned char );
extern unsigned short get_timer1khz( unsigned char );
extern void wait_timer1khz( unsigned char );
extern unsigned short get_timer1hz( unsigned char );
extern void reset_timer1hz( unsigned char );
extern int paws_gettimeofday( struct paws_timeval *restrict tv, void *tz );

// AUDIO
extern void beep( unsigned char, unsigned char, unsigned char, unsigned short );
extern void await_beep( unsigned char );
extern unsigned short get_beep_active( unsigned char );

// DISPLAY
//extern void await_vblank( void );
//extern unsigned int total_frames( void );
//extern void screen_mode( unsigned char, unsigned char );
extern void bitmap_display( unsigned char );
extern void bitmap_draw( unsigned char );

// BACKGROUND GENERATOR
extern void set_background( unsigned char, unsigned char, unsigned char );
extern void copper_startstop( unsigned char ) ;
extern void copper_program( unsigned char, unsigned char, unsigned char, unsigned short, unsigned char, unsigned char, unsigned char );
//extern void set_copper_cpuinput( unsigned short );

// TILEMAP
extern void set_tilemap_tile( unsigned char tm_layer, unsigned char x, unsigned char y, unsigned char tile, unsigned char action );
extern void set_tilemap_bitmap( unsigned char tm_layer, unsigned char tile, unsigned char *bitmap );
extern void set_tilemap_bitmap32x32( unsigned char tm_layer, unsigned char tile, unsigned char *bitmap );
extern void set_tilemap_tile32x32( unsigned char tm_layer, short x, short y, unsigned char start_tile );
extern unsigned char tilemap_scrollwrapclear( unsigned char, unsigned char );

// GPU AND BITMAP
extern void gpu_dither( unsigned char , unsigned char );
extern void gpu_crop( unsigned short, unsigned short, unsigned short, unsigned short );
extern void gpu_pixel( unsigned char, short, short );
extern void gpu_rectangle( unsigned char, short, short, short, short );
extern void gpu_box( unsigned char, short, short, short, short, unsigned short );
extern void gpu_cs( void );
extern void gpu_line( unsigned char, short, short, short, short );
extern void gpu_wideline( unsigned char, short, short, short, short, unsigned short );
extern void gpu_circle( unsigned char, short, short, short, unsigned char, unsigned char );
extern void gpu_blit( unsigned char, short, short, short, unsigned char, unsigned char );
extern void gpu_character_blit( unsigned char, short, short, unsigned short, unsigned char, unsigned char );
extern void gpu_character_blit_shadow( unsigned char, unsigned char, short, short, unsigned char, unsigned char, unsigned char );
extern void gpu_colourblit( short, short, short, unsigned char, unsigned char );
extern void gpu_triangle( unsigned char, short, short, short, short, short, short );
extern void gpu_quadrilateral( unsigned char, short, short, short, short, short, short, short, short );
extern void draw_vector_block( unsigned char, unsigned char, short, short, unsigned char, unsigned char );
extern void set_vector_vertex( unsigned char, unsigned char , unsigned char, char, char );
extern void bitmap_scrollwrap( unsigned char );
extern void set_blitter_bitmap( unsigned char, unsigned short *);
extern void set_blitter_chbitmap( unsigned char, unsigned char *);
extern void set_colourblitter_bitmap( unsigned char, unsigned char *);
extern void gpu_pixelblock( short , short , unsigned short, unsigned short, unsigned char, unsigned char *);
extern void gpu_pixelblock24( short , short , unsigned short, unsigned short, unsigned char *);
extern void gpu_pixelblock_start( short , short , unsigned short );
extern void gpu_pixelblock_mode( unsigned char mode );
//extern void gpu_pixelblock_pixel( unsigned char );
//extern void gpu_pixelblock_pixel24( unsigned char, unsigned char, unsigned char );
//extern void gpu_pixelblock_stop( void );
//extern void gpu_pixelblock_remap( unsigned char from, unsigned char to );

extern void gpu_printf( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_printf_centre( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_printf_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_printf_centre_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_print( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);
extern void gpu_print_centre( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);
extern void gpu_print_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);
extern void gpu_print_centre_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);

// SOFTWARE VECTOR SHAPES
extern void DrawVectorShape2D( unsigned char, struct Point2D *, short, short, short, short, float );

// SOFTWARE DRAW LISTS
extern void DoDrawList2D( struct DrawList2D *, short, short, short, short, float );
extern void DoDrawList2Dscale( struct DrawList2D *, short, short, short, float );

// SPRITES - MAIN ACCESS
extern void set_sprite( unsigned char sprite_layer, unsigned char sprite_number, unsigned char active, short x, short y, unsigned char tile, unsigned char sprite_attributes );
extern short get_sprite_attribute( unsigned char, unsigned char , unsigned char );
extern void set_sprite_attribute( unsigned char, unsigned char, unsigned char, short );
extern void update_sprite( unsigned char, unsigned char, unsigned short );
extern unsigned short get_sprite_collision( unsigned char, unsigned char );
extern unsigned short get_sprite_layer_collision( unsigned char, unsigned char );
extern void set_sprite_bitmaps( unsigned char sprite_layer, unsigned char sprite_number, unsigned char *sprite_bitmaps );

// CHARACTER MAP
extern void tpu_cs( void );
extern void tpu_clearline( unsigned char );
extern void tpu_set(  unsigned char, unsigned char, unsigned char, unsigned char );
extern void tpu_output_character( short );
extern void tpu_printf( char, const char *,... );
extern void tpu_printf_centre( unsigned char, unsigned char, unsigned char, char, const char *,... );
extern void tpu_print( char, char *);
extern void tpu_print_centre( unsigned char, unsigned char, unsigned char, char, char *);

// TERMINAL WINDOW
extern void terminal_cs( void );
extern void terminal_showhide( unsigned char );
extern void terminal_output_character( char );
extern void terminal_print( char *);
extern void terminal_printf( const char *,... );

// IMAGE DECODERS
extern void netppm_display( unsigned char *, unsigned char );
extern void netppm_decoder( unsigned char *, unsigned char *);

// nanojpeg.c from https://keyj.emphy.de/nanojpeg/
#ifndef _NANOJPEG_H
typedef enum _nj_result {
    NJ_OK = 0,        // no error, decoding successful
    NJ_NO_JPEG,       // not a JPEG file
    NJ_UNSUPPORTED,   // unsupported format
    NJ_OUT_OF_MEM,    // out of memory
    NJ_INTERNAL_ERR,  // internal error
    NJ_SYNTAX_ERROR,  // syntax error
    __NJ_FINISHED,    // used internally, will never be reported
} nj_result_t;
extern void njInit(void);
extern nj_result_t njDecode(const void* jpeg, const int size);
extern int njGetWidth(void);
extern int njGetHeight(void);
extern int njIsColor(void);
extern unsigned char* njGetImage(void);
extern int njGetImageSize(void);
extern void njDone(void);
#endif

// SIMPLE CURSES
extern void initscr( void );
extern int endwin( void );
extern int refresh( void );
extern int clear( void );
extern void cbreak( void );
extern void echo( void );
extern void noecho( void );
extern void scroll( void );
extern void noscroll( void );
extern void curs_set( int );
extern void autorefresh( int );
extern int start_color( void );
extern bool has_colors( void );
extern bool can_change_color( void );
extern int init_pair( short pair, short f, short b );
extern int init_color( short color, short r, short g, short b );
extern int move( int y, int x );
extern void getyx( int *y, int *x );
extern int addch( unsigned char ch );
extern int mvaddch( int y, int x, unsigned char ch );
extern int printw( const char *fmt,... );
extern int mvprintw( int y, int x, const char *fmt,... );
extern int attron( int attrs );
extern int attroff( int attrs );
extern int deleteln( void );
extern int clrtoeol( void );
extern int intrflush( void *, bool );
extern int keypad( void *, bool );
extern void *stdscr;

// SDCARD using PAWS fat32
extern unsigned char *sdcard_selectfile( char *, char *, unsigned int *, char *);

// SDCARD using fat_io_lib
#ifndef	_SYS_STAT_H
#include <sys/types.h>
struct stat {
    dev_t     st_dev;     /* ID of device containing file */
    ino_t     st_ino;     /* inode number */
    mode_t    st_mode;    /* protection */
    nlink_t   st_nlink;   /* number of hard links */
    uid_t     st_uid;     /* user ID of owner */
    gid_t     st_gid;     /* group ID of owner */
    dev_t     st_rdev;    /* device ID (if special file) */
    off_t     st_size;    /* total size, in bytes */
    blksize_t st_blksize; /* blocksize for file system I/O */
    blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
    time_t    st_atime;   /* time of last access */
    time_t    st_mtime;   /* time of last modification */
    time_t    st_ctime;   /* time of last status change */
};
#endif

#define fopen(a,b)      paws_fopen(a, b)
#define fclose(a)       paws_fclose(a)
#define getc(a)         paws_fgetc(a)
#define fgetc(a)        paws_fgetc(a)
#define fgets(a,b,c)    paws_fgets(a, b, c)
#define fputc(a,b)      paws_fputc(a, b)
#define fputs(a,b)      paws_fputs(a, b)
#define fwrite(a,b,c,d) paws_fwrite(a, b, c, d)
#define fread(a,b,c,d)  paws_fread(a, b, c, d)
#define fseek(a,b,c)    fl_fseek(a, b, c)
#define fgetpos(a,b)    fl_fgetpos(a, b)
#define ftell(a)        fl_ftell(a)
#define feof(a)         fl_feof(a)
#define remove(a)       fl_remove(a)
#define mkdir(a)        fl_createdirectory(a)
#define rmdir(a)        0

extern void *paws_fopen( const char *path, const char *modifiers );
extern int paws_fclose( void *stream );
extern int paws_fgetc( void *fd );
extern char *paws_fgets( char *s, int cnt, void *fd );
extern int paws_fputc( int c, void *fd );
extern int paws_fputs( const char *s, void *fd );
extern int paws_fwrite(const void *data, int size, int count, void *fd );
extern int paws_fread( void *data, int size, int count, void *fd );

#define prinf paws_printf
#define fprintf paws_fprintf

extern int paws_printf(const char *restrict format, ... );
extern int paws_fprintf( void *fd, const char *restrict format, ... );

// INLINE SMALL FUNCTIONS FOR SPEED

// DISPLAY
extern unsigned char volatile *VBLANK;
extern unsigned int volatile *FRAMES;
extern unsigned char volatile *SCREENMODE;
extern unsigned char volatile *COLOUR;
extern unsigned char volatile *PB_COLOUR;
extern unsigned char volatile *PB_COLOUR8R;
extern unsigned char volatile *PB_COLOUR8G;
extern unsigned char volatile *PB_COLOUR8B;
extern unsigned char volatile *PB_STOP;
extern unsigned char volatile *PB_CMNUMBER;
extern unsigned char volatile *PB_CMENTRY;
extern unsigned short volatile *BACKGROUND_COPPER_CPUINPUT;

static inline void await_vblank( void ) {
    while( !*VBLANK );
}

static inline unsigned int total_frames( void ) {
    return( *FRAMES );
}

static inline void screen_mode( unsigned char screenmode, unsigned char colour ) {
    *SCREENMODE = screenmode;
    *COLOUR = colour;
}

static inline void gpu_pixelblock_pixel( unsigned char pixel ) {
    *PB_COLOUR = pixel;
}
static inline void gpu_pixelblock_pixel24( unsigned char red, unsigned char green, unsigned char blue ) {
    *PB_COLOUR8R = red;
    *PB_COLOUR8G= green;
    *PB_COLOUR8B = blue;
}
static inline void gpu_pixelblock_stop( void ) {
    *PB_STOP = 3;
}
static inline void gpu_pixelblock_remap( unsigned char from, unsigned char to ) {
    *PB_CMNUMBER = from;
    *PB_CMENTRY = to;
}

static inline void set_copper_cpuinput( unsigned short value ) {
    *BACKGROUND_COPPER_CPUINPUT = value;
}

// RISC-V CSR FUNCTIONS
static inline unsigned int CSRisa() {
   unsigned int isa;
   asm volatile ("csrr %0, 0x301" : "=r"(isa));
   return isa;
}

static inline unsigned long CSRcycles() {
   unsigned long cycles;
   asm volatile ("rdcycle %0" : "=r"(cycles));
   return cycles;
}

static inline unsigned long CSRinstructions() {
   unsigned long insns;
   asm volatile ("rdinstret %0" : "=r"(insns));
   return insns;
}

static inline unsigned long CSRtime() {
  unsigned long time;
  asm volatile ("rdtime %0" : "=r"(time));
  return time;
}

extern float volatile *FRNG;
static inline float frng( void ) {
    return( *FRNG );
}

// I/O
extern unsigned char volatile *UART_STATUS;
extern unsigned char volatile *PS2_AVAILABLE;
extern unsigned char volatile *PS2_MODE;
extern unsigned short volatile *BUTTONS;
extern unsigned char volatile *LEDS;
static inline unsigned char uart_character_available( void ) {
    return( *UART_STATUS & 1 );
}
static inline char ps2_character_available( void ) {
    return *PS2_AVAILABLE;
}
static inline void ps2_keyboardmode( unsigned char mode ) {
    *PS2_MODE = mode;
}
static inline void set_leds( unsigned char value ) {
    *LEDS = value;
}
static inline unsigned short get_buttons( void ) {
    return( *BUTTONS );
}

// SMT AND DMA
extern unsigned char volatile *SMTSTATUS;
extern unsigned int volatile *SMTPC;
extern unsigned int volatile *DMASOURCE;
extern unsigned int volatile *DMADEST;
extern unsigned int volatile *DMACOUNT;
extern unsigned char volatile *DMAMODE;
extern unsigned char volatile *DMASET;

static inline void SMTSTOP( void ) {
    *SMTSTATUS = 0;
}

static inline void SMTSTART( unsigned int code ) {
    *SMTPC = code;
    *SMTSTATUS = 1;
}

static inline unsigned char SMTSTATE( void ) {
    return( *SMTSTATUS );
}

//static inline void *paws_memcpy( void *restrict destination, const void *restrict source, size_t count ) {
//    *DMASOURCE = (unsigned int)source;
//    *DMADEST = (unsigned int)destination;
//    *DMACOUNT = count;
//    *DMAMODE = 3;
//    return( destination );
//}

//static inline void *paws_memset( void *restrict destination, int value, size_t count ) {
//    *DMASET = value;
//   *DMASOURCE = (unsigned int)DMASET;
//    *DMADEST = (unsigned int)destination;
//    *DMACOUNT = count;
//    *DMAMODE = 4;
//    return( destination );
//}

#define memcpy(a,b,c)   paws_memcpy(a,b,c)
#define memset(a,b,c)   paws_memset(a,b,c)

//TIMERS
extern unsigned short volatile *SYSTEMSECONDS;
extern unsigned int volatile *SYSTEMMILLISECONDS;
static inline unsigned short systemclock( void ) {
    return( *SYSTEMSECONDS );
}

#define __PAWSLIBRARY__
#endif
