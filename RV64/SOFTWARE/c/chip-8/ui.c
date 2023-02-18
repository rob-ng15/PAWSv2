#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "chip-8.h"

// LOAD THE UI GRAPHICS
unsigned char sprites[] = {
#include "CHIP8-UI.h"
};

unsigned char *modes[] = {
    "CHIP-8 ",
    "CHIP-48",
    "S-CHIP ",
    "XO-CHIP"
};

unsigned char *crashes[] = {
    "                   ",
    "INVALID INSTRUCTION",
    "WRONG MODE         ",
    "INVALID FILE       ",
    "STACK OVERFLOW     ",
    "STACK UNDERFLOW    "
};

unsigned char *states[] = {
    "PAUSED ",
    "RUNNING"
};

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

extern struct C8 machine;                                                                                                              // INSTANCE OF THE CHIP8 MACHINE

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

void restart_machine( void ) {
    memset( &machine.V[0], 0, 16 );                                                                                             // CLEAR REGISTERS
    memcpy( machine.MEMORY, chip8font, FONTSIZE );                                                                    // COPY FONT
    memset( machine.DISPLAY, 0, DISPLAYSIZE );                                                                                   // CLEAR DISPLAY
    machine.HIRES = 0; machine.PLANES = 1;                                                                                      // SET DISPLAY FLAGS
    machine.STACKTOP = -1;                                                                                                      // EMPTY THE STACK
    machine.PC = 0x200; machine.crashed = NONE;                                                                                 // SET PC TO START OF PROGRAM
}

void reset_machine( void ) {
    memset( &machine.V[0], 0, 16 );                                                                                             // CLEAR REGISTERS
    memset( machine.MEMORY, 0, MEMSIZE ); memcpy( machine.MEMORY, chip8font, FONTSIZE );                                           // CLEAR MEMORY, COPY FONT
    memset( machine.DISPLAY, 0, DISPLAYSIZE );                                                                                   // CLEAR DISPLAY
    machine.HIRES = 0; machine.PLANES = 1;                                                                                      // SET DISPLAY FLAGS
    machine.STACKTOP = -1;                                                                                                      // EMPTY THE STACK
    machine.PC = 0x200; machine.debug = 0; machine.crashed = NONE; machine.limit = 1;                                                              // SET PC TO START OF PROGRAM
}

extern void execute( void );

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
            if( filebuffer && ( filesize > 0 ) && ( filesize < 65536 ) ) {
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
