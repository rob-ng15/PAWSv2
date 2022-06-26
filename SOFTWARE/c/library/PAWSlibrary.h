#ifndef __PAWSLIBRARY__
#include "PAWSdefinitions.h"

// MEMORY
extern unsigned char *MEMORYTOP;

// RISC-V CSR FUNCTIONS
extern unsigned int CSRisa( void );
extern unsigned long CSRcycles( void );
extern unsigned long CSRinstructions( void );
extern unsigned long CSRtime( void );

// SMT START AND STOP
extern void SMTSTOP( void );
extern void SMTSTART( unsigned int );
extern unsigned char SMTSTATE( void );

// UART INPUT / OUTPUT
extern void uart_outputcharacter(char);
extern void uart_outputstring( char *);
extern char uart_inputcharacter( void );
extern unsigned char uart_character_available( void );

// PS/2
extern char ps2_event_available( void );
extern short ps2_event_get( void );
extern unsigned char ps2_character_available( void );
extern unsigned short ps2_inputcharacter( void );
extern void ps2_keyboardmode( unsigned char mode );

// BASIC I/O
extern void set_leds( unsigned char );
extern unsigned short get_buttons( void );

// TIMERS AND PSEUDO RANDOM NUMBER GENERATOR
extern float frng( void );
extern unsigned short rng( unsigned short );
extern void sleep1khz( unsigned short, unsigned char );
extern void set_timer1khz( unsigned short, unsigned char );
extern unsigned short get_timer1khz( unsigned char );
extern void wait_timer1khz( unsigned char );
extern unsigned short get_timer1hz( unsigned char );
extern void reset_timer1hz( unsigned char );
extern unsigned short systemclock( void );
extern int paws_gettimeofday( struct paws_timeval *restrict tv, void *tz );

// AUDIO
extern void beep( unsigned char, unsigned char, unsigned char, unsigned short );
extern void await_beep( unsigned char );
extern unsigned short get_beep_active( unsigned char );
extern void sample_upload( unsigned char channel_number, unsigned short length, unsigned char *samples );

// DISPLAY
 void await_vblank( void );
 void await_vblank_finish( void );
extern void screen_mode( unsigned char, unsigned char, unsigned char );
extern void screen_dimmer( unsigned char dimmerlevel );
extern void bitmap_display( unsigned char );
extern void bitmap_draw( unsigned char );
extern void bitmap_256( unsigned char );

// BACKGROUND GENERATOR
extern void set_background( unsigned char, unsigned char, unsigned char );
extern void copper_startstop( unsigned char ) ;
extern void copper_program( unsigned char, unsigned char, unsigned char, unsigned short, unsigned char, unsigned char, unsigned char );
 void set_copper_cpuinput( unsigned short );

// TILEMAP
extern void set_tilemap_tile( unsigned char tm_layer, unsigned char x, unsigned char y, unsigned char tile, unsigned char action );
extern unsigned short read_tilemap_tile(  unsigned char tm_layer, unsigned char x, unsigned char y );
extern void set_tilemap_bitmap( unsigned char tm_layer, unsigned char tile, unsigned char *bitmap );
extern void set_tilemap_bitmap32x32( unsigned char tm_layer, unsigned char tile, unsigned char *bitmap );
extern void set_tilemap_tile32x32( unsigned char tm_layer, short x, short y, unsigned char start_tile );
extern void set_tilemap_bitamps_from_spritesheet( unsigned char tm_layer, unsigned char *tile_bitmaps );
extern unsigned char tilemap_scrollwrapclear( unsigned char tm_layer, unsigned char action, unsigned char amount );

// GPU AND BITMAP
extern void gpu_dither( unsigned char , unsigned char );
extern void gpu_crop( unsigned short, unsigned short, unsigned short, unsigned short );
extern void gpu_pixel( unsigned char, short, short );
extern void gpu_rectangle( unsigned char, short, short, short, short );
extern void gpu_box( unsigned char, short, short, short, short, unsigned short );
extern void gpu_cs( void );
extern void gpu_line( unsigned char, short, short, short, short );
extern void gpu_wideline( unsigned char, short, short, short, short, unsigned char );
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
 void gpu_pixelblock_start( short , short , unsigned short );
 void gpu_pixelblock_mode( unsigned char mode );
 void gpu_pixelblock_pixel( unsigned char );
 void gpu_pixelblock_pixel24( unsigned char, unsigned char, unsigned char );
 void gpu_pixelblock_stop( void );
 void gpu_pixelblock_remap( unsigned char from, unsigned char to );

extern void gpu_printf( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_printf_centre( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_printf_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_printf_centre_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_print( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);
extern void gpu_print_centre( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);
extern void gpu_print_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);
extern void gpu_print_centre_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);

// SOFTWARE VECTOR SHAPES
extern void DrawVectorShape2D( unsigned char, union Point2D *, short, short, short, short, float );

// SOFTWARE DRAW LISTS
extern void DoDrawList2D( struct DrawList2D *, short, short, short, short, float );
extern void DoDrawList2Dscale( struct DrawList2D *, short, short, short, float );

// SPRITES - MAIN ACCESS
extern void set_sprite( unsigned char sprite_layer, unsigned char sprite_number, unsigned char active, short x, short y, unsigned char tile, unsigned char sprite_attributes );
extern void set_sprite32( unsigned char sprite_layer, unsigned char sprite_number, unsigned char active, short x, short y, unsigned char tile, unsigned char sprite_attributes );
extern short get_sprite_attribute( unsigned char, unsigned char , unsigned char );
extern void set_sprite_attribute( unsigned char, unsigned char, unsigned char, short );
extern void update_sprite( unsigned char, unsigned char, unsigned short );
extern unsigned short get_sprite_collision( unsigned char, unsigned char );
extern unsigned short get_sprite_layer_collision( unsigned char, unsigned char );
extern void set_sprite_bitmaps( unsigned char sprite_layer, unsigned char sprite_number, unsigned char *sprite_bitmaps );
extern void set_sprite_bitamps_from_spritesheet( unsigned char sprite_layer, unsigned char *sprite_bitmaps );
extern void set_sprite_bitamps_from_spritesheet32x32( unsigned char sprite_layer, unsigned char *sprite_bitmaps );

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
extern void terminal_outputstring( char *s );
extern void terminal_print( char *);
extern void terminal_printf( const char *,... );

// IMAGE DECODERS
extern void netppm_display( unsigned char *, unsigned char );
extern void netppm_decoder( unsigned char *, unsigned char *);

// SDCARD using PAWS fat32
extern unsigned char *sdcard_selectfile( char *, char *, unsigned int *, char *);

// ALLOCATION OF FAST BRAM MEMORY
extern void *malloc_bram( int size );

// FIXED POINT DIVISION 16.16 ACCELERATOR
extern int fixed_divide( int a, int b );

// HARDWARE REGISTER BASES
extern int volatile *IO_REGS;
extern int volatile *DMA_REGS;
extern int volatile *DMA_REGS_ALT;
extern int volatile *GPU_REGS;
extern int volatile *AUDIO_REGS;
extern int volatile *FIXED_REGS;
extern int volatile *TIMER_REGS;

#define __PAWSLIBRARY__
#endif
