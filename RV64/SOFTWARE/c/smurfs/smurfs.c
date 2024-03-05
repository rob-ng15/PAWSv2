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

// LOAD THE TILEMAPS
#include "graphics/TM-village.h"

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

unsigned char tune_bass[] = {   41,

                                61, 45,
                                31, 37, 35,
                                27, 31,

                                0xff };

unsigned short size_bass [] = { 128,

                                128, 128,
                                128, 96, 32,
                                128, 128,

                                0xff };

unsigned char harmonic_wave[256] = {
    127,137,147,156,165,174,182,190,196,203,208,213,216,220,223,225,226,
    228,229,230,231,232,233,234,235,236,238,239,241,243,245,247,249,250,
    252,253,254,254,254,253,252,251,249,246,243,240,237,234,231,227,224,
    222,219,217,216,215,215,215,216,217,219,221,223,225,228,230,233,234,
    236,237,238,238,237,236,234,231,228,224,220,215,210,204,199,194,188,
    183,178,174,169,166,162,159,156,154,152,150,148,146,144,143,141,139,
    136,134,131,128,125,122,118,115,111,108,105,102,99,97,96,95,95,
    96,97,99,102,105,108,113,117,122,127,132,137,141,146,149,152,
    155,157,158,159,159,158,157,155,152,149,146,143,139,136,132,129,126,
    123,120,118,115,113,111,110,108,106,104,102,100,98,95,92,88,85,
    80,76,71,66,60,55,50,44,39,34,30,26,23,20,18,17,16,
    16,17,18,20,21,24,26,29,31,33,35,37,38,39,39,39,38,
    37,35,32,30,27,23,20,17,14,11,8,5,3,2,1,0,0,
    0,1,2,4,5,7,9,11,13,15,16,18,19,20,21,22,23,
    24,25,26,28,29,31,34,38,41,46,51,58,64,72,80,89,98,
    107,117
};

// SMT THREAD TO PLAY THE INTRO TUNE
__attribute__((used)) void playtune( void ) {
    short trebleposition = 0, bassposition = 0;

    while( ( tune_treble[ trebleposition ] != 0xff ) || ( tune_bass[ bassposition ] != 0xff ) ) {
        if( tune_treble[ trebleposition ] != 0xff ) {
            if( !get_beep_active( 1 ) ) {
                beep( 1, WAVE_USER, tune_treble[ trebleposition ] * 2 + 3, size_treble[ trebleposition ] << 2 );
                trebleposition++;
            }
        }
        if( tune_bass[ bassposition ] != 0xff ) {
            if( !get_beep_active( 2 ) ) {
                beep( 2, WAVE_USER, tune_bass[ bassposition ], size_bass[ bassposition ] << 2 );
                bassposition++;
            }
        }
    }
    SMTSTOP();
}
void smt_thread( void ) {
    // SETUP STACKPOINTER FOR THE SMT THREAD
    asm volatile ("li sp, 0x5f80000");
    asm volatile ("j playtune");
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

    // SET CLOUD TILEMAPS
    for( int i = 0; i < 6; i++ ) {
        set_tilemap_bitmap32x32( LOWER_LAYER, 1 + ( i * 4 ), &cloud_graphics[ i * 1024 ] );
    }
    // DRAW THE CLOUD WITH SUN
    set_tilemap_tile32x32( LOWER_LAYER, 17, 4, 1 );
    set_tilemap_tile32x32( LOWER_LAYER, 17, 6, 5 );
    set_tilemap_tile32x32( LOWER_LAYER, 19, 4, 9 );
    set_tilemap_tile32x32( LOWER_LAYER, 19, 6, 13 );

    // DRAW THE OTHER CLOUDS
    set_tilemap_tile32x32( LOWER_LAYER, 1, 12, 17 );
    set_tilemap_tile32x32( LOWER_LAYER, 3, 12, 21 );

    set_tilemap_tile32x32( LOWER_LAYER, 27, 8, 17 );
    set_tilemap_tile32x32( LOWER_LAYER, 29, 8, 21 );

}
void display_village( void ) {
    int BDx = 0, BDx_last = 0, BDwidth = 1024, FDx = 0, FDx_last = 0, FDwidth = 4608, anim_number = 0;

    // DISPLAY VILLAGE + START TUNE
    bitmap_display( 3 ); SMTSTART( smt_thread );

    while( FDx < ( FDwidth - 320 ) ) {
        await_vblank();
        if( BDx_last != BDx ) {
            BDx_last = BDx; paws_memcpy_rectangle( (const void *restrict)(0x2000000+88*320), BD_village + BDx, 320, 320, BDwidth, 152 );
            tilemap_scrollwrapclear( LOWER_LAYER, TM_LEFT, 1 );
        }
        paws_memcpy_rectangle( (const void *restrict)(0x2020000+32*320), FD_village + FDx, 320, 320, FDwidth, 208 );
        set_sprite32( UPPER_LAYER, 0, SPRITE_SHOW, 320, 416, (anim_number) & 7, SPRITE_DOUBLE );
        FDx+=2; if( !(FDx & 3) ) { anim_number++; if( BDx == 510 ) { BDx = 0; } else { BDx+=2; } }
    }
    while( FDx > 0 ) {
        await_vblank();
        if( BDx_last != BDx ) {
            BDx_last = BDx; paws_memcpy_rectangle( (const void *restrict)(0x2000000+88*320), BD_village + BDx, 320, 320, BDwidth, 152 );
            tilemap_scrollwrapclear( LOWER_LAYER, TM_RIGHT, 1 );
        }
        paws_memcpy_rectangle( (const void *restrict)(0x2020000+32*320), FD_village + FDx, 320, 320, FDwidth, 208 );
        set_sprite32( UPPER_LAYER, 0, SPRITE_SHOW, 320, 416, (anim_number) & 7, SPRITE_DOUBLE | REFLECT_X);
        FDx-=2; if( !(FDx & 3) ) { anim_number++; if( BDx == 0 ) { BDx = 510; } else { BDx-=2; } }
    }
}

int main( void ) {
    wavesample_upload( CHANNEL_BOTH, harmonic_wave );
    displayreset();
    display_village();

    sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
