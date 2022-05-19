#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include "PAWS.h"
#include "PAWSdefinitions.h"

// TOP OF SDRAM MEMORY
unsigned char *MEMORYTOP = (unsigned char *)0x8000000;;

// RISC-V CSR FUNCTIONS
unsigned int CSRisa() {
   unsigned int isa;
   asm volatile ("csrr %0, 0x301" : "=r"(isa));
   return isa;
}

unsigned long CSRcycles() {
   unsigned long cycles;
   asm volatile ("rdcycle %0" : "=r"(cycles));
   return cycles;
}

unsigned long CSRinstructions() {
   unsigned long insns;
   asm volatile ("rdinstret %0" : "=r"(insns));
   return insns;
}

unsigned long CSRtime() {
  unsigned long time;
  asm volatile ("rdtime %0" : "=r"(time));
  return time;
}

// SMT START STOP AND STATUS
void SMTSTOP( void ) {
    *SMTSTATUS = 0;
}

void SMTSTART( unsigned int code ) {
    *SMTPC = code;
    *SMTSTATUS = 1;
}

unsigned char SMTSTATE( void ) {
    return( *SMTSTATUS );
}

// DMA CONTROLLER
void DMASTART( const void *restrict source, void *restrict destination, unsigned int count, unsigned char mode ) {
    *DMASOURCE = (unsigned int)source;
    *DMADEST = (unsigned int)destination;
    *DMACOUNT = count;
    *DMAMODE = mode;
}

// PAWS MEMCPY USING THE DMA ENGINE - MODE 3 IS READ INCREMENT TO WRITE INCREMENT
void *paws_memcpy( void *restrict destination, const void *restrict source, size_t count ) {
    DMASTART( source, destination, count, 3 );
    return( destination );
}

// PAWS MEMSET USING THE DMA ENGINE - MODE 4 IS READ NO INCREMENT TO WRITE INCREMENT
void *paws_memset( void *restrict destination, int value, size_t count ) {
    *DMASET = (unsigned char)value;
    DMASTART( (const void *restrict)DMASET, destination, count, 4 );
    return( destination );
}

// OUTPUT TO UART
// OUTPUT INDIVIDUAL CHARACTER/STRING TO THE UART
void uart_outputcharacter(char c) {
	while( *UART_STATUS & 2 ) {}
    *UART_DATA = c;
    if( c == '\n' )
        uart_outputcharacter('\r');
}
void uart_outputstring( const char *s ) {
    while( *s ) {
        uart_outputcharacter( *s++ );
    }
}
// INPUT FROM UART
// RETURN 1 IF UART CHARACTER AVAILABLE, OTHERWISE 0
unsigned char uart_character_available( void ) {
    return( *UART_STATUS & 1 );
}
// RETURN CHARACTER FROM UART
char uart_inputcharacter( void ) {
	while( !uart_character_available() ) {}
    return *UART_DATA;
}

// TIMER AND PSEUDO RANDOM NUMBER GENERATOR

// PSEUDO RANDOM NUMBER GENERATOR
// RETURN FLOAT IN RANGE 0 <= frng < 1.0
float frng( void ) {
    return( *FRNG );
}

// RETURN PSEUDO RANDOM NUMBER 0 <= RNG < RANGE ( effectively 0 to range - 1 )
unsigned short rng( unsigned short range ) {
    unsigned short trial, mask;

    switch( range ) {
        case 0:
            trial = 0;
            break;

        case 1:
        case 2:
            trial = *ALT_RNG & 1;
            break;
        case 4:
            trial = *ALT_RNG & 3;
            break;
        case 8:
            trial = *ALT_RNG & 7;
            break;
        case 16:
            trial = *ALT_RNG & 15;
            break;
        case 32:
            trial = *ALT_RNG & 31;
            break;
        case 64:
            trial = *ALT_RNG & 63;
            break;

        default:
            if( range < 256 ) { mask = 255; }
            else if( range < 512 ) { mask = 511; }
            else if( range < 1024 ) { mask = 1023; }
            else { mask = 65535; }
            do {
                trial = *RNG & mask;
            } while ( trial >= range );
    }

    return( trial );
}

// SLEEP FOR counter milliseconds
void sleep1khz( unsigned short counter, unsigned char timer ) {
    switch( timer ) {
        case 0:
            *SLEEPTIMER0 = counter;
            while( *SLEEPTIMER0 );
            break;
        case 1:
            *SLEEPTIMER1 = counter;
            while( *SLEEPTIMER1 );
            break;
    }
}

// SET THE 1khz COUNTDOWN TIMER
void set_timer1khz( unsigned short counter, unsigned char timer ) {
    switch( timer ) {
        case 0:
            *TIMER1KHZ0 = counter;
            break;
        case 1:
            *TIMER1KHZ1 = counter;
            break;
    }
}

// READ THE 1khz COUNTDOWN TIMER
unsigned short get_timer1khz( unsigned char timer  ) {
    return( timer ? *TIMER1KHZ1 : *TIMER1KHZ0 );
}

// WAIT FOR THE 1khz COUNTDOWN TIMER
void wait_timer1khz( unsigned char timer  ) {
    while( timer ? *TIMER1KHZ1 : *TIMER1KHZ0 );
}

// READ THE 1hz TIMER
unsigned short get_timer1hz( unsigned char timer  ) {
    return( timer ? *TIMER1HZ1 : *TIMER1HZ0 );
}

// RESET THE 1hz TIMER
void reset_timer1hz( unsigned char timer  ) {
    switch( timer ) {
        case 0:
            *TIMER1HZ0 = 1;
            break;
        case 1:
            *TIMER1HZ1 = 1;
            break;
    }
}

// AUDIO OUTPUT
// START A note (1 == DEEP C, 25 == MIDDLE C )
// OF duration MILLISECONDS TO THE LEFT ( channel_number == 1 ) RIGHT ( channel_number == 2 ) or BOTH ( channel_number == 3 ) AUDIO CHANNEL
// IN waveform 0 == SQUARE, 1 == SAWTOOTH, 2 == TRIANGLE, 3 == SINE, 4 == WHITE NOISE, 7 == SAMPLE MODE
// 1 = C 2 or Deep C
// 13 = C 3
// 25 = C 4 or Middle C
// 37 = C 5 or Tenor C
// 49 = C 6 or Soprano C
// 61 = C 7 or Double High C
void beep( unsigned char channel_number, unsigned char waveform, unsigned char note, unsigned short duration ) {
    *AUDIO_WAVEFORM = waveform;
    *AUDIO_FREQUENCY = note;
    *AUDIO_DURATION = duration;
    *AUDIO_START = channel_number;
}

void await_beep( unsigned char channel_number ) {
    while( ( ( channel_number & 1) && *AUDIO_L_ACTIVE ) | ( ( channel_number & 2) && *AUDIO_R_ACTIVE ) ) {}
}

unsigned short get_beep_active( unsigned char channel_number ) {
    return( ( ( channel_number & 1) && *AUDIO_L_ACTIVE ) | ( ( channel_number & 2) && *AUDIO_R_ACTIVE ) );
}

// USES DOOM PC SPEAKER FORMAT SAMPLES - USE DMA MODE 1 multi-source to single-dest
void sample_upload( unsigned char channel_number, unsigned short length, unsigned char *samples ) {
    *AUDIO_NEW_SAMPLE = channel_number;
    if( channel_number & 1 ) { DMASTART( samples, (void *restrict)AUDIO_LEFT_SAMPLE, length, 1 ); }
    if( channel_number & 2 ) { DMASTART( samples, (void *restrict)AUDIO_RIGHT_SAMPLE, length, 1 ); }
}

// SDCARD FUNCTIONS
// INTERNAL FUNCTION - WAIT FOR THE SDCARD TO BE READY
void sdcard_wait( void ) {
    while( *SDCARD_READY == 0 ) {}
}

// READ A SECTOR FROM THE SDCARD AND COPY TO MEMORY
void sdcard_readsector( unsigned int sectorAddress, unsigned char *copyAddress ) {
    sdcard_wait();
    *SDCARD_SECTOR = sectorAddress;
    *SDCARD_RESET_BUFFERADDRESS = 0;                // WRITE ANY VALUE TO RESET THE BUFFER ADDRESS
    *SDCARD_READSTART = 1;
    sdcard_wait();

    // USE DMA CONTROLLER TO COPY THE DATA, MODE 4 COPIES FROM A SINGLE ADDRESS TO MULTIPLE
    // EACH READ OF THE SDCARD BUFFER INCREMENTS THE BUFFER ADDRESS
    DMASTART( (const void *restrict)SDCARD_DATA, copyAddress, 512, 4 );
}
// WRITE A SECTOR TO THE SDCARD COPIED FROM MEMORY
void sdcard_writesector( unsigned int sectorAddress, unsigned char *copyAddress ) {
    sdcard_wait();

    // USE DMA CONTROLLER TO COPY THE DATA, MODE 1 COPIES FROM MULTIPLE-ADDRESSES TO SINGLE ADDRESS
    // EACH WRITE OF THE SDCARD BUFFER INCREMENTS THE BUFFER ADDRESS
    *SDCARD_RESET_BUFFERADDRESS = 0;                // WRITE ANY VALUE TO RESET THE BUFFER ADDRESS
    DMASTART( copyAddress, (void *restrict)SDCARD_DATA, 512, 1 );

    *SDCARD_SECTOR = sectorAddress;
    *SDCARD_WRITESTART = 1;
    sdcard_wait();
}

// I/O FUNCTIONS
// SET THE LEDS
void set_leds( unsigned char value ) {
    *LEDS = value;
}

// READ THE ULX3S JOYSTICK BUTTONS OR KEYBOARD AS JOYSTICK
unsigned short get_buttons( void ) {
    return( *BUTTONS );
}

// DISPLAY FUNCTIONS
// FUNCTIONS ARE IN LAYER ORDER: BACKGROUND, TILEMAP, SPRITES (for LOWER ), BITMAP & GPU, ( UPPER SPRITES ), CHARACTERMAP & TPU
// colour is in the form { Arrggbb } { ALPHA - show layer below, RED, GREEN, BLUE } or { rrggbb } { RED, GREEN, BLUE } giving 64 colours + transparent
// INTERNAL FUNCTION - WAIT FOR THE GPU TO BE ABLE TO RECEIVE A NEW COMMAND
void wait_gpu( void ) {
    while( *GPU_STATUS );
}
// INTERNAL FUNCTION - WAIT FOR THE GPU TO FINISH THE ALL COMMANDS
void wait_gpu_finished( void ) {
    while( !*GPU_FINISHED );
}

// WAIT FOR VBLANK TO START/FINISH
void await_vblank( void ) {
    while( !*VBLANK );
}
void await_vblank_finish( void ) {
    while( *VBLANK );
}

// SET THE LAYER ORDER FOR THE DISPLAY
void screen_mode( unsigned char screenmode, unsigned char colour, unsigned char tmresolution ) {
    *SCREENMODE = screenmode;
    *COLOUR = colour;
    *REZ = tmresolution;
}

// SET THE DIMMER LEVEL FOR THE DISPLAY 0 == FULL BRIGHTNESS, 1 - 7 DIMMER, 8 - 15 BLANK
void screen_dimmer( unsigned char dimmerlevel ) {
    *DIMMER = dimmerlevel;
}

// SET THE FRAMEBUFFER TO DISPLAY / DRAW
void bitmap_display( unsigned char framebuffer ) {
    await_vblank();
    *FRAMEBUFFER_DISPLAY = framebuffer;
}

void bitmap_draw( unsigned char framebuffer ) {
    while( !*GPU_FINISHED );
    *FRAMEBUFFER_DRAW = framebuffer;
}

// BACKGROUND GENERATOR
// backgroundmode ==
//  0 SOLID in colour
//  1, 2, 3, 4 checkerboard in colour/altcolour in increasing sizes of squares
//  5 rainbow
//  6 static
//  7 @sylefeb's snow/starfield with colour stars and altcolour background
//  8 split vertical
//  9 split horizontal
//  10 quarters
void set_background( unsigned char colour, unsigned char altcolour, unsigned char backgroundmode ) {
    *BACKGROUND_COPPER_STARTSTOP = 0;
    *BACKGROUND_COLOUR = colour;
    *BACKGROUND_ALTCOLOUR = altcolour;
    *BACKGROUND_MODE = backgroundmode;
}

// BACKGROUND COPPER
void copper_startstop( unsigned char status ) {
    await_vblank();
    *BACKGROUND_COPPER_STARTSTOP = status;
}

struct copper_command {
    unsigned int command:3;
    unsigned int condition:3;
    unsigned int coordinate:11;
    unsigned int mode:4;
    unsigned int altcolour:7;
    unsigned int colour:7;
};

void copper_program( unsigned char address, unsigned char command, unsigned char condition, unsigned short coordinate, unsigned char mode, unsigned char altcolour, unsigned char colour ) {
    *BACKGROUND_COPPER_ADDRESS = address;
    *BACKGROUND_COPPER_COMMAND = command;
    *BACKGROUND_COPPER_CONDITION = condition;
    *BACKGROUND_COPPER_COORDINATE = coordinate;
    *BACKGROUND_COPPER_MODE = mode;
    *BACKGROUND_COPPER_ALT = altcolour;
    *BACKGROUND_COPPER_COLOUR = colour;
    *BACKGROUND_COPPER_PROGRAM = 1;
}

void set_copper_cpuinput( unsigned short value ) {
    *BACKGROUND_COPPER_CPUINPUT = value;
}

// SCROLLABLE TILEMAP
// The tilemap is 42 x 32, with 40 x 30 displayed, with an x and y offset in the range -15 to 15 to scroll the tilemap
// The tilemap can scroll or wrap once x or y is at -15 or 15

// SET THE TILEMAP TILE at (x,y) to tile
void set_tilemap_tile( unsigned char tm_layer, unsigned char x, unsigned char y, unsigned char tile, unsigned char action ) {
    switch( tm_layer ) {
        case 0:
            while( *LOWER_TM_STATUS );
            *LOWER_TM_X = x;
            *LOWER_TM_Y = y;
            *LOWER_TM_TILE = tile;
            *LOWER_TM_ACTION = action;
            *LOWER_TM_COMMIT = 1;
            break;
        case 1:
            while( *UPPER_TM_STATUS );
            *UPPER_TM_X = x;
            *UPPER_TM_Y = y;
            *UPPER_TM_TILE = tile;
            *UPPER_TM_ACTION = action;
            *UPPER_TM_COMMIT = 1;
            break;
    }
}

// SET THE TILE BITMAP for tile to the 16 x 16 pixel bitmap
void set_tilemap_bitmap( unsigned char tm_layer, unsigned char tile, unsigned char *bitmap ) {
    *( tm_layer ? UPPER_TM_WRITER_TILE_NUMBER : LOWER_TM_WRITER_TILE_NUMBER ) = tile;
    DMASTART( bitmap, (void *restrict)( tm_layer ? UPPER_TM_WRITER_COLOUR : LOWER_TM_WRITER_COLOUR ), 256, 1 );
}

// SET THE TILE BITMAP for 4 tiles to the 32 x 32 pixel bitmap
void set_tilemap_bitmap32x32( unsigned char tm_layer, unsigned char tile, unsigned char *bitmap ) {
    for( short i = 0; i < 4; i++ ) {
        if( tm_layer ) {
            *UPPER_TM_WRITER_TILE_NUMBER = tile + i;
        } else {
            *LOWER_TM_WRITER_TILE_NUMBER = tile + i;
        }
        for( short y = 0; y < 16; y++ ) {
            for( short x = 0; x < 16; x++ ) {
                *( tm_layer ? UPPER_TM_WRITER_COLOUR : LOWER_TM_WRITER_COLOUR ) = bitmap[ ( y + (i&1 ? 16 : 0) )* 32 + ( x + ( i>1 ? 16 : 0 ) ) ];
            }
        }
    }
}

// HELPER FOR PLACING A 4 TILE 32 x 32 TILE TO THE TILEMAPS
void set_tilemap_tile32x32( unsigned char tm_layer, short x, short y, unsigned char start_tile ) {
    set_tilemap_tile( tm_layer, x, y, start_tile, 0 );
    set_tilemap_tile( tm_layer, x, y + 1, start_tile + 1, 0 );
    set_tilemap_tile( tm_layer, x + 1, y, start_tile + 2,  0 );
    set_tilemap_tile( tm_layer, x + 1, y + 1, start_tile + 3, 0 );
}

// SCROLL WRAP or CLEAR the TILEMAP by amount ( 0 - 15 ) pixels
//  action == 1 to 4 move the tilemap amount pixels LEFT, UP, RIGHT, DOWN and SCROLL at limit
//  action == 5 to 8 move the tilemap amount pixels LEFT, UP, RIGHT, DOWN and WRAP at limit
//  action == 9 clear the tilemap
//  RETURNS 0 if no action taken other than pixel shift, action if SCROLL WRAP or CLEAR was actioned
unsigned char tilemap_scrollwrapclear( unsigned char tm_layer, unsigned char action, unsigned char amount ) {
    while( *( tm_layer ? UPPER_TM_STATUS : LOWER_TM_STATUS ) );
    *( tm_layer ? UPPER_TM_SCROLLWRAPAMOUNT : LOWER_TM_SCROLLAMOUNT ) = amount;
    *( tm_layer ? UPPER_TM_SCROLLWRAPCLEAR : LOWER_TM_SCROLLWRAPCLEAR ) = action;
    return( tm_layer ? *UPPER_TM_SCROLLWRAPCLEAR : *LOWER_TM_SCROLLWRAPCLEAR );
}

// GPU AND BITMAP
// The bitmap is 320 x 240 pixels (0,0) is top left
// The GPU can draw pixels, filled rectangles, lines, (filled) circles, filled triangles and has a 16 x 16 pixel blitter from user definable tiles

// SET GPU DITHER MODE AND ALTERNATIVE COLOUR
void gpu_dither( unsigned char mode, unsigned char colour ) {
    wait_gpu();
    *GPU_COLOUR_ALT = colour;
    *GPU_DITHERMODE = mode;
}

// SET GPU CROPPING RECTANGLE
void gpu_crop( short left, short top, short right, short bottom ) {
    wait_gpu();
    *CROP_LEFT = left < 0 ? 0 : left;
    *CROP_RIGHT = right > 319 ? 319 : right;
    *CROP_TOP = top < 0 ? 0 : top;
    *CROP_BOTTOM = bottom > 239 ? 239 : bottom;
}

// SET THE PIXEL at (x,y) to colour
void gpu_pixel( unsigned char colour, short x, short y ) {
    *GPU_COLOUR = colour;
    *GPU_X = x;
    *GPU_Y = y;
    wait_gpu();
    *GPU_WRITE = 1;
}

// DRAW A LINE FROM (x1,y1) to (x2,y2) in colour - uses Bresenham's Line Drawing Algorithm - single pixel width
void gpu_line( unsigned char colour, short x1, short y1, short x2, short y2 ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = x2;
    *GPU_PARAM1 = y2;
    *GPU_PARAM2 = 1;
    wait_gpu();
    *GPU_WRITE = 2;
}

// DRAW A LINE FROM (x1,y1) to (x2,y2) in colour - uses Bresenham's Line Drawing Algorithm - pixel width
void gpu_wideline( unsigned char colour, short x1, short y1, short x2, short y2, unsigned char width ) {
    if( width != 0 ) {
        *GPU_COLOUR = colour;
        *GPU_X = x1;
        *GPU_Y = y1;
        *GPU_PARAM0 = x2;
        *GPU_PARAM1 = y2;
        *GPU_PARAM2 = width;
        wait_gpu();
        *GPU_WRITE = 2;
    }
}

// DRAW AN OUTLINE RECTANGLE from (x1,y1) to (x2,y2) in colour with width pixel lines
void gpu_box( unsigned char colour, short x1, short y1, short x2, short y2, unsigned short width ) {
    gpu_wideline( colour, x1, y1, x2, y1, width );
    gpu_wideline( colour, x2, y1, x2, y2, width );
    gpu_wideline( colour, x2, y2, x1, y2, width );
    gpu_wideline( colour, x1, y2, x1, y1, width );
}

// DRAW AN FILLED RECTANGLE from (x1,y1) to (x2,y2) in colour
void gpu_rectangle( unsigned char colour, short x1, short y1, short x2, short y2 ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = x2;
    *GPU_PARAM1 = y2;
    wait_gpu();
    *GPU_WRITE = 3;
}

// CLEAR THE BITMAP by drawing a transparent rectangle from (0,0) to (319,239) and resetting the dither pattern
void gpu_cs( void ) {
    gpu_dither( 0, TRANSPARENT ); gpu_rectangle( TRANSPARENT, 0, 0, 319, 239 );
}


// DRAW A (optional filled) CIRCLE at centre (x1,y1) of radius
void gpu_circle( unsigned char colour, short x1, short y1, short radius, unsigned char drawsectors, unsigned char filled ) {
    if( radius != 0 ) {
        *GPU_COLOUR = colour;
        *GPU_X = x1;
        *GPU_Y = y1;
        *GPU_PARAM0 = radius;
        *GPU_PARAM1 = drawsectors;
        wait_gpu();
        *GPU_WRITE = filled ? 5 : 4;
    }
}

// BLIT A 16 x 16 ( blit_size == 1 doubled to 32 x 32 ) TILE ( from tile 0 to 31 ) to (x1,y1) in colour
// REFLECT { y, x }
void gpu_blit( unsigned char colour, short x1, short y1, short tile, unsigned char blit_size, unsigned char action ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = tile;
    *GPU_PARAM1 = blit_size;
    *GPU_PARAM2 = action;
    wait_gpu();
    *GPU_WRITE = 7;
}

// BLIT AN 8 x8  ( blit_size == 1 doubled to 16 x 16, blit_size == 1 doubled to 32 x 32 ) CHARACTER ( from tile 0 to 255 ) to (x1,y1) in colour
// REFLECT { y, x }
void gpu_character_blit( unsigned char colour, short x1, short y1, unsigned short tile, unsigned char blit_size, unsigned char action ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = tile;
    *GPU_PARAM1 = blit_size;
    *GPU_PARAM2 = action;
    wait_gpu();
    *GPU_WRITE = 8;
}

// COLOURBLIT A 16 x 16 ( blit_size == 1 doubled to 32 x 32 ) TILE ( from tile 0 to 31 ) to (x1,y1)
// { rotate/reflect, ACTION } ROTATION == 4 0 == 5 90 == 6 180 == 7 270
// == 1 REFLECT X, == 2 REFLECT Y
void gpu_colourblit( short x1, short y1, short tile, unsigned char blit_size, unsigned char action ) {
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = tile;
    *GPU_PARAM1 = blit_size;
    *GPU_PARAM2 = action;
    wait_gpu();
    *GPU_WRITE = 9;
}

// SET THE BLITTER TILE to the 16 x 16 pixel bitmap
void set_blitter_bitmap( unsigned char tile, unsigned short *bitmap ) {
    *BLIT_WRITER_TILE = tile;

    for( int i = 0; i < 16; i ++ ) {
        *BLIT_WRITER_BITMAP = bitmap[i];
    }
}

// SET THE BLITTER CHARACTER TILE to the 8 x 8 pixel bitmap
void set_blitter_chbitmap( unsigned char tile, unsigned char *bitmap ) {
    *BLIT_CHWRITER_TILE = tile;

    for( int i = 0; i < 8; i ++ ) {
        *BLIT_CHWRITER_BITMAP = bitmap[i];
    }
}

// SET THE COLOURBLITTER TILE to the 16 x 16 pixel bitmap
void set_colourblitter_bitmap( unsigned char tile, unsigned char *bitmap ) {
    *COLOURBLIT_WRITER_TILE = tile;
    DMASTART( bitmap, (void *restrict)COLOURBLIT_WRITER_COLOUR, 256, 1 );
}

// DRAW A FILLED TRIANGLE with vertices (x1,y1) (x2,y2) (x3,y3) in colour
// VERTICES SHOULD BE PRESENTED CLOCKWISE FROM THE TOP ( minimal adjustments made to the vertices to comply )
void gpu_triangle( unsigned char colour, short x1, short y1, short x2, short y2, short x3, short y3 ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = x2;
    *GPU_PARAM1 = y2;
    *GPU_PARAM2 = x3;
    *GPU_PARAM3 = y3;
    wait_gpu();
    *GPU_WRITE = 6;
}

// DRAW A FILLED QUADRILATERAL with vertices (x1,y1) (x2,y2) (x3,y3) (x4,y4) in colour BY DRAWING TWO FILLED TRIANGLES
// VERTICES SHOULD BE PRESENTED CLOCKWISE FROM THE TOP ( minimal adjustments made to the vertices to comply )
void gpu_quadrilateral( unsigned char colour, short x1, short y1, short x2, short y2, short x3, short y3, short x4, short y4 ) {
     *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = x2;
    *GPU_PARAM1 = y2;
    *GPU_PARAM2 = x3;
    *GPU_PARAM3 = y3;
    *GPU_PARAM4 = x4;
    *GPU_PARAM5 = y4;
    wait_gpu();
    *GPU_WRITE = 15;
}

// OUTPUT A STRING TO THE GPU
void gpu_print( unsigned char colour, short x, short y, unsigned char bold, unsigned char size, unsigned char action, char *s ) {
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256:0 ) + *s++, size, action );
        x = x + ( 8 << size );
    }
}
void gpu_print_vertical( unsigned char colour, short x, short y, unsigned char bold, unsigned char size, unsigned char action, char *s ) {
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256:0 ) + *s++, size, action );
        y = y - ( 8 << size );
    }
}
void gpu_printf( unsigned char colour, short x, short y, unsigned char bold, unsigned char size, unsigned char action, const char *fmt,... ) {
    char *buffer = (char *)0x1000;
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 80, fmt, args);
    va_end(args);

    char *s = buffer;
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256:0 ) + *s++, size, action );
        x = x + ( 8 << size );
    }
}
void gpu_printf_vertical( unsigned char colour, short x, short y, unsigned char bold, unsigned char size, unsigned char action, const char *fmt,... ) {
    char *buffer = (char *)0x1000;
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 80, fmt, args);
    va_end(args);

    char *s = buffer;
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256:0 ) + *s++, size, action );
        y = y - ( 8 << size );
    }
}

// OUTPUT A STRING TO THE GPU - CENTRED AT ( x, y )
void gpu_printf_centre( unsigned char colour, short x, short y, unsigned char bold, unsigned char size, unsigned char action, const char *fmt,... ) {
    char *buffer = (char *)0x1000;
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 80, fmt, args);
    va_end(args);

    char *s = buffer;
    x = x - ( ( strlen( s ) * ( 8 << size ) ) /2 );
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256:0 ) + *s++, size, action );
        x = x + ( 8 << size );
    }
}
void gpu_printf_centre_vertical( unsigned char colour, short x, short y, unsigned char bold, unsigned char size, unsigned char action, const char *fmt,... ) {
    char *buffer = (char *)0x1000;
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 80, fmt, args);
    va_end(args);

    char *s = buffer;
    y = y + ( ( strlen( s ) * ( 8 << size ) ) /2 );
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256:0 ) + *s++, size, action );
        y = y - ( 8 << size );
    }
}
void gpu_print_centre( unsigned char colour, short x, short y, unsigned char bold, unsigned char size, unsigned char action, char *s ) {
    x = x - ( ( strlen( s ) * ( 8 << size ) ) /2 );
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256:0 ) + *s++, size, action );
        x = x + ( 8 << size );
    }
}void gpu_print_centre_vertical( unsigned char colour, short x, short y, unsigned char bold, unsigned char size, unsigned char action, char *s ) {
    y = y + ( ( strlen( s ) * ( 8 << size ) ) /2 );
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256:0 ) + *s++, size, action );
        y = y - ( 8 << size );
    }
}
// PB_MODE = 0 COPY A ARRGGBB BITMAP STORED IN MEMORY TO THE BITMAP USING THE PIXEL BLOCK
// PB_MODE = 1 COPY A 256 COLOUR BITMAP STORED IN MEMORY TO THE BITMAP USING THE PIXEL BLOCK AND REMAPPER
void gpu_pixelblock( short x,  short y, unsigned short w, unsigned short h, unsigned char transparent, unsigned char *buffer ) {
    wait_gpu_finished();
    *GPU_X = x;
    *GPU_Y = y;
    *GPU_PARAM0 = w;
    *GPU_PARAM1 = transparent;

    *GPU_WRITE = 10;

    // USE THE DMA CONTROLLER TO TRANSFER THE PIXELS
    if( ( (int)buffer & 1 ) || ( ( w*h ) & 1 ) ) {
        DMASTART( buffer, (void *)PB_COLOUR, w*h, 1 );      // UNALIGNED, ODD NUMBER OF PIXELS USE 8 BIT MODE
    } else {
        DMASTART( buffer, (void *)PB_COLOUR, w*h/2, 7 );    // ALIGNED, EVEN PIXELS USE 16 BIT MODE
    }
    *PB_STOP = 3;
}

// PB_MODE = 0 COPY A { RRRRRRRR GGGGGGGG BBBBBBBB } BITMAP STORED IN MEMORY TO THE BITMAP USING THE PIXEL BLOCK
// PB_MODE = 1 SAME BUT CONVERT TO GREYSCALE
void gpu_pixelblock24( short x, short y, unsigned short w, unsigned short h, unsigned char *buffer  ) {
    wait_gpu_finished();
    *GPU_X = x;
    *GPU_Y = y;
    *GPU_PARAM0 = w;
    *GPU_WRITE = 10;

    // USE THE DMA CONTROLLER TO TRANSFER THE PIXELS
    DMASTART( buffer, (void *)PB_COLOUR8R, 3*w*h, 2 );
    *PB_STOP = 3;
}

// SET GPU TO RECEIVE A PIXEL BLOCK, SEND INDIVIDUAL PIXELS, STOP
void gpu_pixelblock_start( short x,  short y, unsigned short w ) {
    wait_gpu_finished();
    *GPU_X = x;
    *GPU_Y = y;
    *GPU_PARAM0 = w;
    *GPU_PARAM1 = TRANSPARENT;
    *GPU_WRITE = 10;
}
void gpu_pixelblock_pixel( unsigned char pixel ) {
    *PB_COLOUR = pixel;
}

void gpu_pixelblock_pixel24( unsigned char red, unsigned char green, unsigned char blue ) {
    *PB_COLOUR8R = red;
    *PB_COLOUR8G= green;
    *PB_COLOUR8B = blue;
}

void gpu_pixelblock_stop( void ) {
    *PB_STOP = 3;
}

// SWITCH BETWEEN PAWSv2 COLOURS AND THE REMAPPER, OR GRRGGBB OR GREYSCALE
void gpu_pixelblock_mode( unsigned char mode ) {
    *PB_MODE = mode;
}

// SET AN ENTRY IN THE REMAPPER
void gpu_pixelblock_remap( unsigned char from, unsigned char to ) {
    *PB_CMNUMBER = from;
    *PB_CMENTRY = to;
}

// GPU VECTOR BLOCK
// 32 VECTOR BLOCKS EACH OF 16 VERTICES ( offsets in the range -15 to 15 from the centre )
// WHEN ACTIVATED draws lines from a vector block (x0,y0) to (x1,y1), (x1,y1) to (x2,y2), (x2,y2) to (x3,y3) until (x15,y15) or an inactive vertex is encountered

// START DRAWING A VECTOR BLOCK centred at (xc,yc) in colour
// { rotate/reflect, ACTION } ROTATION == 4 0 == 5 90 == 6 180 == 7 270
// == 1 REFLECT X, == 2 REFLECT Y
void draw_vector_block( unsigned char block, unsigned char colour, short xc, short yc, unsigned char scale, unsigned char action ) {
    while( *VECTOR_DRAW_STATUS );
    *VECTOR_DRAW_BLOCK = block;
    *VECTOR_DRAW_COLOUR = colour;
    *VECTOR_DRAW_XC = xc;
    *VECTOR_DRAW_YC = yc;
    *VECTOR_DRAW_SCALE = scale;
    *VECTOR_DRAW_ACTION = action;
    *VECTOR_DRAW_START = 1;
}

// SET A VERTEX IN A VECTOR BLOCK - SET AN INACTIVE VERTEX IF NOT ALL 16 VERTICES ARE TO BE USED
void set_vector_vertex( unsigned char block, unsigned char vertex, unsigned char active, char deltax, char deltay ) {
    *VECTOR_WRITER_BLOCK = block;
    *VECTOR_WRITER_VERTEX = vertex;
    *VECTOR_WRITER_ACTIVE = active;
    *VECTOR_WRITER_DELTAX = deltax;
    *VECTOR_WRITER_DELTAY = deltay;
}

// SOFTWARE VECTORS AND DRAWLISTS

// SCALE A POINT AND MOVE TO CENTRE POINT
struct Point2D Scale2D( struct Point2D point, short xc, short yc, float scale ) {
    struct Point2D newpoint;
    newpoint.dx = point.dx * scale + xc;
    newpoint.dy = point.dy * scale + yc;
    return( newpoint );
}
struct Point2D Rotate2D( struct Point2D point, short xc, short yc, short angle, float scale ) {
    struct Point2D newpoint;
    float radians = angle*0.01745329252;

    newpoint.dx = ( (point.dx * scale)*cosf(radians)-(point.dy * scale)*sinf(radians) ) + xc;
    newpoint.dy = ( (point.dx * scale)*sinf(radians)+(point.dy * scale)*cosf(radians) ) + yc;

    return( newpoint );
}

struct Point2D MakePoint2D( short x, short y ) {
    struct Point2D newpoint;
    newpoint.dx = x; newpoint.dy = y;
    return( newpoint );
}

// PROCESS A SOFTWARE VECTOR BLOCK AFTER SCALING AND ROTATION
void DrawVectorShape2D( unsigned char colour, struct Point2D *points, short numpoints, short xc, short yc, short angle, float scale ) {
    struct Point2D *NewShape  = (struct Point2D *)0x1800;
    for( short vertex = 0; vertex < numpoints; vertex++ ) {
        NewShape[ vertex ] = Rotate2D( points[vertex], xc, yc, angle, scale );
    }
    for( short vertex = 0; vertex < numpoints; vertex++ ) {
        gpu_line( colour, NewShape[ vertex ].dx, NewShape[ vertex ].dy, NewShape[ ( vertex == ( numpoints - 1 ) ) ? 0 : vertex + 1 ].dx, NewShape[ vertex == ( numpoints - 1 ) ? 0 : vertex + 1 ].dy );
    }
}

// PROCESS A DRAWLIST DRAWING SHAPES AFTER SCALING, ROTATING AND MOVING TO CENTRE POINT
void DoDrawList2D( struct DrawList2D *list, short numentries, short xc, short yc, short angle, float scale ) {
    struct Point2D XY1, XY2, XY3, XY4;
    for( int i = 0; i < numentries; i++ ) {
        gpu_dither( list[i].dithermode, list[i].alt_colour );
        switch( list[i].shape ) {
            case DLLINE:
                XY1 = Rotate2D( list[i].xy1, xc, yc, angle, scale );
                XY2 = Rotate2D( list[i].xy2, xc, yc, angle, scale );
                gpu_wideline( list[i].colour, XY1.dx, XY1.dy, XY2.dx, XY2.dy, list[i].xy3.dx * scale );
                break;
            case DLRECT:
                // CONVERT TO QUADRILATERAL
                XY1 = Rotate2D( list[i].xy1, xc, yc, angle, scale );
                XY2 = Rotate2D( MakePoint2D( list[i].xy2.dx, list[i].xy1.dy ), xc, yc, angle, scale );
                XY3 = Rotate2D( list[i].xy2, xc, yc, angle, scale );
                XY4 = Rotate2D( MakePoint2D( list[i].xy1.dx, list[i].xy2.dy ), xc, yc, angle, scale );
                gpu_quadrilateral( list[i].colour, XY1.dx, XY1.dy, XY2.dx, XY2.dy, XY3.dx, XY3.dy, XY4.dx, XY4.dy );
                break;
            case DLCIRC:
                // NO SECTOR MASK, FULL CIRCLE ONLY
                XY1 = Rotate2D( list[i].xy1, xc, yc, angle, scale );
                gpu_circle( list[i].colour, XY1.dx, XY1.dy, list[i].xy2.dx * scale, 0xff, 1 );
                break;
            case DLARC:
                // NO SECTOR MASK, CIRCLE OUTLINE ONLY
                XY1 = Rotate2D( list[i].xy1, xc, yc, angle, scale );
                gpu_circle( list[i].colour, XY1.dx, XY1.dy, list[i].xy2.dx * scale, 0xff, 0 );
                break;
            case DLTRI:
                XY1 = Rotate2D( list[i].xy1, xc, yc, angle, scale );
                XY2 = Rotate2D( list[i].xy2, xc, yc, angle, scale );
                XY3 = Rotate2D( list[i].xy3, xc, yc, angle, scale );
                gpu_triangle( list[i].colour, XY1.dx, XY1.dy, XY2.dx, XY2.dy, XY3.dx, XY3.dy );
                break;
            case DLQUAD:
                XY1 = Rotate2D( list[i].xy1, xc, yc, angle, scale );
                XY2 = Rotate2D( list[i].xy2, xc, yc, angle, scale );
                XY3 = Rotate2D( list[i].xy3, xc, yc, angle, scale );
                XY4 = Rotate2D( list[i].xy3, xc, yc, angle, scale );
                gpu_quadrilateral( list[i].colour, XY1.dx, XY1.dy, XY2.dx, XY2.dy, XY3.dx, XY3.dy, XY4.dx, XY4.dy );
                break;
        }
    }
}

// PROCESS A DRAWLIST DRAWING SHAPES AFTER SCALING AND MOVING TO CENTRE POINT
void DoDrawList2Dscale( struct DrawList2D *list, short numentries, short xc, short yc, float scale ) {
    struct Point2D XY1, XY2, XY3, XY4;
    for( int i = 0; i < numentries; i++ ) {
        gpu_dither( list[i].dithermode, list[i].alt_colour );
        switch( list[i].shape ) {
            case DLLINE:
                XY1 = Scale2D( list[i].xy1, xc, yc, scale );
                XY2 = Scale2D( list[i].xy2, xc, yc, scale );
                gpu_wideline( list[i].colour, XY1.dx, XY1.dy, XY2.dx, XY2.dy, list[i].xy3.dx * scale );
                break;
            case DLRECT:
                XY1 = Scale2D( list[i].xy1, xc, yc, scale );
                XY2 = Scale2D( list[i].xy2, xc, yc, scale );
                gpu_rectangle( list[i].colour, XY1.dx, XY1.dy, XY2.dx, XY2.dy );
                break;
            case DLCIRC:
                XY1 = Scale2D( list[i].xy1, xc, yc, scale );
                gpu_circle( list[i].colour, XY1.dx, XY1.dy, list[i].xy2.dx * scale, list[i].xy2.dy, 1 );
                break;
            case DLARC:
                XY1 = Scale2D( list[i].xy1, xc, yc, scale );
                gpu_circle( list[i].colour, XY1.dx, XY1.dy, list[i].xy2.dx * scale, list[i].xy2.dy, 0 );
                break;
            case DLTRI:
                XY1 = Scale2D( list[i].xy1, xc, yc, scale );
                XY2 = Scale2D( list[i].xy2, xc, yc, scale );
                XY3 = Scale2D( list[i].xy3, xc, yc, scale );
                gpu_triangle( list[i].colour, XY1.dx, XY1.dy, XY2.dx, XY2.dy, XY3.dx, XY3.dy );
                break;
            case DLQUAD:
                XY1 = Scale2D( list[i].xy1, xc, yc, scale );
                XY2 = Scale2D( list[i].xy2, xc, yc, scale );
                XY3 = Scale2D( list[i].xy3, xc, yc, scale );
                XY4 = Scale2D( list[i].xy4, xc, yc, scale );
                gpu_quadrilateral( list[i].colour, XY1.dx, XY1.dy, XY2.dx, XY2.dy, XY3.dx, XY3.dy, XY4.dx, XY4.dy );
                break;
        }
    }
}

// SPRITE LAYERS - MAIN ACCESS
// TWO SPRITE LAYERS ( 0 == lower, 1 == upper )
// WITH 16 SPRITES ( 0 to 15 ) each with 8 16 x 16 pixel bitmaps

// SET THE BITMAPS FOR sprite_number in sprite_layer to the 8 x 16 x 16 pixel bitmaps ( 2048 ARRGGBB pixels )
void set_sprite_bitmaps( unsigned char sprite_layer, unsigned char sprite_number, unsigned char *sprite_bitmaps ) {
    *( sprite_layer ? UPPER_SPRITE_WRITER_NUMBER : LOWER_SPRITE_WRITER_NUMBER ) = sprite_number;
    DMASTART( sprite_bitmaps, (void *restrict)(sprite_layer ? UPPER_SPRITE_WRITER_COLOUR : LOWER_SPRITE_WRITER_COLOUR), 2048, 1 );
}

// SET SPRITE sprite_number in sprite_layer to active status, in colour to (x,y) with bitmap number tile ( 0 - 7 ) in sprite_attributes bit 0 size == 0 16 x 16 == 1 32 x 32 pixel size, bit 1 x-mirror bit 2 y-mirror
void set_sprite( unsigned char sprite_layer, unsigned char sprite_number, unsigned char active, short x, short y, unsigned char tile, unsigned char sprite_attributes ) {
    switch( sprite_layer ) {
        case 0:
            LOWER_SPRITE_ACTIVE[sprite_number] = active;
            LOWER_SPRITE_TILE[sprite_number] = tile;
            LOWER_SPRITE_X[sprite_number] = x;
            LOWER_SPRITE_Y[sprite_number] = y;
            LOWER_SPRITE_ACTIONS[sprite_number] = sprite_attributes;
            break;

        case 1:
            UPPER_SPRITE_ACTIVE[sprite_number] = active;
            UPPER_SPRITE_TILE[sprite_number] = tile;
            UPPER_SPRITE_X[sprite_number] = x;
            UPPER_SPRITE_Y[sprite_number] = y;
            UPPER_SPRITE_ACTIONS[sprite_number] = sprite_attributes;
            break;
    }
}

// SET or GET ATTRIBUTES for sprite_number in sprite_layer
//  attribute == 0 active status ( 0 == inactive, 1 == active )
//  attribute == 1 tile number ( 0 to 7 )
//  attribute == 3 x coordinate
//  attribute == 4 y coordinate
//  attribute == 5 attributes bit 0 = size == 0 16x16 == 1 32x32. bit 1 = x-mirror bit 2 = y-mirror
void set_sprite_attribute( unsigned char sprite_layer, unsigned char sprite_number, unsigned char attribute, short value ) {
    if( sprite_layer == 0 ) {
        switch( attribute ) {
            case 0:
                LOWER_SPRITE_ACTIVE[sprite_number] = ( unsigned char) value;
                break;
            case 1:
                LOWER_SPRITE_TILE[sprite_number] = ( unsigned char) value;
                break;
            case 2:
                break;
            case 3:
                LOWER_SPRITE_X[sprite_number] = value;
                break;
            case 4:
                LOWER_SPRITE_Y[sprite_number] = value;
                break;
            case 5:
                LOWER_SPRITE_ACTIONS[sprite_number] = ( unsigned char) value;
                break;
        }
    } else {
        switch( attribute ) {
            case 0:
                UPPER_SPRITE_ACTIVE[sprite_number] = ( unsigned char) value;
                break;
            case 1:
                UPPER_SPRITE_TILE[sprite_number] = ( unsigned char) value;
                break;
            case 2:
                break;
            case 3:
                UPPER_SPRITE_X[sprite_number] = value;
                break;
            case 4:
                UPPER_SPRITE_Y[sprite_number] = value;
                break;
            case 5:
                UPPER_SPRITE_ACTIONS[sprite_number] = ( unsigned char) value;
                break;
        }
    }
}

short get_sprite_attribute( unsigned char sprite_layer, unsigned char sprite_number, unsigned char attribute ) {
    if( sprite_layer == 0 ) {
        switch( attribute ) {
            case 0:
                return( (short)LOWER_SPRITE_ACTIVE[sprite_number] );
            case 1:
                return( (short)LOWER_SPRITE_TILE[sprite_number] );
            case 2:
                return( 0 );
            case 3:
                return( LOWER_SPRITE_X[sprite_number] );
            case 4:
                return( LOWER_SPRITE_Y[sprite_number] );
            case 5:
                return( (short)LOWER_SPRITE_ACTIONS[sprite_number] );
            default:
                return( 0 );
        }
    } else {
        switch( attribute ) {
            case 0:
                return( (short)UPPER_SPRITE_ACTIVE[sprite_number] );
            case 1:
                return( (short)UPPER_SPRITE_TILE[sprite_number] );
            case 2:
                return( 0 );
            case 3:
                return( UPPER_SPRITE_X[sprite_number] );
            case 4:
                return( UPPER_SPRITE_Y[sprite_number] );
            case 5:
                return( (short)UPPER_SPRITE_ACTIONS[sprite_number] );
            default:
                return( 0 );
        }
    }
}

// RETURN THE COLLISION STATUS for sprite_number in sprite_layer to other in layer sprites
//  bit is 1 if sprite is in collision with { in layer sprite 15, in layer sprite 14 .. in layer sprite 0 }
unsigned short get_sprite_collision( unsigned char sprite_layer, unsigned char sprite_number ) {
    return( sprite_layer ? UPPER_SPRITE_COLLISION_BASE[sprite_number] : LOWER_SPRITE_COLLISION_BASE[sprite_number] );
}
// RETURN THE COLLISION STATUS for sprite number in sprite layer to other layers
// bit is 1 if sprite is in collision with { bitmap, tilemap L, tilemap U, other sprite layer }
unsigned short get_sprite_layer_collision( unsigned char sprite_layer, unsigned char sprite_number ) {
    return( sprite_layer ? UPPER_SPRITE_LAYER_COLLISION_BASE[sprite_number] : LOWER_SPRITE_LAYER_COLLISION_BASE[sprite_number] );
}

// UPDATE A SPITE moving by x and y deltas, with optional wrap/kill and optional changing of the tile
//  update_flag = { y action, x action, tile action, 5 bit y delta, 5 bit x delta }
//  x and y action ( 0 == wrap, 1 == kill when moves offscreen )
//  x and y deltas a 2s complement -15 to 15 range
//  tile action, increase the tile number ( provides limited animation effects )
void update_sprite( unsigned char sprite_layer, unsigned char sprite_number, unsigned short update_flag ) {
    switch( sprite_layer ) {
        case 0:
            LOWER_SPRITE_UPDATE[sprite_number] = update_flag;
            break;
        case 1:
            UPPER_SPRITE_UPDATE[sprite_number] = update_flag;
            break;
    }
}

// CHARACTER MAP FUNCTIONS
// The character map is an 80 x 30 character window with a 512 character 8 x 8 pixel character generator ROM ) normal/bold
// NO SCROLLING, CURSOR WRAPS TO THE TOP OF THE SCREEN
// CURSES LIBRARY PROVIDES MORE CAPABILITIES, SEE BELOW

// CLEAR THE CHARACTER MAP
void tpu_cs( void ) {
    while( *TPU_COMMIT );
    *TPU_COMMIT = 3;
}

// CLEAR A LINE
void tpu_clearline( unsigned char y ) {
    while( *TPU_COMMIT );
    *TPU_Y = y;
    *TPU_COMMIT = 4;
}

// POSITION THE CURSOR to (x,y) and set background and foreground colours
void tpu_set(  unsigned char x, unsigned char y, unsigned char background, unsigned char foreground ) {
    while( *TPU_COMMIT );
    *TPU_X = x; *TPU_Y = y; *TPU_BACKGROUND = background; *TPU_FOREGROUND = foreground; *TPU_COMMIT = 1;
}

// OUTPUT CHARACTER, STRING, and PRINTF EQUIVALENT FOR THE TPU
void tpu_output_character( short c ) {
    while( *TPU_COMMIT );
    *TPU_CHARACTER = c; *TPU_COMMIT = 2;
}
void tpu_outputstring( char attribute, char *s ) {
    while( *s ) {
        tpu_output_character( ( attribute ? 256 : 0 ) + *s );
        s++;
    }
}
void tpu_print( char attribute, char *buffer ) {
    tpu_outputstring( attribute, buffer );
}
void tpu_printf( char attribute, const char *fmt,... ) {
    char *buffer = (char *)0x1000;
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 1023, fmt, args);
    va_end(args);

    tpu_outputstring( attribute, buffer );
}
void tpu_print_centre( unsigned char y, unsigned char background, unsigned char foreground, char attribute, char *buffer  ) {
    tpu_clearline( y );
    tpu_set( 40 - ( strlen(buffer) >> 1 ), y, background, foreground );
    tpu_outputstring( attribute, buffer );
}
void tpu_printf_centre( unsigned char y, unsigned char background, unsigned char foreground, char attribute, const char *fmt,...  ) {
    char *buffer = (char *)0x1000;
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 80, fmt, args);
    va_end(args);

    tpu_clearline( y );
    tpu_set( 40 - ( strlen(buffer) >> 1 ), y, background, foreground );
    tpu_outputstring( attribute, buffer );
}

// SIMPLE TERMINAL WINDOW FUNCTIONS
// The terminal is an 80 x 8 character window with a 256 character 8 x 8 pixel character generator ROM )
// Outputs characters, understands backspace, newline, linefeed and clear

// CLEAR THE TERMINAL
void terminal_cs( void ) {
    while( *TERMINAL_STATUS );
    *TERMINAL_RESET = 1;
}

// SHOW/HIDE THE TERMINAL WINDOW
void terminal_showhide( unsigned char flag ) {
    *TERMINAL_SHOW = flag;
}

void terminal_output_character( char c ) {
    while( *TERMINAL_STATUS );
    *TERMINAL_COMMIT = c;
    if( c == '\n' ) {
        while( *TERMINAL_STATUS );
        *TERMINAL_COMMIT = 13;
    }
}
void terminal_outputstring( char *s ) {
    while( *s ) {
        terminal_output_character( *s++ );
    }
}
void terminal_print( char *buffer ) {
    terminal_outputstring( buffer );
}
void terminal_printf( const char *fmt,... ) {
    char *buffer = (char *)0x1000;
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 1023, fmt, args);
    va_end(args);

    terminal_outputstring( buffer );
}

// READ A FILE USING THE SIMPLE FILE BROWSER
// WILL ALLOW SELECTION OF A FILE FROM THE SDCARD, INCLUDING SUB-DIRECTORIES
// ALLOCATES MEMORY FOR THE FILE, AND LOADS INTO MEMORY
unsigned char *BOOTRECORD = NULL;
PartitionTable *PARTITIONS = NULL;
Fat32VolumeID *VolumeID = NULL;
FAT32DirectoryEntry *directorycluster = NULL;
unsigned int FAT32startsector, FAT32clustersize, FAT32clusters, *FAT32table = NULL;
DirectoryEntry *directorynames;

// DISPLAY A FILENAME CLEARING THE AREA BEHIND IT
void gpu_outputstring( unsigned char colour, short x, short y, unsigned char bold, char *s, unsigned char size ) {
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256 : 0 ) + *s++, size, 0 );
        x = x + ( 8 << size );
    }
}
void gpu_outputstringcentre( unsigned char colour, short y, unsigned char bold, char *s, unsigned char size ) {
    gpu_rectangle( TRANSPARENT, 0, y, 319, y + ( 8 << size ) - 1 );
    gpu_outputstring( colour, 160 - ( ( ( 8 << size ) * strlen(s) ) >> 1) , y, bold, s, size );
}

void displayfilename( unsigned char *filename, unsigned char type ) {
    char displayname[10], i, j;
    gpu_outputstringcentre( WHITE, 144, 0, "Current File:", 0 );
    for( i = 0; i < 10; i++ ) {
        displayname[i] = 0;
    }
    j = type - 1;
    if( j == 1 ) {
        displayname[0] = 16;
    }
    for( i = 0; i < 8; i++ ) {
        if( filename[i] != ' ' ) {
            displayname[j++] = filename[i];
        }
    }
    gpu_outputstringcentre( type == 1 ? WHITE : GREY5, 176, 0, displayname, 2 );
}

unsigned int __basecluster = 0xffffff8;
unsigned int getnextcluster( unsigned int thiscluster ) {
    unsigned int readsector = thiscluster/128;
    if( ( __basecluster == 0xffffff8 ) || ( thiscluster < __basecluster ) || ( thiscluster > __basecluster + 127 ) ) {
        sdcard_readsector( FAT32startsector + readsector, (unsigned char *)FAT32table );
        __basecluster = readsector * 128;
    }
    return( FAT32table[ thiscluster - __basecluster ] );
}

void readcluster( unsigned int cluster, unsigned char *buffer ) {
     for( unsigned char i = 0; i < FAT32clustersize; i++ ) {
        sdcard_readsector( FAT32clusters + ( cluster - 2 ) * FAT32clustersize + i, buffer + i * 512 );
    }
}

void readfile( unsigned int starting_cluster, unsigned char *copyAddress ) {
    unsigned int nextCluster = starting_cluster;
    unsigned char *CLUSTERBUFFER = (unsigned char *)directorycluster;
    int i;

    do {
        readcluster( nextCluster, CLUSTERBUFFER );
        for( i = 0; i < FAT32clustersize * 512; i++ ) {
            *copyAddress = CLUSTERBUFFER[i];
            copyAddress++;
        }
        nextCluster = getnextcluster( nextCluster);
    } while( nextCluster < 0xffffff8 );
}

void swapentries( short i, short j ) {
    DirectoryEntry temporary;

    paws_memcpy( &temporary, &directorynames[i], sizeof( DirectoryEntry ) );
    paws_memcpy( &directorynames[i], &directorynames[j], sizeof( DirectoryEntry ) );
    paws_memcpy( &directorynames[j], &temporary, sizeof( DirectoryEntry ) );
}

void sortdirectoryentries( unsigned short entries ) {
    // SIMPLE BUBBLE SORT, PUT DIRECTORIES FIRST, THEN FILES, IN ALPHABETICAL ORDER
    if( !entries )
        return;

    short changes;
    do {
        changes = 0;

        for( int i = 0; i < entries; i++ ) {
            if( directorynames[i].type < directorynames[i+1].type ) {
                swapentries(i,i+1);
                changes++;
            }
            if( ( directorynames[i].type == directorynames[i+1].type ) && ( directorynames[i].filename[0] > directorynames[i+1].filename[0] ) ) {
                swapentries(i,i+1);
                changes++;
            }
        }
    } while( changes );
}

unsigned int filebrowser( char *message, char *extension, int startdirectorycluster, int rootdirectorycluster, unsigned int *filesize ) {
    unsigned int thisdirectorycluster = startdirectorycluster;
    FAT32DirectoryEntry *fileentry;

    unsigned char rereaddirectory = 1;
    unsigned short entries, present_entry;
    int temp;

    while( 1 ) {
        if( rereaddirectory ) {
            entries = 0xffff; present_entry = 0;
            fileentry = (FAT32DirectoryEntry *) directorycluster;
            paws_memset( &directorynames[0], 0, sizeof( DirectoryEntry ) * 256 );
        }

        while( rereaddirectory ) {
            readcluster( thisdirectorycluster, (unsigned char *)directorycluster );

            for( int i = 0; i < 16 * FAT32clustersize; i++ ) {
                if( ( fileentry[i].filename[0] != 0x00 ) && ( fileentry[i].filename[0] != 0xe5 ) ) {
                    // LOG ITEM INTO directorynames, if appropriate
                    if( fileentry[i].attributes &  0x10 ) {
                        // DIRECTORY, IGNORING "." and ".."
                        if( fileentry[i].filename[0] != '.' ) {
                            entries++;
                            paws_memcpy( &directorynames[entries], &fileentry[i].filename[0], 11 );
                            directorynames[entries].type = 2;
                            directorynames[entries].starting_cluster = ( fileentry[i].starting_cluster_high << 16 )+ fileentry[i].starting_cluster_low;
                        }
                    } else {
                        if( fileentry[i].attributes & 0x08 ) {
                            // VOLUMEID
                        } else {
                            if( fileentry[i].attributes != 0x0f ) {
                                // SHORT FILE NAME ENTRY
                                if( ( fileentry[i].ext[0] == extension[0] ) && ( fileentry[i].ext[1] == extension[1] ) && ( fileentry[i].ext[2] == extension[2] ) ) {
                                    entries++;
                                    paws_memcpy( &directorynames[entries], &fileentry[i].filename[0], 11 );
                                    directorynames[entries].type = 1;
                                    directorynames[entries].starting_cluster = ( fileentry[i].starting_cluster_high << 16 )+ fileentry[i].starting_cluster_low;
                                    directorynames[entries].file_size = fileentry[i].file_size;
                                }
                            }
                        }
                    }
                }
            }

            // MOVE TO THE NEXT CLUSTER OF THE DIRECTORY
            if( getnextcluster( thisdirectorycluster ) >= 0xffffff8 ) {
                rereaddirectory = 0;
            } else {
                thisdirectorycluster = getnextcluster( thisdirectorycluster );
            }
        }

        if( entries == 0xffff ) {
            // NO ENTRIES FOUND
            return(0);
        } else {
            sortdirectoryentries( entries );
            gpu_outputstringcentre( WHITE, 128, 1, message, 0 );
        }

        while( !rereaddirectory ) {
            displayfilename( directorynames[present_entry].filename, directorynames[present_entry].type );

            unsigned short buttons = get_buttons();
            while( buttons == 1 ) { buttons = get_buttons(); }
            while( get_buttons() != 1 ) {} sleep1khz( 100, 0 );
            if( buttons & 64 ) {
                // MOVE RIGHT
                if( present_entry == entries ) { present_entry = 0; } else { present_entry++; }
            }
            if( buttons & 32 ) {
                // MOVE LEFT
                if( present_entry == 0 ) { present_entry = entries; } else { present_entry--; }
           }
            if( buttons & 8 ) {
                // MOVE UP
                if( startdirectorycluster != rootdirectorycluster ) { return(0); }
           }
            if( buttons & 2 ) {
                // SELECTED
                switch( directorynames[present_entry].type ) {
                    case 1:
                        if( !(*filesize) ) *filesize = directorynames[present_entry].file_size;
                        return( directorynames[present_entry].starting_cluster );
                        break;
                    case 2:
                        temp = filebrowser( message, extension, directorynames[present_entry].starting_cluster, rootdirectorycluster, filesize );
                        if( temp ) {
                            return( temp );
                        } else {
                            rereaddirectory = 1;
                        }
                }
            }
        }
    }
}

unsigned char *sdcard_selectfile( char *message, char *extension, unsigned int *filesize, char *afterloading ) {
    unsigned int starting_cluster;

    *filesize = 0;

    if( VolumeID == NULL ) {
        // MEMORY SPACE NOT ALLOCATED FOR FAT32 STRUCTURES
        BOOTRECORD = malloc( 512 );
        PARTITIONS = (PartitionTable *)&BOOTRECORD[446];
        VolumeID = malloc( 512 );
        FAT32table = malloc( 512 );
        directorynames = (DirectoryEntry *)malloc( sizeof( DirectoryEntry ) * 256 );
        sdcard_readsector( 0, BOOTRECORD );
        sdcard_readsector( PARTITIONS[0].start_sector, (unsigned char *)VolumeID );

        FAT32startsector = PARTITIONS[0].start_sector + VolumeID -> reserved_sectors;
        FAT32clusters = PARTITIONS[0].start_sector + VolumeID -> reserved_sectors + ( VolumeID -> number_of_fats * VolumeID -> fat32_size_sectors );
        FAT32clustersize = VolumeID -> sectors_per_cluster;

        directorycluster = malloc( FAT32clustersize * 512 );
    }

    starting_cluster = filebrowser( message, extension, VolumeID -> startof_root, VolumeID -> startof_root, filesize );
    if( starting_cluster ) {
        // ALLOCATE ENOUGH MEMORY TO READ CLUSTERS
        unsigned char *copyaddress = malloc( ( ( *filesize / ( FAT32clustersize * 512 )  ) + 1 ) * ( FAT32clustersize * 512 ) );
        if( copyaddress ) {
            gpu_outputstringcentre( WHITE, 224, 0, "Loading File", 0 );
            readfile( starting_cluster, copyaddress );
            gpu_outputstringcentre( WHITE, 224, 0, afterloading, 0 );
            return( copyaddress );
        } else {
            gpu_outputstringcentre( WHITE, 224, 1, "Insufficient Memory", 0 );
            return(0);
        }
    } else {
        return(0);
    }
}

// NETPBM DECODER
unsigned int skipcomment( unsigned char *netppmimagefile, unsigned int location ) {
    while( netppmimagefile[ location ] != 0x0a )
        location++;
    location++;
    return( location );
}

void netppm_display( unsigned char *netppmimagefile, unsigned char transparent ) {
    unsigned int location = 3;
    unsigned short width = 0, height = 0, depth = 0;
    unsigned char colour;

    // CHECK HEADER
    if( ( netppmimagefile[0] == 0x50 ) && ( netppmimagefile[1] == 0x36 ) && ( netppmimagefile[2] == 0x0a ) ) {
        // VALID HEADER

        // SKIP COMMENT
        while( netppmimagefile[ location ] == 0x23 ) {
            location = skipcomment( netppmimagefile, location );
        }

        // READ WIDTH
        while( netppmimagefile[ location ] != 0x20 ) {
            width = width * 10 + netppmimagefile[ location ] - 0x30;
            location++;
        }
        location++;

        // READ HEIGHT
        while( netppmimagefile[ location ] != 0x0a ) {
            height = height * 10 + netppmimagefile[ location ] - 0x30;
            location++;
        }
        location++;

        // READ DEPTH
        while( netppmimagefile[ location ] != 0x0a ) {
            depth = depth * 10 + netppmimagefile[ location ] - 0x30;
            location++;
        }
        location++;

        // 24 bit image
        if( depth == 255 ) {
            for( unsigned short y = 0; y < height; y++ ) {
                for( unsigned short x = 0; x < width; x++ ) {
                    colour = ( netppmimagefile[ location++ ] & 0xc0 ) >> 2;
                    colour = colour + ( ( netppmimagefile[ location++ ] & 0xc0 ) >> 4 );
                    colour = colour + ( ( netppmimagefile[ location++ ] & 0xc0 ) >> 6 );
                    if( colour != transparent )
                        gpu_pixel( colour, x, y );
                }
            }
        }
    }
}

// DECODE NETPPM FILE TO ARRAY
void netppm_decoder( unsigned char *netppmimagefile, unsigned char *buffer ) {
    unsigned int location = 3, bufferpos = 0;
    unsigned short width = 0, height = 0, depth = 0;
    unsigned char colour;

    // CHECK HEADER
    if( ( netppmimagefile[0] == 0x50 ) && ( netppmimagefile[1] == 0x36 ) && ( netppmimagefile[2] == 0x0a ) ) {
        // VALID HEADER

        // SKIP COMMENT
        while( netppmimagefile[ location ] == 0x23 ) {
            location = skipcomment( netppmimagefile, location );
        }

        // READ WIDTH
        while( netppmimagefile[ location ] != 0x20 ) {
            width = width * 10 + netppmimagefile[ location ] - 0x30;
            location++;
        }
        location++;

        // READ HEIGHT
        while( netppmimagefile[ location ] != 0x0a ) {
            height = height * 10 + netppmimagefile[ location ] - 0x30;
            location++;
        }
        location++;

        // READ DEPTH
        while( netppmimagefile[ location ] != 0x0a ) {
            depth = depth * 10 + netppmimagefile[ location ] - 0x30;
            location++;
        }
        location++;

        // 24 bit image
        if( depth == 255 ) {
            for( unsigned short y = 0; y < height; y++ ) {
                for( unsigned short x = 0; x < width; x++ ) {
                    colour = ( netppmimagefile[ location++ ] & 0xc0 ) >> 2;
                    colour = colour + ( ( netppmimagefile[ location++ ] & 0xc0 ) >> 4 );
                    colour = colour + ( ( netppmimagefile[ location++ ] & 0xc0 ) >> 6 );
                    buffer[ bufferpos++ ] = colour;
                }
            }
        }
    }
}

// SIMPLE CURSES LIBRARY
// USES THE CURSES BUFFER IN THE CHARACTER MAP
char __stdinout_init = FALSE, __sdcard_init = FALSE;

unsigned char   __curses_backgroundcolours[COLOR_PAIRS], __curses_foregroundcolours[COLOR_PAIRS],
                __curses_scroll = 1, __curses_echo = 0, __curses_bold = 0, __curses_reverse = 0, __curses_autorefresh = 0;
unsigned short  __curses_x = 0, __curses_y = 0, __curses_fore = WHITE, __curses_back = BLACK;

void *stdscr;

typedef union curses_cell {
    unsigned int bitfield;
    struct {
        unsigned int pad : 9;
        unsigned int character : 9;
        unsigned int background : 7;
        unsigned int foreground : 7;
    } cell;
} __curses_cell;

void __position_curses( unsigned short x, unsigned short y ) {
    while( *TPU_COMMIT );
    *TPU_X = x; *TPU_Y = y; *TPU_COMMIT = 1;
}

void __update_tpu( void ) {
    while( *TPU_COMMIT );
    *TPU_X = __curses_x; *TPU_Y = __curses_y; *TPU_COMMIT = 1;
    *TPU_BACKGROUND = __curses_back; *TPU_FOREGROUND = __curses_fore;
}

__curses_cell __read_curses_cell( unsigned short x, unsigned short y ) {
    __curses_cell storage;
    __position_curses( x, y );
    storage.cell.character = *TPU_CHARACTER;
    storage.cell.background = *TPU_BACKGROUND;
    storage.cell.foreground = *TPU_FOREGROUND;
    return( storage );
}

void __write_curses_cell( unsigned short x, unsigned short y, __curses_cell writecell ) {
    while( *TPU_COMMIT );
    __position_curses( x, y );
    *TPU_CHARACTER = writecell.cell.character;
    *TPU_BACKGROUND = writecell.cell.background;
    *TPU_FOREGROUND = writecell.cell.foreground;
    *TPU_COMMIT = 5;
}

void initscr( void ) {
    *CURSES_BACKGROUND = BLACK; *CURSES_FOREGROUND = WHITE;
    while( *TPU_COMMIT );
    *TPU_COMMIT = 6;
    __curses_x = 0; __curses_y = 0; __curses_fore = WHITE; __curses_back = BLACK; *TPU_CURSOR = 1; __curses_scroll = 1; __curses_bold = 0; __update_tpu();
    __stdinout_init = TRUE;
}

int endwin( void ) {
    return( true );
}

int refresh( void ) {
    while( *TPU_COMMIT );
    *TPU_COMMIT = 7;
    return( true );
}

int clear( void ) {
    while( *TPU_COMMIT );
    *TPU_COMMIT = 6;
    __curses_x = 0; __curses_y = 0; __curses_fore = WHITE; __curses_back = BLACK; __curses_bold = 0; __update_tpu();
    return( true );
}

void cbreak( void ) {
}

void echo( void ) {
}

void noecho( void ) {
}

void scroll( void ) {
    __curses_scroll = 1;
}

void noscroll( void ) {
    __curses_scroll = 0;
}

void curs_set( int visibility ) {
    *TPU_CURSOR = visibility;
}

void autorefresh( int flag ) {
    __curses_autorefresh = flag;
}

int start_color( void ) {
    for( unsigned short i = 0; i < 15; i++ ) {
        __curses_foregroundcolours[i] = BLACK;
        __curses_backgroundcolours[i] = BLACK;
    }
    __curses_foregroundcolours[0] = BLACK;
    __curses_foregroundcolours[1] = RED;
    __curses_foregroundcolours[2] = GREEN;
    __curses_foregroundcolours[3] = YELLOW;
    __curses_foregroundcolours[4] = BLUE;
    __curses_foregroundcolours[5] = MAGENTA;
    __curses_foregroundcolours[6] = CYAN;
    __curses_foregroundcolours[7] = WHITE;

    return( true );
}

bool has_colors( void ) {
    return( true );
}

bool can_change_color( void ) {
    return( true );
}

int init_color(short color, short r, short g, short b) {
    return( true );
}

int init_pair( short pair, short f, short b ) {
    __curses_foregroundcolours[ pair ] = f;
    __curses_backgroundcolours[ pair ] = b;
    return( true );
}


int move( int y, int x ) {
    __curses_x = ( unsigned short ) ( x < 0 ) ? 0 : ( x > COLS-1 ) ? COLS-1 : x;
    __curses_y = ( unsigned short ) ( y < 0 ) ? 0 : ( y > LINES-1 ) ? LINES-1 : y;
    __position_curses( __curses_x, __curses_y );
    return( true );
}

int getyx( int *y, int *x ) {
    *y = (int)__curses_y;
    *x = (int)__curses_x;
    return( true );
}

void __scroll( void ) {
    while( *TPU_COMMIT );
    *TPU_COMMIT = 8;
}

int addch( unsigned char ch ) {
    __curses_cell temp;
    short gonextline = 0;

    switch( ch ) {
        case '\b': {
            // BACKSPACE
            if( __curses_x ) {
                __curses_x--;
            } else {
                if( __curses_y ) {
                    __curses_y--;
                    __curses_x = COLS-1;
                }
            }
            break;
        }
        case '\n': {
            // LINE FEED
            __curses_x = 0;
            gonextline = 1;
            break;
        }
        case '\r': {
            // CARRIAGE RETURN
            __curses_x = 0;
            break;
        }
        case '\t': {
            // TAB
            __curses_x = ( 1 + __curses_x / 8 ) * 8;
            if( __curses_x >= COLS ) {
                __curses_x = 0;
                gonextline = 1;
            }
            break;
        }

        default: {
            if( __curses_autorefresh ) {
                tpu_set( __curses_x, __curses_y, __curses_back, __curses_fore );
                tpu_output_character( ( __curses_bold ? 256 : 0 ) + ch );
            }
            temp.cell.character = ( __curses_bold ? 256 : 0 ) + ch;
            temp.cell.background = __curses_back;
            temp.cell.foreground = __curses_fore;
            __write_curses_cell( __curses_x, __curses_y, temp );
            if( __curses_x == COLS-1 ) {
                __curses_x = 0;
                gonextline = 1;
            } else {
                __curses_x++;
            }
        }
    }

    // GO TO NEXT LINE, SCROLL/WRAP IF REQUIRED
    if( gonextline ) {
        if( __curses_y == LINES-1 ) {
            if( __curses_scroll ) {
                __scroll();
                if( __curses_autorefresh )
                    refresh();
            } else {
                __curses_y = 0;
            }
        } else {
            __curses_y++;
        }
    }

    __position_curses( __curses_x, __curses_y );
    return( true );
}

int mvaddch( int y, int x, unsigned char ch ) {
    (void)move( y, x );
    return( addch( ch ) );
}

void __curses_print_string(const char* s) {
   for(const char* p = s; *p; ++p) {
      addch(*p);
   }
}

int printw( const char *fmt,... ) {
    char *buffer = (char *)0x1000;
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 1023, fmt, args);
    va_end(args);

    __curses_print_string( buffer );
    return( true );
}

int mvprintw( int y, int x, const char *fmt,... ) {
    char *buffer = (char *)0x1000;
    va_list args;
    va_start (args, fmt);
    vsnprintf( buffer, 1023, fmt, args);
    va_end(args);

    move( y, x );
    __curses_print_string( buffer );

    return( true );
}

int attron( int attrs ) {
    if( attrs & COLORS ) {
        __curses_fore = __curses_foregroundcolours[ attrs & 0x7f ];
        __curses_back = __curses_backgroundcolours[ attrs & 0x7f ];
        __update_tpu();
    }
    if( attrs & A_NORMAL ) {
        __curses_bold = 0;
        __curses_reverse = 0;
    }

    if( attrs & A_BOLD ) {
        __curses_bold = 1;
    }

    if( attrs & A_REVERSE )
        __curses_reverse = 1;

    return( true );
}

int attroff( int attrs ) {
   if( attrs & A_BOLD ) {
        __curses_bold = 0;
    }

    if( attrs & A_REVERSE )
        __curses_reverse = 0;

    return( true );
}

void bkgdset( int attrs ) {
    __curses_fore = __curses_foregroundcolours[ attrs & 0x7f ]; *CURSES_FOREGROUND = __curses_foregroundcolours[ attrs & 0x7f ];
    __curses_back = __curses_backgroundcolours[ attrs & 0x7f ]; *CURSES_BACKGROUND = __curses_backgroundcolours[ attrs & 0x7f ];
}

int deleteln( void ) {
     while( *TPU_COMMIT );
    *TPU_COMMIT = 9;

    return( true );
}

int clrtoeol( void ) {
     while( *TPU_COMMIT );
    *TPU_COMMIT = 10;

    return( true );
}

int clrtobot( void ) {
     while( *TPU_COMMIT );
    *TPU_COMMIT = 11;

    return( true );
}

int intrflush( WINDOW *win, bool bf ) {
    return( 0 );
}

int keypad( WINDOW *win, bool bf ) {
    return( 0 );
}

// FAT16/32 File IO Library from Ultra-Embedded.com
#ifdef feof
#undef feof
#endif
#define FAT_PRINTF_NOINC_STDIO
#define USE_FILELIB_STDIO_COMPAT_NAMES
#include "fat_io_lib/fat_filelib.h"

// READ MULTIPLE SECTORS INTO MEMORY FOR fat_io_lib
int sd_media_read( uint32 sector, uint8 *buffer, uint32 sector_count ) {
    while( sector_count-- ) {
        sdcard_readsector( sector, buffer );
        // MOVE TO NEXT SECTOR
        sector++; buffer += FAT_SECTOR_SIZE;
    }

    return(1);
}

int sd_media_write( uint32 sector, uint8 *buffer, uint32 sector_count ) {
    while( sector_count-- ) {
        sdcard_writesector( sector, buffer );
        // MOVE TO NEXT SECTOR
        sector++; buffer += FAT_SECTOR_SIZE;
    }
    return(1);
}

// newlib support routines - define standard malloc memory size 16MB
#ifndef MALLOC_MEMORY
#define MALLOC_MEMORY ( 16384 * 1024 )
#endif

// Standard i/o directs to the curses terminal, input is from the ps_keyboard
extern unsigned char ps2_character_available( void );
extern unsigned char ps2_inputcharacter( void );
extern void ps2_keyboardmode( unsigned char mode );

unsigned char *_heap = NULL;
unsigned char *_sbrk( int incr ) {
unsigned char *prev_heap;

  if (_heap == NULL) {
    _heap = (unsigned char *)MEMORYTOP - MALLOC_MEMORY - 32;
  }
  prev_heap = _heap;

  if( incr < 0 ) {
      _heap = _heap;
  } else {
    _heap += incr;
  }

  return prev_heap;
}

// PAWS INITIALISATION FOR THE TERMNAL AND THE SDCARD for fat_io_lib
void __start_stdinout( void ) {
    initscr(); start_color(); autorefresh( TRUE ); ps2_keyboardmode( PS2_KEYBOARD );
    __stdinout_init = TRUE;
}
void __start_sdmedia( void ) {
    // Initialise File IO Library
    fl_init();
    fl_attach_media(sd_media_read, sd_media_write);
    __sdcard_init = TRUE;
}

// PAWS redirection of printf/fprintf to console/uart/fat_io_lib as needed
int paws_printf(const char *restrict format, ... ) {
    if( !__stdinout_init ) __start_stdinout();

    char *buffer = (char *)0x1400;
    va_list args;
    va_start (args, format);
    vsnprintf( buffer, 1024, format, args);
    va_end(args);

    printw( "%s", buffer );
    return( strlen( buffer ) );
}

int paws_fprintf( void *fd, const char *restrict format, ... ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();

    char *buffer = (char *)0x1400;
    va_list args;
    va_start (args, format);
    vsnprintf( buffer, 1024, format, args);
    va_end(args);

    if( ( fd == stdout ) || ( fd == stderr ) ) {
        if( fd == stdout ) printw( "%s", buffer );
        if( fd == stderr ) uart_outputstring( buffer );
    } else {
        fl_fwrite( buffer, strlen( buffer ), 1, fd );
    }

    return( strlen( buffer ) );
}

int paws_vfprintf( void *fd, const char *format, va_list args ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();

    char *buffer = (char *)0x1400;
    vsnprintf( buffer, 1024, format, args);
    if( ( fd == stdout ) || ( fd == stderr ) ) {
        if( fd == stdout ) printw( "%s", buffer );
        if( fd == stderr ) uart_outputstring( buffer );
    } else {
        fl_fwrite( buffer, strlen( buffer ), 1, fd );
    }

    return( strlen( buffer ) );
}


// LINK NEWLIB STUB FUNCTIONS TO FAT_IO_LIB FUNCTIONS

#define MAXOPENFILES 4
struct sFL_FILE *__filehandles[ MAXOPENFILES + 3 ]; // stdin, stdout, stderr

// FIND AN UNUSED FILE HANDLE FROM 3, 0 = stdin, 1 = stdout, 2 = stderr
int __find_filehandlespace( void ) {
    for( int i = 3; i < MAXOPENFILES+3; i++ ) {
        if( __filehandles[ i ] == NULL ) {
            return i;
        }
    }
    return -1;
}

int _open( const char *file, int flags ) {
    if( !__sdcard_init ) __start_sdmedia();

    static char *__openmodes[] = { "r", "r+", "w", "w+", "a", "a+" };
    int handle = __find_filehandlespace(), mode;
    if( handle == -1 ) {
        return -1;
    } else {
        // DETERMINE OPENING MODE
        switch( flags ) {
            case O_RDONLY:                      mode = 0; break;
            case O_WRONLY | O_CREAT | O_TRUNC:  mode = 2; break;
            case O_WRONLY | O_CREAT | O_APPEND: mode = 4; break;
            case O_RDWR:                        mode = 1; break;
            case O_RDWR | O_CREAT | O_TRUNC:    mode = 3; break;
            case O_RDWR | O_CREAT | O_APPEND:   mode = 5; break;
            default: mode = -1;
        }
        fprintf(stderr,"Opening file %s into handle %0d using mode %s : ",file,handle,(mode == -1 ) ? "err" : __openmodes[mode]);
        if( mode != -1 ) {
            __filehandles[ handle ] = fl_fopen( file, __openmodes[ mode ] );
            if( __filehandles[ handle ] != NULL ) {
                fprintf(stderr,"Success\n");
                return handle;
            }
        }
        fprintf(stderr,"Failed\n");
        return( -1 );
    }
}

int _close( int handle ) {
    if( !__sdcard_init ) __start_sdmedia();

    if( __filehandles[ handle ] != NULL ) {
        fl_fclose( __filehandles[ handle ] );
        __filehandles[ handle ] = NULL;
        return 0;
    } else {
        return -1;
    }
}

int _stat( char *file, struct stat *st ) {
    if( !__sdcard_init ) __start_sdmedia();

    int handle = _open( file, 0 );
    if( handle != -1 ) {
        st->st_size = __filehandles[ handle ]->filelength;
        _close( handle );
        return( 0 );
    } else {
        return -1;
    }
}

long _write( int fd, const void *buf, size_t cnt ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();

    unsigned char *buffer = (unsigned char *)buf;

    while( cnt-- ) {
        switch( fd ) {
            case STDOUT_FILENO:
                addch( *buffer++ );
                break;
            case STDERR_FILENO:
                uart_outputcharacter( *buffer++ );
                break;
            default:
                break;
        }
    }
    return( cnt );
}
long _read( int fd, void *buf, size_t cnt ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();

    unsigned char *buffer = (unsigned char *)buf, input;

    switch( fd ) {
        case STDIN_FILENO:
            while( cnt-- ) {
                input = ps2_inputcharacter();
                if( input != 0x0d ) {
                    *buffer++ = input;
                } else {
                    *buffer = 0;
                    return( strlen( buf ) );
                }
            }
            break;
        default:
            return( fl_fread( buf, cnt, 1, __filehandles[ fd ] ) );
            break;
    }
    buffer[ cnt - 1 ] = 0; return( strlen( buf ) );
}
int _lseek( int fd, int pos, int whence ) {
    if( !__sdcard_init ) __start_sdmedia();

    switch( fd ) {
        case STDIN_FILENO:
        case STDOUT_FILENO:
        case STDERR_FILENO:
            return( -1 );
            break;
        default:
            if( !fl_fseek( __filehandles[ fd ], pos, whence ) ) {
                return( fl_ftell( __filehandles[ fd ] ) );
            } else {
                return( -1 );
            break;
            }
    }
}
int _isatty( int fd ) {
    return( 0 );
}

int _getpid() {
    return( 0 );
}
int _kill() {
    return( -1 );
}
void  __attribute__ ((noreturn)) _exit( int status ){
    ((void(*)(void))0x00000000)();
    while(1);
}
int _gettimeofday( struct timeval *restrict tv, struct timezone *restrict tz ) {
    tv->tv_sec = (time_t)*SYSTEMSECONDS;
    tv->tv_usec = (suseconds_t)*SYSTEMMILLISECONDS;
    return( 0 );
}
int _times() {
    return( 0 );
}
int _link ( const char *oldname, const char *newname ) {
    return -1;
}
int _unlink ( const char *name ) {
    return -1;
}
int _fstat( int fd, struct stat *st ) {
    if( !__sdcard_init ) __start_sdmedia();

    if( __filehandles[ fd ] != NULL ) {
        st->st_size = __filehandles[ fd ]->filelength;
        return( 0 );
    }
    return -1;
}

// LINK TO fat_io_lib, check sdcard is initialised
// DIRECT to stdin, stdout, stderr or fat_io_lib as appropriate
void *paws_fopen( const char *path, const char *modifiers ) {
    if( !__sdcard_init ) __start_sdmedia();
    return( fl_fopen( path, modifiers ) );
}
int paws_fclose( void *fd ) {
    if( !__sdcard_init ) __start_sdmedia();
    fl_fclose( fd );
    return( 0 );
}
void *paws_freopen( const char *path, const char *mode, FILE *fd ) {
    char filename[FATFS_MAX_LONG_FILENAME];

    if( fd ) {
        // FILE IS PRESENTLY OPEN
        paws_memcpy( filename, fd -> filename, FATFS_MAX_LONG_FILENAME );
        paws_fclose( fd );
    } else {
        // FILE IS NOT OPEN
        paws_memcpy( filename, path, strlen(path) );
    }
    return( paws_fopen( filename, mode ) );
}
void *paws_tmpfile( void ) {
    sprintf( (char * restrict)0x1800, "%0d%0d.tmp", rng(65535), *SYSTEMSECONDS );
    return( paws_fopen( (const char *)0x1800, "w+b" ) );
}
int paws_fgetc( void *fd ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();
    char input;
    if( fd == stdin ) {
        input = ps2_inputcharacter();
        addch( input );
        return ( input );
    } else {
        return( fl_fgetc( fd ) );
    }
}
char *paws_fgets( char *s, int cnt, void *fd ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();

    unsigned char *buffer = (unsigned char *)s, input;
    if( fd == stdin ) {
        while( cnt-- ) {
            input = ps2_inputcharacter();
            if( input != 0x0d ) {
                *buffer++ = input;
                addch( input );
            } else {
                addch( '\n' );
                *buffer = 0;
                return( s);
            }
        }
    } else {
        return( fl_fgets( s, cnt, fd ) );
    }
    *buffer = 0; return( s );
}
int paws_fputc( int c, void *fd ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();
    if( ( fd == stdout ) || ( fd == stderr ) )  {
        if( fd == stdout ) addch( c );
        if( fd == stderr ) uart_outputcharacter( c );
        return( c );
    } else {
        return( fl_fputc( c, fd ) );
    }
}
int paws_fputs( const char *s, void *fd ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();
    if( ( fd == stdout ) || ( fd == stderr ) )  {
        if( fd == stdout ) printw( "%s", s );
        if( fd == stderr ) uart_outputstring( s );
        return( strlen( s ) );
    } else {
        return( fl_fputs( s, fd ) );
    }
}
int paws_fwrite(const void *data, int size, int count, void *fd ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();

    unsigned char *buffer;
    if( ( fd == stdout ) || ( fd == stderr ) )  {
        for( int i = 0; i < count; i++ ) {
            buffer = (unsigned char *)data;
            for( int j = 0; j < size; j++ ) {
                if( fd == stdout ) addch( *buffer++ );
                if( fd == stderr ) uart_outputcharacter(  *buffer++  );
            }
        }
        return( count );
    } else {
        return( fl_fwrite( data, size, count, fd ) );
    }
}
int paws_fread( void *data, int size, int count, void *fd ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();

    unsigned char *buffer = (unsigned char *)data;
    if( fd == stdin ) {
        for( int i = 0; i < count; i++ ) {
            for( int j = 0; j < size; j++ ) {
                *buffer++ = ps2_inputcharacter();
            }
        }
        return( count );
    } else {
        return( fl_fread( data, size, count, fd ) );
    }
}

int paws_fflush( void *fd ) {
    if( !__stdinout_init ) __start_stdinout();
    if( !__sdcard_init ) __start_sdmedia();

    if( ( fd == stdin ) || ( fd == stdout ) || ( fd == stderr ) ) {
        return(0);
    } else {
        return( fl_fflush( fd ) );
    }
}

void paws_clearerr( void *fd ) {
}

int paws_ungetc( int c, void *fd ) {
    if( !__stdinout_init ) __start_stdinout();
    if( fd == stdin ) {
        // MECHANISM TO RETURN A CHARACTER TO STDIN
        return( c );
    } else {
        return( fseek( fd, -1L, SEEK_CUR ) );
    }
}

int paws_rmdir (const char *__path) {
    return(0);
}

// PAWS SYSTEMCLOCK
int paws_clock_gettime( clockid_t clk_id, void *tz ) {
    struct timespec *tv = tz;
    tv->tv_sec = (time_t)*SYSTEMSECONDS;
    tv->tv_nsec = (long)(*SYSTEMMILLISECONDS/1000);
    return( 0 );
}

// PAWS SLEEP FOR sys/unistd.h
unsigned int paws_sleep( unsigned int seconds ) {
    // WAIT FOR A FREE TIMER
    while( *SLEEPTIMER0 && *SLEEPTIMER1 );
    sleep1khz( 1000 * seconds, ( !*SLEEPTIMER0 ) ? 0 : 1 );
    return(0);
}
