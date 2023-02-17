#include <PAWSlibrary.h>
#include <PAWSintrinsics.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

// LOAD THE SPRITES
unsigned char sprites[] = {
#include "CHIP8.h"
};

// MACRO TO SAFELY ADD TO AN ADDRESS WITH 12-bit MASKING
#define ADDRESS(x,y) ( ( x + y ) & ( ( machine.MODE == XOCHIP ) ? 0xffff : 0xfff ) )

// MACRO TO FETCH TWO BYTES AND COMBINE TO ONE HALF-WORD
#define FETCH(x) ( machine.MEMORY[ ADDRESS(x,0) ] << 8 ) | machine.MEMORY[ ADDRESS(x,1) ]

enum MODES { CHIP8, CHIP48, SCHIP, XOCHIP };                                                                                                   // MODELS SUPPORTED
unsigned char *modes[] = {
    "CHIP-8 ",
    "CHIP-48",
    "S-CHIP ",
    "XO-CHIP"
};

enum CRASHES { NONE, INVALIDINSN, WRONGMODE, INVALIDFILE, STACKOFLOW, STACKUFLOW };                                             // CRASH REASONS
unsigned char *crashes[] = {
    "                   ",
    "INVALID INSTRUCTION",
    "WRONG MODE         ",
    "INVALID FILE       ",
    "STACK OVERFLOW     ",
    "STACK UNDERFLOW    "
};

enum STATES { PAUSED, RUNNING };
unsigned char *states[] = {
    "PAUSED ",
    "RUNNING"
};

enum SPEEDS { MAX, VIP, CYCLE };
unsigned char *speed[] = {
    "MAX  I/S",
    "1000 I/S",
    "1 I/S   "
};

unsigned char *keyboard = "1234QWERASDFZXCV";
unsigned char *keys = "123C456D789EA0BF";

unsigned char *fkeys[] = {
    "F1    ", "F2    ", "F3    ", "F4    ", "F5    ", "F6    ", "F7    ", "F8    ", "F9    ", "F10   ", "F11   ", "F12   "
};

unsigned char *factions[] = {
    "CHIP-8", "CHIP48", "S-CHIP", "XOCHIP", "      ", "      ", "      ", "LIMIT ", "QUIT  ", "      ", "RESET ", "LOAD  "
};

// NUMBER OF BYTES IN THE DISPLAY BUFFER UNSIGNED LONG * 2 PER ROW * 64 ROWS * 2 BUFFERS
#define PLANESIZE 8 * 2 * 64
#define DISPLAYSIZE 2 * PLANESIZE

struct C8 {                                                                                                                     // STRUCTURE FOR THE CHIP 8 CPU AND MEMORY
    enum MODES MODE;                                                                                                            // OPERATING MODE ( CHIP8 SCHIP XOCHIP )
    uint16_t PC;                                                                                                                // PC
    uint16_t I;                                                                                                                 // INDEX REGISTER
    uint8_t V[16], FLAGS[16];                                                                                                   // REGISTERS AND STORAGE SPACE FOR COPIES
    uint8_t MEMORY[ 4096 ];                                                                                                     // MACHINE RAM
    uint16_t KEYS;                                                                                                              // KEY PRESSEED BITMAP
    uint64_t DISPLAY[ 2 ][ 64 ][ 2 ];                                                                                           // DISPLAY 128(bits) x 64(lines) x 2(planes)
    uint8_t HIRES;                                                                                                              // HI RESOLUTION SWITCH
    uint8_t PLANES;                                                                                                             // BITMASK OF PLANES TO DRAW ON
    uint8_t timer;                                                                                                              // 60Hz timer
    uint8_t audio_timer;                                                                                                        // 60Hz audio timer

    int STACKTOP;                                                                                                               // POINTER TO TOP OF STACK - -1 == NOTHING
    uint16_t STACK[ 16 ];                                                                                                       // 16 STACK ENTRIES

    int quit;                                                                                                                   // QUIT
    int restart;                                                                                                                // RESTART FROM 0x200 REQUESTED
    int running;                                                                                                                // MACHINE IS RUNNING
    int loading;                                                                                                                // LOAD REQUESTED
    int limit;                                                                                                                  // LIMIT TO 1000 ISNS PER SECOND
    int debug;                                                                                                                  // OUTPUT DEBUG INFORMATION
    enum CRASHES crashed;                                                                                                       // CRASHED, ENCODES REASON
    uint16_t lastPC;                                                                                                            // PC OF LAST INSTRUCTION
    uint16_t lastinstruction;                                                                                                   // LAST INSTRUCTION
};
struct C8 machine;                                                                                                              // INSTANCE OF THE CHIP8 MACHINE

#define FONTSIZE 5 * 16 + 10 * 16
unsigned char chip8font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F

    // Big Hex (0-9):
    0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, // 0
    0x0C, 0x0C, 0x3C, 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x3F, // 1
    0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, // 2
    0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, // 3
    0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03, // 4
    0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, // 5
    0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, // 6
    0xFF, 0xFF, 0x03, 0x03, 0x0C, 0x0C, 0x30, 0x30, 0x30, 0x30, // 7
    0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, // 8
    0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, // 9

    /* Big Hex (A-F, which is not defined by original S-CHIP,
    but some programs assume they exist anyway) */
    0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, // A
    0xFC, 0xFC, 0xC3, 0xC3, 0xFC, 0xFC, 0xC3, 0xC3, 0xFC, 0xFC, // B
    0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, // C
    0xFC, 0xFC, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFC, 0xFC, // D
    0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, // E
    0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0  // F
};

// SIMPLE ACCESS TO THE FLAG REGISTER
#define VF machine.V[15]

// COLOUR MAPS 4 POSSIBLE COLOURS
#define C32(x) ( ( x << 24 ) | ( x << 16 ) | ( x << 8 ) | x )
#define C16(x ) ( ( x << 8 ) | x )

uint16_t colourmap16[] = {
    C16( TRANSPARENT), C16( BLACK ), C16( RED ), C16( GREEN )
};

uint32_t colourmap32[] = {
    C32( TRANSPARENT), C32( BLACK ), C32( RED ), C32( GREEN )
};

// SMT THREAD FUNCTIONS
// MONITOR KEYBOARD
// DRAW SCREEN DURING REFRESH ( avoids flicker )
// DISPLAY MACHINE STATUS INFORMATION
// DRAW SCREEN FUNCTIONS
//      LORES will draw 64 x 32 pixels as a 4x4 PAWSv2 pixel giving 256 x 128 display
//      HIRES will draw 128 x 64 pixels as a 2x2 PAWSv2 pixel giving 256 x 128 display
int get_pixel( int x, int y ) {
    return(
        _rv64_bext( machine.DISPLAY[ 0 ][ y ][ _rv64_bext( x , 6 ) ], 64 - ( x & 63 ) ) |
        ( _rv64_bext( machine.DISPLAY[ 1 ][ y ][ _rv64_bext( x , 6 ) ], 64 - ( x & 63 ) ) << 1 )
    );
}

void draw_screen_lores( void ) {
    uint32_t *TL = (uint32_t *)( 0x2000000 + 112 * 320 + 32 );                                                                  // TOP LEFT CORNER
    uint32_t *L, *P;                                                                                                            // PRESENT LINE AND PRESENT PIXEL

    L = TL;
    for( int y = 0; y < 64; y+=2 ) {
        P = L;
        for( int x = 0; x < 128; x+=2 ) {
            P[ 0 ] = P[ 80 ] = P[ 160 ] = P[ 240 ] = colourmap32[ get_pixel( x, y ) ];
            P++;
        }
        L = &L[ 320 ];
    }
}

void draw_screen_hires( void ) {
    uint16_t *TL = (uint16_t *)( 0x2000000 + 112 * 320 + 32 );                                                                  // TOP LEFT CORNER
    uint16_t *L, *P;                                                                                                            // PRESENT LINE AND PRESENT PIXEL

    L = TL;
    for( int y = 0; y < 64; y++ ) {
        P = L;
        for( int x = 0; x < 128; x++ ) {
            P[ 0 ] = P[ 160 ] = colourmap16[ get_pixel( x, y ) ];
            P++;
        }
        L = &L[ 320 ];
    }
}

void display_state( void ) {
    // DISPLAY STATE
    set_sprite32( UPPER_LAYER, 0, SPRITE_SHOW, 544, 64, machine.MODE, SPRITE_DOUBLE );
    set_sprite32( UPPER_LAYER, 4, machine.crashed ? ( systemclock() & 1 ) : SPRITE_SHOW, 608, 64, machine.crashed ? 0 : machine.running, SPRITE_DOUBLE );
    set_sprite32( UPPER_LAYER, 8, SPRITE_SHOW, 544, 128, machine.limit, SPRITE_DOUBLE );
    set_sprite32( UPPER_LAYER, 12, ( machine.crashed == 0 ) ? SPRITE_SHOW : ( systemclock() & 1 ), 608, 128, machine.crashed, SPRITE_DOUBLE );

    tpu_set( 1, 5, TRANSPARENT, BLACK ); tpu_printf( 0, "PC[%03x] I[%03x]", machine.PC, machine.I );
    tpu_set( 17, 5, TRANSPARENT, machine.timer ? GREEN : GREY3 ); tpu_printf( 0, "T[%02x]", machine.timer );
    tpu_set( 25, 5, TRANSPARENT, machine.audio_timer ? GREEN : GREY3 ); tpu_printf( 0, "A[%02x]", machine.audio_timer );
    tpu_set( 33, 5, TRANSPARENT, machine.HIRES ? GREEN : GREY3 ); tpu_printf( 0, "H[%01x]", machine.HIRES );
    tpu_set( 41, 5, TRANSPARENT, BLACK ); tpu_printf( 0, "P[%01x]", machine.PLANES );
    tpu_set( 49, 5, TRANSPARENT, GREY4 ); tpu_printf( 0, "X[%04x] @[%03x]", machine.lastinstruction, machine.lastPC );

    for( int n = 0; n < 16; n++ ) {
        if( n == 0 ) tpu_set( 1, 7, TRANSPARENT, BLACK );
        if( n == 8 ) tpu_set( 1, 8, TRANSPARENT, BLACK );
        tpu_printf( 0, "V%1x[%02x]  ", n, machine.V[n] );
    }

    for( int n = 0; n < 16; n++ ) {
        tpu_set( ( ( n < 8 ) ? n : n - 8 ) * 8 + 1, ( n < 8 ) ? 10 : 11, TRANSPARENT, ( n == machine.STACKTOP ) ? BLACK : GREY3 );
        tpu_printf( 0, "[%03x]",machine.STACK[ n ] );
    }

    for( int y = 0; y < 4; y++ ) {
        for( int x = 0; x < 4; x++ ) {
            int number;
            if( keys[ y * 4 + x ] >= 'A' ) { number = keys[ y * 4 + x ] - 'A' + 10; } else { number = keys[ y * 4 + x ] - '0'; }
            tpu_set( x * 3 + 1, 13 + y * 3, _rv64_bext( machine.KEYS, number ) ? GREY6 : GREY3, BLACK );
                tpu_print( 1, "   " );
                tpu_move( x * 3 + 1, 14 + y * 3 ); tpu_printf( 1, " %c ", keyboard[ y * 4 + x ] );
                tpu_move( x * 3 + 1, 15 + y * 3 ); tpu_print( 1, "   " );
            tpu_set( x * 3 + 17, 13 + y * 3, _rv64_bext( machine.KEYS, number ) ? GREY6 : GREY3, BLACK );
                tpu_print( 1, "   " );
                tpu_move( x * 3 + 17, 14 + y * 3 ); tpu_printf( 1, " %c ", keys[ y * 4 + x ] );
                tpu_move( x * 3 + 17, 15 + y * 3 ); tpu_print( 1, "   " );
        }
    }

    for( int y = 0; y < 3; y++ ) {
        for( int x = 0; x < 4; x++ ) {
            tpu_set( 33 + x * 8, 13 + y * 4, BLUE, YELLOW ); tpu_print( 1, "      " );
            tpu_set( 33 + x * 8, 14 + y * 4, BLUE, YELLOW ); tpu_print( 1, fkeys[ y * 4 + x ] );
            tpu_set( 33 + x * 8, 15 + y * 4, BLUE, YELLOW ); tpu_print( 1, factions[ y * 4 + x ] );
            tpu_set( 33 + x * 8, 16 + y * 4, BLUE, YELLOW ); tpu_print( 1, "      " );
        }
    }
}


// MONITOR THE KEYBOARD AND SET/CLR BITS IN THE KEYS REGISTER
// CHECK FOR FUNCTION KEYS AND TAKE ACTION / FLAG MAIN LOOP
__attribute__((used)) void interactivity( void ) {
    int pass_control = 0, needvblank = 0, presentvblank;

    ps2_keyboardmode( TRUE );                                                                                                   // SWITCH TO PS2 MODE TO DETECT KEY PRESSES

    while( !pass_control ) {
        if( ps2_event_available() ) {
            uint16_t keycode = ps2_event_get();
            int keypressed = -1;
            switch( keycode & 0x1ff ) {
                case 0x05: if( keycode & 0x200 ) { machine.running = 0; machine.MODE = CHIP8; } break;                          // F1
                case 0x06: if( keycode & 0x200 ) { machine.running = 0; machine.MODE = CHIP48; } break;                         // F2
                case 0x04: if( keycode & 0x200 ) { machine.running = 0; machine.MODE = SCHIP; } break;                          // F3
                case 0x0c: if( keycode & 0x200 ) { machine.running = 0; machine.MODE = XOCHIP; } break;                         // F4

                case 0x83: if( keycode & 0x200 ) { machine.debug = 1 - machine.debug; }; break;                                 // F7
                case 0x0a: if( keycode & 0x200 ) { machine.limit = ( machine.limit == 2 ) ? 0 : machine.limit + 1; } break;     // F8

                case 0x01: if( keycode & 0x200 ) { machine.running = 0; machine.quit = 1; pass_control = 1; } break;            // F9
                case 0x78: if( keycode & 0x200 ) { machine.restart = 1; } break;                                                // F11
                case 0x07: if( keycode & 0x200 ) { machine.running = 0; machine.loading = 1; pass_control = 1; } break;         // F12

                case 0x16: keypressed = 1; break;                                                                               // 1 == 1
                case 0x1e: keypressed = 2; break;                                                                               // 2 == 2
                case 0x26: keypressed = 3; break;                                                                               // 3 == 3
                case 0x25: keypressed = 12; break;                                                                              // 4 == C
                case 0x15: keypressed = 4; break;                                                                               // Q == 4
                case 0x1d: keypressed = 5; break;                                                                               // W == 5
                case 0x24: keypressed = 6; break;                                                                               // E == 6
                case 0x2d: keypressed = 13; break;                                                                              // R == D
                case 0x1c: keypressed = 7; break;                                                                               // A == 7
                case 0x1b: keypressed = 8; break;                                                                               // S == 8
                case 0x23: keypressed = 9; break;                                                                               // D == 9
                case 0x2b: keypressed = 14; break;                                                                              // F == E
                case 0x1a: keypressed = 10; break;                                                                              // Z == A
                case 0x22: keypressed = 0; break;                                                                               // X == 0
                case 0x21: keypressed = 11; break;                                                                              // C == B
                case 0x2a: keypressed = 15; break;                                                                              // V == F
            }
            if( keypressed != -1 ) {
                machine.KEYS = ( keycode & 0x200 ) ? _rv64_bset( machine.KEYS, keypressed ) : _rv64_bclr( machine.KEYS, keypressed );
            }
        }

        presentvblank = is_vblank();
        if( needvblank == presentvblank ) {                                                                                     // IF NEW VBLANK
            if( presentvblank ) {
                if( machine.HIRES ) { draw_screen_hires(); } else { draw_screen_lores(); }                                      // DRAW THE SCREEN
                display_state();                                                                                                // DISPLAY STATUS
            }
            needvblank = 1 - needvblank;
        }
    }

    ps2_keyboardmode( FALSE );                                                                                                  // SWITCH TO JOYSTICK MODE FOR FILE SELECTOR
    SMTSTOP();
}

void smt_thread( void ) {
    asm volatile ("li sp, 0x4000");
    asm volatile ("j interactivity");
}

// DISPLAY FUNCTIONS
int set_pixel( int p, int x, int y ) {
    int max_x = ( 64 << machine.HIRES ) - 1;                                                                                    // MAX COORDINATE MASK FOR WRAPPING
    int max_y = ( 32 << machine.HIRES ) - 1;                                                                                    // MAX COORDINATE MASK FOR WRAPPING

    if( ( machine.MODE != XOCHIP ) && ( ( x > max_x ) || ( y > max_y ) ) ) {
        return( 0 );                                                                                                            // OUT OF RANGE AND NOT XO-CHIP, NO ACTION
    } else {
        x = ( x & max_x ) << ( 1 - machine.HIRES );                                                                             // WRAP AND ADJUST IF LORES
        y = ( y & max_y ) << ( 1 - machine.HIRES );
    }

    int temp = _rv64_bext( machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ], 64 - ( x & 63 ) );                                  // SAVE CURRENT PIXEL

    switch( machine.HIRES ) {
        case 0:                                                                                                                 // HIRES SET 2x2 PIXELS
            machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ], 64 - ( x ) );
            machine.DISPLAY[ p ][ y ][ _rv64_bext( x + 1, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y ][ _rv64_bext( x + 1, 6 ) ], 64 - ( x + 1 ) );
            machine.DISPLAY[ p ][ y + 1 ][ _rv64_bext( x, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y + 1 ][ _rv64_bext( x, 6 ) ], 64 - ( x ) );
            machine.DISPLAY[ p ][ y + 1 ][ _rv64_bext( x + 1, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y + 1 ][ _rv64_bext( x + 1, 6 ) ], 64 - ( x + 1 ) );
            break;
        case 1:                                                                                                                 // HIRES SET 1 PIXEL
            machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ], 64 - ( x ) );
            break;
    }
    return( temp );
}

int drawsprite( uint8_t xc, uint8_t yc, uint16_t i, uint8_t n ) {
    int pixelflag, pixelerased = 0, rowpixelerased, hirescount = 0;
    int max_x = ( 64 << machine.HIRES ) - 1;
    int max_y = ( 32 << machine.HIRES ) - 1;

    xc = xc & max_x; yc = yc & max_y;                                                                                           // WRAP COORDINATES IF REQUIRED

    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    if( n ) {
        for( int p = min_p; p < max_p; p++ ) {
            for( int y = 0; ( y < n ); y++ ) {                                                                                     // N != 0, DRAW STANDARD 8 x n SPRITE
                rowpixelerased = 0;
                for( int x = 0; ( x < 8 ); x++ ) {
                    if( _rv64_bext( machine.MEMORY[ i ], 7 - x ) ) {
                        pixelflag = set_pixel( p, xc + x, yc + y );
                        pixelerased |= pixelflag;
                        rowpixelerased |= pixelflag;
                    }
                }
                hirescount += rowpixelerased;
                i = ADDRESS( i, 1 );
            }
        }
    } else {                                                                                                                    // N == 0, DRAW 16 HEIGHT SPRITE
        for( int p = min_p; p < max_p; p++ ) {
            for( int y = 0; ( y < 16 ); y++ ) {
                uint16_t spritedata = FETCH( i );
                rowpixelerased = 0;
                for( int x = 0; ( x < 16 ); x++ ) {                                                                                     // ONLY DRAW 8 WIDE IF IN LORES
                    if( _rv64_bext( spritedata, 15 - x ) ) {
                        pixelflag = set_pixel( p, xc + x, yc + y );
                        pixelerased |= pixelflag;
                        rowpixelerased |= pixelflag;
                    }
                }
                hirescount += rowpixelerased;
                i = ADDRESS( i, 2 );
            }
        }
    }

    if( machine.HIRES )
        if( yc + ( ( n == 0 ) ? 16 : n ) > max_y )
            hirescount += ( yc + ( ( n == 0 ) ? 16 : n ) ) - max_y;

    return( machine.HIRES ? hirescount : pixelerased );
}

void scroll_left( void ) {
    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    for( int p = min_p; p < max_p; p++ ) {
        for( int y = 0; y < 64; y++ ) {
            int temp = _rv64_rol( machine.DISPLAY[ p ][ y ][ 1 ], 4 ) & 0xf;                                                         // EXTRACT PIXELS 64, 65, 66, 67
            machine.DISPLAY[ p ][ y ][ 0 ] = ( machine.DISPLAY[ p ][ y ][ 0 ] << 4 ) | temp;                                              // SHIFT LEFT PIXELS 0 - 63 AND OR IN PIXELS 64, 65, 66, 67
            machine.DISPLAY[ p ][ y ][ 1 ] = machine.DISPLAY[ p ][ y ][ 1 ] << 4;                                                         // SHIFT LEFT PIXELS 64 - 127
        }
    }
}

void scroll_right( void ) {
    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    for( int p = min_p; p < max_p; p++ ) {
        for( int y = 0; y < 64; y++ ) {
            int temp = _rv64_ror( machine.DISPLAY[ p ][ y ][ 0 ] & 0xf, 4 );                                                         // EXTRACT PIXELS 60, 61, 62, 63
            machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y ][ 0 ] >> 4;                                                         // SHIFT RIGHT PIXELS 0 - 63 AND OR IN PIXELS 64, 65, 66, 67
            machine.DISPLAY[ p ][ y ][ 1 ] = ( machine.DISPLAY[ p ][ y ][ 1 ] >> 4 ) | temp;                                              // SHIFT LEFT PIXELS 64 - 127
        }
    }
}

void scroll_down( uint8_t n ) {
    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    for( int p = min_p; p < max_p; p++ ) {
        for( int y = 63; y >= 0; y-- ) {
            if( y >= n ) {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y - n ][ 0 ];
                machine.DISPLAY[ p ][ y ][ 1 ] = machine.DISPLAY[ p ][ y - n ][ 1 ];
            } else {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y ][ 1 ] = 0;
            }
        }
    }
}

void scroll_up( uint8_t n ) {
    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    for( int p = min_p; p < max_p; p++ ) {
        for( int y = 0; y < 64; y++) {
            if( y < ( 64 - n ) ) {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y + n ][ 0 ];
                machine.DISPLAY[ p ][ y ][ 1 ] = machine.DISPLAY[ p ][ y + n ][ 1 ];
            } else {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y ][ 1 ] = 0;
            }
        }
    }
}

void DEBUG( const char *fmt,... ) {
    if( machine.debug ) {
        static char buffer[1024];
        va_list args;
        va_start (args, fmt);
        vsnprintf( buffer, 80, fmt, args);
        va_end(args);

        fprintf(stderr,buffer);
    }
}

void execute( void ) {
    uint16_t instruction = FETCH( machine.PC );                                                                                 // EXTRACT INSTRUCTION ( wrap PC if required )
    uint8_t O = ( instruction & 0xf000 ) >> 12;                                                                                 // EXTARCT FIRST NIBBLE, OPERAND
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t Yn = ( instruction & 0x00f0 ) >> 4;                                                                                 // EXTRACT Y REGISTER NUMBER
    uint8_t Y = machine.V[ Yn ];                                                                                                // EXTRACT Y REGISTER
    uint8_t N = instruction & 0x000f;                                                                                           // EXTRACT LOW NIBBLE
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE
    uint16_t NNN = instruction & 0x0fff;                                                                                        // EXTRACT ADDRESS ( LOWER 12 BITS )
    uint16_t I = machine.I;                                                                                                     // CURRENT I VALUE
    uint16_t temp;

    DEBUG( "%03x : %04x : ", machine.PC, instruction );

    machine.lastPC = machine.PC; machine.lastinstruction = instruction;                                                         // STORE PC AND INSTRUCTION INCASE OF CRASH
    machine.PC = ADDRESS( machine.PC, 2 );                                                                                      // INCREMENT PC, WRAP AROUND IF REQUIRED

    switch( O ) {
        case 0x0:
            switch( instruction ) {
                case 0x00E0:                                                                                                    // CLEAR THE DISPLAY
                    switch( machine.PLANES ) {
                        case 0: break;
                        case 1: memset( &machine.DISPLAY[0][0][0], 0, PLANESIZE ); break;
                        case 2: memset( &machine.DISPLAY[1][0][0], 0, PLANESIZE ); break;
                        case 3: memset( machine.DISPLAY, 0, DISPLAYSIZE ); break;
                    }
                    DEBUG( "cls");
                    break;
                case 0x00EE:                                                                                                    // RETURN USE STACKTOP ENTRY
                    DEBUG("return");
                    if( machine.STACKTOP == -1 ) {
                        machine.crashed = STACKUFLOW;                                                                           // CHECK FOR STACK UNDERFLOW
                    } else {
                        machine.PC = machine.STACK[ machine.STACKTOP-- ];                                                       // POP PC
                    }
                    break;
                case 0xFB:                                                                                                      // SCROLL DISPLAY RIGHT 4 PIXELS IN SCHIP / XOCHIP MODE
                    DEBUG("scroll(R)");
                    switch( machine.MODE ) {
                        case CHIP8:
                        case CHIP48:
                            machine.crashed = WRONGMODE;
                            break;
                        case SCHIP:
                        case XOCHIP:
                            scroll_right();
                            break;
                    }
                    break;
                case 0xFC:                                                                                                      // SCROLL DISPLAY LEFT 4 PIXELS IN SCHIP / XOCHIP MODE
                    DEBUG("scroll(L)");
                    switch( machine.MODE ) {
                        case CHIP8:
                        case CHIP48:
                            machine.crashed = WRONGMODE;
                            break;
                        case SCHIP:
                        case XOCHIP:
                            scroll_left();
                            break;
                    }
                    break;
                case 0xFD:                                                                                                      // EXIT
                    switch( machine.MODE ) {
                        case CHIP8:
                        case CHIP48:
                            machine.crashed = WRONGMODE;
                            break;
                        case SCHIP:
                        case XOCHIP:
                            machine.running = 0;
                            break;
                    }
                    break;
                case 0xFE:                                                                                                      // SWITCH TO LORES IF IN SCHIP / XOCHIP MODE
                    DEBUG("display(L)");
                    switch( machine.MODE ) {
                        case CHIP8:
                            machine.crashed = WRONGMODE;
                            break;
                        case CHIP48:
                        case SCHIP:
                        case XOCHIP:
                            machine.HIRES = 0;
                            break;
                    }
                    break;
                case 0xFF:                                                                                                      // SWITCH TO HIRES IF IN SCHIP / XOCHIP MODE
                    DEBUG("display(H)");
                    switch( machine.MODE ) {
                        case CHIP8:
                            machine.crashed = WRONGMODE;
                            break;
                        case CHIP48:
                        case SCHIP:
                        case XOCHIP:
                            machine.HIRES = 1;
                            break;
                    }
                    break;
                default:
                    if( ( instruction & 0xfff0 ) == 0x00c0 ) {                                                                  // SCROLL DISPLAY N DOWN IF IN SCHIP / XOCHIP MODE
                        DEBUG("scroll(D by %x)",N );
                        switch( machine.MODE ) {
                            case CHIP8:
                            case CHIP48:
                                machine.crashed = WRONGMODE;
                                break;
                            case SCHIP:
                            case XOCHIP:
                                scroll_down( N );
                                break;
                        }
                    } else {
                        if( ( instruction & 0xfff0 ) == 0x00d0 ) {                                                                  // SCROLL DISPLAY N UP IF IN XOCHIP MODE
                            DEBUG("scroll(D by %x)",N );
                            switch( machine.MODE ) {
                                case CHIP8:
                                case CHIP48:
                                case SCHIP:
                                    machine.crashed = WRONGMODE;
                                    break;
                                case XOCHIP:
                                    scroll_up( N );
                                    break;
                            }
                        } else {
                            machine.crashed = INVALIDINSN;
                        }
                    }
                    break;
            }
            break;
        case 0x1:                                                                                                               // GOTO NNN
            machine.PC = NNN;
            DEBUG("jump %03x",NNN);
            break;
        case 0x2:                                                                                                               // CALL NNN
            if( machine.STACKTOP == 15 ) {
                machine.crashed = STACKOFLOW;                                                                                   // CHECK FOR STACK OVERFLOW
            } else {
                machine.STACK[ ++machine.STACKTOP ] = machine.PC;                                                               // PUSH PC
                machine.PC = NNN;                                                                                               // GOTO NNN
            }
            DEBUG("call %03x",NNN);
            break;
        case 0x6:
            machine.V[ Xn ] = NN;                                                                                               // SET REGISTER VX TO NN
            DEBUG("V%01x = %02x",Xn,NN);
            break;
        case 0x7:
            machine.V[ Xn ] += NN;                                                                                              // ADD NN TO X ( DOES NOT CHANGE VF )
            DEBUG("V%01x += %02x ( %02x + %02X = %02x )",Xn,NN,X,NN,machine.V[ Xn ]);
            break;
        case 0x8:
            switch( N ) {
                case 0x0:
                    VF = 0; machine.V[ Xn ] = machine.V[ Yn ];                                                                          // SET VX TO VY
                    DEBUG("V%01x = V%01x ( %02x )",Xn,Yn,Y);
                    break;
                case 0x1:
                    VF = 0; machine.V[ Xn ] = X | Y;                                                                                    // SET VX TO VX | VY
                    DEBUG("V%01x |= V%01x ( %02x | %02x = %02x )",Xn,Yn,X,Y,machine.V[ Xn ]);
                    break;
                case 0x2:
                    VF = 0; machine.V[ Xn ] = X & Y;                                                                                    // SET VX TO VX & VY
                    DEBUG("V%01x &= V%01x ( %02x & %02x = %02x )",Xn,Yn,X,Y,machine.V[ Xn ]);
                    break;
                case 0x3:
                    VF = 0; machine.V[ Xn ] = X ^ Y;                                                                                    // SET VX TO VX ^ VY
                    DEBUG("V%01x ^= V%01x ( %02x ^ %02x = %02x )",Xn,Yn,X,Y,machine.V[ Xn ]);
                    break;
                case 0x4:
                    temp = X + Y;
                    machine.V[ Xn ] = temp & 0xff;                                                                               // SET VX TO VX + VY
                    VF = _rv64_bext( temp, 8 );                                                                                  // SET CARRY FLAG
                    DEBUG("V%01x += V%01x ( %02x + %02x = (%01x)%02x )",Xn,Yn,X,Y,VF,machine.V[ Xn ]);
                    break;
                case 0x5:
                    temp = X - Y;
                    machine.V[ Xn ] = temp & 0xff;                                                                              // SET VX TO VX - VY
                    VF = ( X >= Y ) ? 1 : 0;                                                                                     // SET CARRY FLAG
                    DEBUG("V%01x -= V%01x ( %02x - %02x = (%01x)%02x )",Xn,Yn,X,Y,VF,machine.V[ Xn ]);
                    break;
                case 0x6:                                                                                                       // SHIFT 1 RIGHT
                    switch( machine.MODE ) {
                        case CHIP8:
                        case CHIP48:
                            machine.V[ Xn ] = ( Y >> 1 );                                                                               // CHIP8 MODE SHIFTS Y THEN PUTS INTO X
                            VF = _rv64_bext( Y, 0 );
                            DEBUG("V%01x = V%01x << 1 ( %02x << 1 = (%01x)%02x )",Xn,Yn,Y,VF,machine.V[ Xn ]);
                            break;
                        case SCHIP:
                        case XOCHIP:
                            machine.V[ Xn ] = ( X >> 1 );                                                                               // OTHER MODES SHIFTS X THEN PUTS INTO X
                            VF = _rv64_bext( X, 0 );
                            DEBUG("V%01x = V%01x << 1 ( %02x << 1 = (%01x)%02x )",Xn,Xn,X,VF,machine.V[ Xn ]);
                            break;
                    }
                    break;
                case 0x7:
                    temp = Y - X;
                    machine.V[ Xn ] = temp & 0xff;                                                                              // SET VX TO VY - VX
                    VF = ( Y >= X ) ? 1 : 0;                                                                                     // SET CARRY FLAG
                    DEBUG("V%01x = V%01x - V%01x ( %02x - %02x = (%01x)%02x )",Xn,Yn,Xn,Y,X,VF,machine.V[ Xn ]);
                    break;
                case 0xe:                                                                                                       // SHIFT 1 LEFT
                    switch( machine.MODE ) {
                        case CHIP8:
                        case CHIP48:
                            machine.V[ Xn ] = ( Y << 1 );                                                                               // CHIP8 MODE SHIFTS Y THEN PUTS INTO X
                            VF = _rv64_bext( Y, 7 );
                            DEBUG("V%01x = V%01x >> 1 ( %02x >> 1 = (%01x)%02x )",Xn,Yn,Y,VF,machine.V[ Xn ]);
                            break;
                        case SCHIP:
                        case XOCHIP:
                            machine.V[ Xn ] = ( X << 1 );                                                                               // OTHER MODES SHIFTS X THEN PUTS INTO X
                            VF = _rv64_bext( X, 7 );
                            DEBUG("V%01x = V%01x >> 1 ( %02x >> 1 = (%01x)%02x )",Xn,Xn,X,VF,machine.V[ Xn ]);
                            break;
                    }
                    break;
                default:
                    machine.crashed = INVALIDINSN;
                    break;
            }
            break;
        case 0xa:
            machine.I = NNN;                                                                                                    // SET I TO NNN
            DEBUG("I = %03x",NNN);
            break;
        case 0xb:
            switch( machine.MODE ) {
                case CHIP8:
                case XOCHIP:
                    machine.PC = ADDRESS( NNN, machine.V[0] );                                                                 // CHIP8 MODE NNN + V[0]
                    DEBUG("branch V0 + %03x ( %02x + %03x = %03x )",NNN,machine.V[0],NNN,machine.PC);
                    break;
                case CHIP48:
                case SCHIP:
                    machine.PC = ADDRESS( NNN, X );                                                                            // OTHER MODES NNN + VX
                    DEBUG("branch V%01x + %03x ( %02x + %03x = %03x )",Xn,NNN,X,NNN,machine.PC);
                    break;
            }
            break;
        case 0xc:
            machine.V[ Xn ] = rng( 256 ) & NN;                                                                                  // RANDOM 0 - 255 ANDED
            DEBUG("V%01x = RAND & %02x ( %02x )",Xn,NN,machine.V[ Xn ]);
            break;
        case 0xd:
            if( N || ( machine.MODE != CHIP8 ) ) {
                VF = drawsprite( X, Y, machine.I, N );
            } else {
                VF = 0;
            }
            DEBUG("draw %03x @ ( %02x, %02x ) X %02x -> %02x",machine.I,X,Y,(N == 0) ? 16 : N,VF);
            break;
        case 0xf:
            switch( NN ) {
                case 0x07:
                    machine.V[ Xn ] = machine.timer;
                    break;
                case 0x15:
                    machine.timer = X;
                    break;
                case 0x18:
                    machine.audio_timer = X; beep( 3, 0, 49, (short)(X * 1000/60) );
                    break;
                case 0x1e:
                    temp = I + X;
                    machine.I = ADDRESS( temp, 0 );
                    VF = _rv64_bext( temp, 12 );
                    DEBUG("I += V%01x ( %03x + %02x = (%01x)%03x )",Xn,I,X,VF,machine.I);
                    break;
                case 0x0a:
                    if( machine.KEYS == 0 ) {
                        machine.PC = ADDRESS( machine.PC, -2 );
                    } else {
                        machine.V[ Xn ] = _rv64_ctz( machine.KEYS );
                    }
                    break;
                case 0x29:
                    machine.I = ADDRESS( 0, 5 * ( X & 0xf ) );
                    break;
                case 0x30:
                    if( machine.MODE != CHIP8 ) {
                        machine.I = ADDRESS( 0x50, 10 * ( X & 0xf ) );
                    } else {
                        machine.crashed = WRONGMODE;
                    }
                    break;
                case 0x33:
                    machine.MEMORY[ ADDRESS( I, 0 ) ] = X / 100;
                    machine.MEMORY[ ADDRESS( I, 1 ) ] = ( X % 100 ) / 10;
                    machine.MEMORY[ ADDRESS( I, 2 ) ] = ( X % 10 );
                    DEBUG("bcd V%01x [ %02x -> %02x %02x %02x ]",Xn,X,machine.MEMORY[ ADDRESS( I, 0 ) ],machine.MEMORY[ ADDRESS( I, 1 ) ],machine.MEMORY[ ADDRESS( I, 2 ) ]);
                    break;
                case 0x55:
                    for( int n = 0; n <= Xn; n++ )
                        machine.MEMORY[ ADDRESS( I, n ) ] = machine.V[ n ];
                    switch( machine.MODE ) {
                        case CHIP8:
                            machine.I =  ADDRESS( I, Xn + 1 );
                            break;
                        case CHIP48:
                            machine.I = ADDRESS( I, Xn );
                        case SCHIP:
                        case XOCHIP:
                            break;
                    }
                    DEBUG("save V0 -> V%01x @ %03x ( I = %03x )",Xn,I,machine.I);
                    break;
                case 0x65:
                    for( int n = 0; n <= Xn; n++ )
                        machine.V[ n ] = machine.MEMORY[ ADDRESS( I, n ) ];
                    switch( machine.MODE ) {
                        case CHIP8:
                            machine.I =  ADDRESS( I, Xn + 1 );
                            break;
                        case CHIP48:
                            machine.I = ADDRESS( I, Xn );
                        case SCHIP:
                        case XOCHIP:
                            break;
                    }
                    DEBUG("load V0 -> V%01x @ %03x ( I = %03x )",Xn,I,machine.I);
                    break;
                case 0x75:
                    if( machine.MODE != CHIP8 ) {
                        for( int n = 0; n <= Xn; n++ )
                            machine.FLAGS[ n ] = machine.V[ n ];
                    } else {
                        machine.crashed = WRONGMODE;
                    }
                    break;
                case 0x85:
                    if( machine.MODE != CHIP8 ) {
                        for( int n = 0; n <= Xn; n++ )
                            machine.V[ n ] = machine.FLAGS[ n ];
                    } else {
                        machine.crashed = WRONGMODE;
                    }
                    break;
                default:
                    machine.crashed = INVALIDINSN;
                    break;
            }
            break;
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x9:
        case 0xe:
            switch( O ) {
                case 0x3: DEBUG("V%01x == NN ( %02x == %02x ) ?",Xn,X,NN); temp = ( X == NN ); break;
                case 0x4: DEBUG("V%01x != NN ( %02x != %02x ) ?",Xn,X,NN); temp = ( X != NN ); break;
                case 0x5: DEBUG("V%01x == V%01x ( %02x == %02x ) ?",Xn,Yn,X,Y); temp = ( X == Y ); break;
                case 0x9: DEBUG("V%01x != V%01x ( %02x != %02x ) ?",Xn,Yn,X,Y); temp = ( X != Y ); break;
                case 0xe:
                    switch( NN ) {
                        case 0x9e: temp = _rv64_bext( machine.KEYS, X & 0xf ); break;
                        case 0xa1: temp =  !_rv64_bext( machine.KEYS, X & 0xf ); break;
                        default: machine.crashed = INVALIDINSN; break;
                    }
                    break;
            }
            if( temp ) {
                DEBUG(" Y"); machine.PC = ADDRESS( machine.PC, 2 );
            } else {
                DEBUG(" N");
            }
            break;
    }
    DEBUG("\n");
}

void restart_machine( void ) {
    memset( &machine.V[0], 0, 16 );                                                                                             // CLEAR REGISTERS
    memcpy( &machine.MEMORY[ 0x000 ], chip8font, FONTSIZE );                                                                    // COPY FONT
    memset( machine.DISPLAY, 0, DISPLAYSIZE );                                                                                   // CLEAR DISPLAY
    machine.HIRES = 0; machine.PLANES = 1;                                                                                      // SET DISPLAY FLAGS
    machine.STACKTOP = -1;                                                                                                      // EMPTY THE STACK
    machine.PC = 0x200; machine.crashed = NONE;                                                                                 // SET PC TO START OF PROGRAM
}

void reset_machine( void ) {
    memset( &machine.V[0], 0, 16 );                                                                                             // CLEAR REGISTERS
    memset( machine.MEMORY, 0, 4096 ); memcpy( &machine.MEMORY[ 0x000 ], chip8font, FONTSIZE );                                 // CLEAR MEMORY, COPY FONT
    memset( machine.DISPLAY, 0, DISPLAYSIZE );                                                                                   // CLEAR DISPLAY
    machine.HIRES = 0; machine.PLANES = 1;                                                                                      // SET DISPLAY FLAGS
    machine.STACKTOP = -1;                                                                                                      // EMPTY THE STACK
    machine.PC = 0x200; machine.debug = 0; machine.crashed = NONE; machine.limit = 1;                                                              // SET PC TO START OF PROGRAM
}

int main( void ) {
    set_background( WHITE, WHITE, BKG_SOLID );
    set_sprite_bitamps_from_spritesheet32x32( UPPER_LAYER, &sprites[0] );                                                       // SET THE STATUS FLAG SPRITES
    reset_machine(); SMTSTART( smt_thread );                                                                                    // START THE KEYBOARD MONITOR

    set_timer1khz( (short)1000/60, 0 );

    while( !machine.quit ) {
        set_timer1khz( ( machine.limit == CYCLE ) ? 1000 : 1, 1 );                                                             // SET !KHz timer, 1000 instructions per second limit

        if( machine.loading ) {
            set_background( BLACK, BLACK, BKG_SOLID );
            gpu_cs(); tpu_cs(); reset_machine();
            int filesize; uint8_t *filebuffer = sdcard_selectfile( "Please select a CH8", "CH8", &filesize, "Running" );    // LOAD A FILE
            if( filebuffer && ( ( filesize > 0 ) & ( filesize < 4096 ) ) ) {
                gpu_cs();                                                                                                   // START EXECUTION
                memcpy( &machine.MEMORY[0x200], filebuffer, filesize );
                machine.running = 1;
            } else {
                machine.crashed = INVALIDFILE;                                                                              // FILE IS INVALID ( WRONG SIZE, CAN't BE READ )
            }
            if( filebuffer ) { free( filebuffer ); }
            machine.loading = 0;
            SMTSTART( smt_thread );                                                                                         // RESTART THE KEYBOARD MONITOR
            set_background( WHITE, WHITE, BKG_SOLID );
        }

        if( machine.restart ) {                                                                                                 // CHECK FOR RESTART ( F11 )
            restart_machine();
            machine.restart = 0;
            machine.running = 1;
        }

        if( machine.running && ( machine.crashed == NONE ) ) {                                                                  // CHECK IF RUNNING, EXECUTE 1 INSTRUCTION
            execute();
        }

        if( !get_timer1khz( 0 ) ) {                                                                                         // 1/60th of a second
            if( machine.timer ) machine.timer--;                                                                            // UPDATE CHIP8 TIMER
            if( machine.audio_timer ) {
                machine.audio_timer--;                                                                                      // UPDATE CHIP8 AUDIO TIMER
            } else {
                beep( 3, 0, 0, 0 );                                                                                         // CANCEL TONE
            }
            set_timer1khz( (short)1000/60, 0 );
        }

        if( machine.limit != MAX ) wait_timer1khz( 1 );                                                                     // LIMIT TO 1 / 1000 INSTRUCTIONS PER SECOND
    }
}

