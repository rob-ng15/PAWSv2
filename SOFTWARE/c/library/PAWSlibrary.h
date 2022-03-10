#ifndef __PAWSLIBRARY__
#include "PAWSdefinitions.h"

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
extern void sample_upload( unsigned char channel_number, unsigned short length, unsigned char *samples );

// DISPLAY
//extern void await_vblank( void );
extern void screen_mode( unsigned char, unsigned char, unsigned char );
extern void screen_dimmer( unsigned char dimmerlevel );
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
extern unsigned char tilemap_scrollwrapclear( unsigned char tm_layer, unsigned char action, unsigned char amount );

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
extern void bkgdset( int attrs );
extern int deleteln( void );
extern int clrtoeol( void );
extern int clrtobot( void );
extern int intrflush( void *, bool );
extern int keypad( void *, bool );
extern void *stdscr;

// SDCARD using PAWS fat32
extern unsigned char *sdcard_selectfile( char *, char *, unsigned int *, char *);

// INLINE SMALL FUNCTIONS FOR SPEED

// DISPLAY
extern unsigned char volatile *VBLANK;
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

//TIMERS
extern unsigned short volatile *SYSTEMSECONDS;
extern unsigned int volatile *SYSTEMMILLISECONDS;
static inline unsigned short systemclock( void ) {
    return( *SYSTEMSECONDS );
}


// BIT MANIPULATION INSTRUCTIONS INTRINSICS
static inline int _rv32_andn (int rs1, int rs2) { int rd; __asm__ ("andn %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_orn  (int rs1, int rs2) { int rd; __asm__ ("orn %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_xnor (int rs1, int rs2) { int rd; __asm__ ("xnor %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_clz   (int rs1) { int rd; __asm__ ("clz     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_ctz   (int rs1) { int rd; __asm__ ("ctz     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_cpop  (int rs1) { int rd; __asm__ ("cpop    %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

static inline int _rv32_sext_b(int rs1) { int rd; __asm__ ("sext.b  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_sext_h(int rs1) { int rd; __asm__ ("sext.h  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_zext_h(int rs1) { int rd; __asm__ ("zext.h  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

static inline int _rv32_min (int rs1, int rs2) { int rd; __asm__ ("min  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_minu(int rs1, int rs2) { int rd; __asm__ ("minu %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_max (int rs1, int rs2) { int rd; __asm__ ("max  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_maxu(int rs1, int rs2) { int rd; __asm__ ("maxu %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_bset (int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("bseti %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & rs2)); else __asm__ ("bset %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_bclr (int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("bclri %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & rs2)); else __asm__ ("bclr %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_binv (int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("binvi %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & rs2)); else __asm__ ("binv %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_bext (int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("bexti %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & rs2)); else __asm__ ("bext %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_rol    (int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("rori    %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & -rs2)); else __asm__ ("rol     %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_ror    (int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("rori    %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 &  rs2)); else __asm__ ("ror     %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_rev8   (int rs1)  { int rd; __asm__ ("rev8     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_orc_b    (int rs1)  { int rd; __asm__ ("orc.b     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

static inline int _rv32_clmul (int rs1, int rs2) { int rd; __asm__ ("clmul   %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_clmulh(int rs1, int rs2) { int rd; __asm__ ("clmulh  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_clmulr(int rs1, int rs2) { int rd; __asm__ ("clmulr  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_sh1add (int rs1, int rs2) { int rd; __asm__ ("sh1add %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_sh2add  (int rs1, int rs2) { int rd; __asm__ ("sh2add %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_sh3add (int rs1, int rs2) { int rd; __asm__ ("sh3add %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

#define __PAWSLIBRARY__
#endif
