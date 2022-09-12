#include <string.h>
#include <PAWSlibrary.h>

// LOAD THE BACKDROPS
unsigned char BD_village[] = {
#include "graphics/BD-village.h"
};

// LOAD THE FRONTDROPS
unsigned char FD_village[] = {
#include "graphics/FD-village.h"
};

// LOAD THE SPRITES
#include "graphics/spritesheet-1.h"

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

unsigned short size_treble[] = { 128,                           // OPENING BAR

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

unsigned char tune_bass[] = {   17,

                                27, 21,
                                7, 13, 11,
                                3, 7,

                                0xff };

unsigned short size_bass [] = { 128,

                                128, 128,
                                128, 96, 32,
                                128, 128,

                                0xff };

// SMT THREAD TO PLAY THE INTRO TUNE
void playtune( void ) {
    short trebleposition = 0, bassposition = 0;

    while( ( tune_treble[ trebleposition ] != 0xff ) || ( tune_bass[ bassposition ] != 0xff ) ) {
        if( tune_treble[ trebleposition ] != 0xff ) {
            if( !get_beep_active( 1 ) ) {
                beep( 1, WAVE_SINE, tune_treble[ trebleposition ] * 2 + 3, size_treble[ trebleposition ] << 2 );
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
    screen_mode( 0, MODE_RGBM, 0 );
    bitmap_draw( 3 ); bitmap_display( 0 ); screen_dimmer( 0 );
    gpu_cs();
    tpu_cs();
    tilemap_scrollwrapclear( LOWER_LAYER, TM_CLEAR );
    tilemap_scrollwrapclear( UPPER_LAYER, TM_CLEAR );
    set_background( 102, 102, BKG_SOLID );
    for( short i = 0; i < 16; i++ ) {
        set_sprite_attribute( LOWER_LAYER, i, SPRITE_ACTIVE, 0 );
        set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, 0 );
    }

    // SET THE INITIAL SPRITES FROM THE SPRITESHEET
    set_sprite_bitamps_from_spritesheet32x32( UPPER_LAYER, &spritesheet_upper_1[0] );
}

void display_village( void ) {
    int BDx = 0, BDx_last = 0, BDwidth = 1024, FDx = 0, FDx_last = 0, FDwidth = 4608, anim_number = 0;

    // DISPLAY VILLAGE + START TUNE
    bitmap_display( 3 ); SMTSTART( (unsigned int )smt_thread );

    while( FDx < ( FDwidth - 320 ) ) {
        await_vblank();
        if( BDx_last != BDx ) { BDx_last = BDx; paws_memcpy_rectangle( (const void *restrict)(0x2000000+32*320), BD_village + BDx, 320, 320, BDwidth, 208 ); }
        paws_memcpy_rectangle( (const void *restrict)(0x2020000+32*320), FD_village + FDx, 320, 320, FDwidth, 208 );
        set_sprite32( UPPER_LAYER, 0, SPRITE_SHOW, 320, 416, (anim_number) & 7, SPRITE_DOUBLE );
        FDx+=2; if( !(FDx & 3) ) { anim_number++; if( BDx == 510 ) { BDx = 0; } else { BDx+=2; } }
    }
    while( FDx > 0 ) {
        await_vblank();
        if( BDx_last != BDx ) { BDx_last = BDx; paws_memcpy_rectangle( (const void *restrict)(0x2000000+32*320), BD_village + BDx, 320, 320, BDwidth, 208 ); }
        paws_memcpy_rectangle( (const void *restrict)(0x2020000+32*320), FD_village + FDx, 320, 320, FDwidth, 208 );
        set_sprite32( UPPER_LAYER, 0, SPRITE_SHOW, 320, 416, (anim_number) & 7, SPRITE_DOUBLE | REFLECT_X);
        FDx-=2; if( !(FDx & 3) ) { anim_number++; if( BDx == 0 ) { BDx = 510; } else { BDx-=2; } }
    }
}

int main( void ) {
    displayreset();
    display_village();

    sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
