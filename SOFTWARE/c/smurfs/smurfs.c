#include <PAWSlibrary.h>

unsigned char BACKDROPS[][320*240] = {
#include "graphics/BD-map2.h"
,
#include "graphics/BD-map1.h"
,
#include "graphics/BD-village.h"
};

unsigned char tune_treble[] = { 32,                             // OPENING BAR

                                37, 32, 34, 30,                 // REPEAT BLOCK
                                27, 32, 29,
                                25, 29, 27,
                                0, 32,
                                37, 32, 36, 30,
                                27, 32, 24,
                                30, 24, 25,
                                0, 32,

                                37, 32, 34, 30,                 // REPEAT BLOCK
                                27, 32, 29,
                                25, 29, 27,
                                0, 32,
                                37, 32, 36, 30,
                                27, 32, 24,
                                30, 24, 25,
                                0, 34,

                                0xff };
unsigned short size_treble[] = {
                                128,                            // OPENING BAR

                                96, 32, 64, 64,                 // REPEAT BLOCK
                                128, 96, 32,
                                64, 64, 128,
                                128, 128,
                                96, 32, 64, 64,
                                128, 96, 32,
                                64, 64, 128,
                                128, 128,

                                96, 32, 64, 64,                 // REPEAT BLOCK
                                128, 96, 32,
                                64, 64, 128,
                                128, 128,
                                96, 32, 64, 64,
                                128, 96, 32,
                                64, 64, 128,
                                128, 128,

                                0xff };

unsigned char tune_bass[] = {   0,
                                0xff };

unsigned short size_bass [] = { 4224,
                                0xff };

// SMT THREAD TO PLAY THE INTRO TUNE
void playtune( void ) {
    short trebleposition = 0, bassposition = 0;

    while( ( tune_treble[ trebleposition ] != 0xff ) || ( tune_bass[ bassposition ] != 0xff ) ) {
        if( tune_treble[ trebleposition ] != 0xff ) {
            if( !get_beep_active( 1 ) ) {
                beep( 1, WAVE_SINE, tune_treble[ trebleposition ], size_treble[ trebleposition ] << 2 );
                trebleposition++;
            }
        }
        if( tune_bass[ bassposition ] != 0xff ) {
            if( !get_beep_active( 2 ) ) {
                beep( 2, WAVE_SINE, tune_bass[ bassposition ], size_bass[ bassposition ] << 2 );
                bassposition++;
            }
        }
    }
    SMTSTOP();
}
void smt_thread( void ) {
    // SETUP STACKPOINTER FOR THE SMT THREAD
    asm volatile ("li sp ,0x4000");

    playtune();
}

// RESET THE DISPLAY
void displayreset( void ) {
    screen_mode( 0, 0, 0 ); bitmap_draw( 0 ); bitmap_display( 0 ); screen_dimmer( 0 );
    gpu_cs();
    tpu_cs();
    tilemap_scrollwrapclear( LOWER_LAYER, TM_CLEAR );
    tilemap_scrollwrapclear( UPPER_LAYER, TM_CLEAR );
    set_background( BLACK, BLACK, BKG_SOLID );
    for( short i = 0; i < 16; i++ ) {
        set_sprite_attribute( LOWER_LAYER, i, SPRITE_ACTIVE, 0 );
        set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, 0 );
    }
}

// DISPLAY THE MAP, ROLLING DOWN FROM THE TOP TO THE VILLAGE
void display_map( void ) {
    // DRAW THE INITIAL PLACEMENT OF THE MAP ( TOP ) AND INCREASE THE BRIGHTNESS
    screen_dimmer( 8 ); gpu_pixelblock( 0, 0, 320, 240, TRANSPARENT, BACKDROPS[0] );
    for( int i = 8; i >= 0; i-- ) { screen_dimmer( i ); sleep1khz( 100, 0 ); }

    // ROLL THE MAP FROM THE TOP
    for( int i = 0; i <= 240; i++ ) {
        await_vblank();
        gpu_pixelblock( 0, 0, 320, 240, TRANSPARENT, &BACKDROPS[0][ i * 320 ] );
    }

     SMTSTART( (unsigned int )smt_thread ); sleep1khz( 2000, 0 );

    for( int i = 0; i <=8; i++ ) { screen_dimmer( i ); sleep1khz( 100, 0 ); }
}

int main( void ) {
    displayreset();

    display_map();

    gpu_pixelblock( 0, 0, 320, 240, TRANSPARENT, BACKDROPS[2] );
    for( int i = 8; i >= 0; i-- ) { screen_dimmer( i ); sleep1khz( 100, 0 ); }
    sleep1khz( 2000, 0 );
}

// EXIT WILL RETURN TO BIOS
