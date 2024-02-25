#ifndef __PAWSLIBRARY__
#include <stddef.h>
#include <PAWSintrinsics.h>
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
extern void SMTSTART( void * );
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
extern void get_mouse( short *x, short *y, short *buttons );

// TIMERS AND PSEUDO RANDOM NUMBER GENERATOR
extern float frng( void );
extern unsigned short rng( unsigned short );
extern void sleep1khz( unsigned short, unsigned char );
extern void set_timer1khz( unsigned short, unsigned char );
extern unsigned short get_timer1khz( unsigned char );
extern void wait_timer1khz( unsigned char );
extern unsigned short get_timer1hz( unsigned char );
extern void reset_timer1hz( unsigned char );
extern unsigned long systemclock( void );
#ifdef __cplusplus
extern int paws_gettimeofday( struct paws_timeval *, void * );
#else
extern int paws_gettimeofday( struct paws_timeval *restrict tv, void *tz );
#endif
extern unsigned long get_systemrtc( void );

// AUDIO
extern void beep( unsigned char, unsigned char, unsigned char, unsigned short );
extern void set_volume( unsigned char left, unsigned char right );
extern void await_beep( unsigned char );
extern unsigned short get_beep_active( unsigned char );
extern void tune_upload( unsigned char channel_number, unsigned short length, unsigned char *samples );
extern void bitsample_upload_128( unsigned char channel_number, unsigned char *samples );
extern void wavesample_upload( unsigned char channel_number, unsigned char *samples );
extern void pcmsample_upload( unsigned char channel_number, unsigned short count, unsigned char *samples );

// DISPLAY
extern int is_vblank( void );
extern void await_vblank( void );
extern void await_vblank_finish( void );
extern void screen_mode( unsigned char, unsigned char, unsigned char );
extern void screen_dimmer( unsigned char dimmerlevel );
extern void bitmap_display( unsigned char );
extern void bitmap_draw( unsigned char );
extern void bitmap_256( unsigned char mode );
extern void set_palette( unsigned char entry, unsigned int rgb );
extern void use_palette( unsigned char mode );
extern void status_lights( unsigned char display, unsigned char background );

// BACKGROUND GENERATOR
extern void set_background( unsigned char, unsigned char, unsigned char );
extern void copper_startstop( unsigned char ) ;
extern void copper_program( unsigned short address, unsigned char command, unsigned char reg1, unsigned char flag, unsigned short reg2 );
extern void copper_set_memory( unsigned short *memory );
extern void set_copper_cpuinput( unsigned short );
extern unsigned short get_copper_cpuoutput( void );

// TILEMAP
extern void set_tilemap_tile( unsigned char tm_layer, unsigned char x, unsigned char y, unsigned char tile, unsigned char action );
extern void set_tilemap_32x32tile( unsigned char tm_layer, short x, short y, unsigned char start_tile );
extern void set_tilemap_16x32tile( unsigned char tm_layer, short x, short y, unsigned char start_tile );
extern unsigned short read_tilemap_tile(  unsigned char tm_layer, unsigned char x, unsigned char y );
extern void set_tilemap_bitmap( unsigned char tm_layer, unsigned char tile, unsigned char *bitmap );
extern void set_tilemap_bitmap32x32( unsigned char tm_layer, unsigned char tile, unsigned char *bitmap );
extern void set_tilemap_tile32x32( unsigned char tm_layer, short x, short y, unsigned char start_tile );
extern void set_tilemap_bitamps_from_spritesheet( unsigned char tm_layer, unsigned char *tile_bitmaps );
extern unsigned char tilemap_scrollwrapclear( unsigned char tm_layer, unsigned char action, unsigned char amount );
extern void tilemap_setbase( unsigned char tm_layer, short x, short y );

// GPU AND BITMAP
extern void gpu_dither( unsigned char , unsigned char );
extern void gpu_crop( unsigned short, unsigned short, unsigned short, unsigned short );
extern void gpu_pixel( unsigned char, short, short );
extern void gpu_pixel_RGB( unsigned int colour, short x, short y );
extern void gpu_rectangle( unsigned char, short, short, short, short );
extern void gpu_box( unsigned char, short, short, short, short, unsigned short );
extern void gpu_cs( void );
extern void gpu_line( unsigned char, short, short, short, short );
extern void gpu_wideline( unsigned char, short, short, short, short, unsigned char );
extern void gpu_circle( unsigned char, short, short, short, unsigned char, unsigned char );
extern void gpu_ellipse( unsigned char colour, short xc, short yc, short radius_x, short radius_y, int filled );
extern void gpu_blit( unsigned char, short, short, short, unsigned char, unsigned char );
extern void gpu_character_blit( unsigned char, short, short, unsigned short, unsigned char, unsigned char );
extern void gpu_character_blit_shadow( unsigned char, unsigned char, short, short, unsigned char, unsigned char, unsigned char );
extern void gpu_colourblit( short, short, short, unsigned char, unsigned char );
extern void gpu_triangle( unsigned char, short, short, short, short, short, short );
extern void gpu_quadrilateral( unsigned char, short, short, short, short, short, short, short, short );
extern void set_blitter_bitmap( unsigned char, unsigned short *);
extern void set_blitter_chbitmap( unsigned char, unsigned char *);
extern void set_colourblitter_bitmap( unsigned char, unsigned char *);
extern void gpu_pixelblock( short , short , unsigned short, unsigned short, unsigned char, unsigned char *);
extern void gpu_pixelblock24( short , short , unsigned short, unsigned short, unsigned char *);
extern void gpu_pixelblockARGB( short x, short y, unsigned short w, unsigned short h, unsigned int *buffer );
extern void gpu_pixelblockRGBA( short x, short y, unsigned short w, unsigned short h, unsigned int *buffer );
extern void gpu_pixelblockABGR( short x, short y, unsigned short w, unsigned short h, unsigned int *buffer );
extern void gpu_pixelblockBGRA( short x, short y, unsigned short w, unsigned short h, unsigned int *buffer );
extern void gpu_pixelblock_start( short , short , unsigned short );
extern void gpu_pixelblock_mode( unsigned char mode );
extern void gpu_pixelblock_pixel( unsigned char );
extern void gpu_pixelblock_pixel24( unsigned char, unsigned char, unsigned char );
extern void gpu_pixelblock_pixelARGB( unsigned int ARGB );
extern void gpu_pixelblock_pixelRGBA( unsigned int RGBA );
extern void gpu_pixelblock_pixelABGR( unsigned int ABGR );
extern void gpu_pixelblock_pixelBGRA( unsigned int BGRA );
extern void gpu_pixelblock_stop( void );
extern void gpu_pixelblock_remap( unsigned char from, unsigned char to );

extern void gpu_printf( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_printf_centre( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_printf_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_printf_centre_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, const char *,...  );
extern void gpu_print( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);
extern void gpu_print_centre( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);
extern void gpu_print_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);
extern void gpu_print_centre_vertical( unsigned char, short, short, unsigned char, unsigned char, unsigned char, char *);

// PIXELBLOCK SCALEABLE SPRITES
extern void DrawBitmapSprite( short x, short y, float scale, bitmap_sprite sprite );
extern void DrawBitmapSpriteAtBaseRight( short x, short y, float scale, bitmap_sprite sprite );
extern void DrawBitmapSpriteAtCentre( short x, short y, float scale, bitmap_sprite sprite );

// SOFTWARE VECTOR SHAPES
extern void DrawVectorShape2D( unsigned char, union Point2D *, int, int, int, int, float );

// SOFTWARE DRAW LISTS
extern void DoDrawList2D( struct DrawList2D *, int, int, int, int, float );
extern void DoDrawList2Dscale( struct DrawList2D *, int, int, int, float );

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
extern void tpu_set(  unsigned char, unsigned char, unsigned char, unsigned char );
extern void tpu_move(  unsigned char, unsigned char );
extern void tpu_outputstring( char attribute, char *s );
extern void tpu_output_character( short );
extern void tpu_printf( char, const char *,... );
extern void tpu_printf_centre( unsigned char, unsigned char, unsigned char, char, const char *,... );
extern void tpu_print( char, char *);
extern void tpu_print_centre( unsigned char, unsigned char, unsigned char, char, char *);

// IMAGE DECODERS
extern void netppm_display( unsigned char *, unsigned char );
extern void netppm_decoder( unsigned char *, unsigned char *);

// SDCARD using PAWS fat32
extern unsigned char *sdcard_selectfile( char *, char *, unsigned int *, char *);

// EXTRA DMA MEMCPY MEMSET OPERATIONS
#ifdef __cplusplus
extern void paws_memcpy_step( const void *, const void *, size_t, int, int );
extern void paws_memcpy_rectangle( const void *, const void *, size_t, int, int, unsigned char );
extern void paws_memset_rectangle( void *, int, size_t, int, unsigned char );
extern void *paws_memset32( void *, int, size_t );
extern void paws_memset_rectangle32( void *, int, size_t, int, unsigned char );
#else
extern void paws_memcpy_step( const void *restrict destination, const void *restrict source, size_t count, int destadd, int sourceadd );
extern void paws_memcpy_rectangle( const void *restrict destination, const void *restrict source, size_t count, int destadd, int sourceadd, unsigned char cycles );
extern void paws_memset_rectangle( void *restrict destination, int value, size_t count, int destadd, unsigned char cycles );
extern void *paws_memset32( void *restrict destination, int value, size_t count );
extern void paws_memset_rectangle32( void *restrict destination, int value, size_t count, int destadd, unsigned char cycles );
#endif

#define __PAWSLIBRARY__
#endif
