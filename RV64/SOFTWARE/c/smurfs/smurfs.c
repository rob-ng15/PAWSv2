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

// SMT THREAD TO PLAY THE INTRO TUNE
short trebleposition = 0, bassposition = 0;
void __attribute__((interrupt ("machine"))) playtune( void ) {
    IRQ_ACK( IRQ_VBLANK );

    if( ( tune_treble[ trebleposition ] != 0xff ) || ( tune_bass[ bassposition ] != 0xff ) ) {
        if( tune_treble[ trebleposition ] != 0xff ) {
            if( !get_beep_active( ACTIVE_CHANNEL_LEFT_0 ) ) {
                beep( CHANNEL_LEFT_0, WAVE_WOOD, tune_treble[ trebleposition ] * 2 + 3, size_treble[ trebleposition ] << 2, 7 );
                trebleposition++;
            }
        }
        if( tune_bass[ bassposition ] != 0xff ) {
            if( !get_beep_active( ACTIVE_CHANNEL_RIGHT_0 ) ) {
                beep( CHANNEL_RIGHT_0, WAVE_BRASS, tune_bass[ bassposition ], size_bass[ bassposition ] << 2, 7 );
                bassposition++;
            }
        }
    } else {
        IRQ_OFF( IRQ_VBLANK, TRUE ); trebleposition = 0; bassposition = 0;
    }
}

// RESET THE DISPLAY
void displayreset( void ) {
    screen_mode( MODE_RGBM );
    bitmap_draw( 3 ); screen_order( FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE ); screen_dimmer( 0 );
    gpu_cs();
    tpu_cs();
    tm_cs( 0 );
    tm_cs( 2 );
    set_background( 102, 102, BKG_SOLID );
    for( short i = 0; i < 64; i++ ) {
        set_sprite_attribute( i, ATTR_SPRITE_ACTIVE, 0 );
    }

    // SET THE INITIAL SPRITES FROM THE SPRITESHEET
    set_sprite_bitamps_from_spritesheet32x32( 0, &spritesheet_upper_1[0] );

    // SET CLOUD TILEMAPS
    for( int i = 0; i < 6; i++ ) {
        set_tilemap_bitmap32x32( 0, 1 + ( i * 4 ), &cloud_graphics[ i * 1024 ] );
    }
    // DRAW THE CLOUD WITH SUN
    set_tilemap_32x32tile_abs( 0, 17, 4, 1 );
    set_tilemap_32x32tile_abs( 0, 17, 6, 5 );
    set_tilemap_32x32tile_abs( 0, 19, 4, 9 );
    set_tilemap_32x32tile_abs( 0, 19, 6, 13 );

    // DRAW THE OTHER CLOUDS
    set_tilemap_32x32tile_abs( 0, 1, 12, 17 );
    set_tilemap_32x32tile_abs( 0, 3, 12, 21 );

    set_tilemap_32x32tile_abs( 0, 27, 8, 17 );
    set_tilemap_32x32tile_abs( 0, 29, 8, 21 );

}
void display_village( void ) {
    int BDx = 0, BDx_last = 0, BDwidth = 1024, FDx = 0, FDx_last = 0, FDwidth = 4608, anim_number = 0;

    // DISPLAY VILLAGE + START TUNE
    screen_order( LAYER_CHARACTERMAP, LAYER_SPRITES_3, LAYER_SPRITES_2, LAYER_SPRITES_1, LAYER_SPRITES_0, LAYER_BITMAP_1, LAYER_BITMAP_0, LAYER_TILEMAP_2, LAYER_TILEMAP_0, FALSE, FALSE );

    while( FDx < ( FDwidth - 320 ) ) {
        await_vblank();
        if( BDx_last != BDx ) {
            BDx_last = BDx; paws_memcpy_rectangle( (const void *restrict)(0x2000000+88*320), BD_village + BDx, 320, 320, BDwidth, 152 );
            tilemap_scroll( 0, TM_LEFT, 1 );
        }
        paws_memcpy_rectangle( (const void *restrict)(0x2020000+32*320), FD_village + FDx, 320, 320, FDwidth, 208 );
        set_sprite32( 0, SPRITE_SHOW, 320, 416, (anim_number) & 7, SPRITE_DOUBLE );
        FDx+=2; if( !(FDx & 3) ) { anim_number++; if( BDx == 510 ) { BDx = 0; } else { BDx+=2; } }
    }
    while( FDx > 0 ) {
        await_vblank();
        if( BDx_last != BDx ) {
            BDx_last = BDx; paws_memcpy_rectangle( (const void *restrict)(0x2000000+88*320), BD_village + BDx, 320, 320, BDwidth, 152 );
            tilemap_scroll( 0, TM_RIGHT, 1 );
        }
        paws_memcpy_rectangle( (const void *restrict)(0x2020000+32*320), FD_village + FDx, 320, 320, FDwidth, 208 );
        set_sprite32( 0, SPRITE_SHOW, 320, 416, (anim_number) & 7, SPRITE_DOUBLE | REFLECT_X);
        FDx-=2; if( !(FDx & 3) ) { anim_number++; if( BDx == 0 ) { BDx = 510; } else { BDx-=2; } }
    }
}

int main( void ) {
    displayreset(); IRQ_VECTOR( (void *)playtune ); IRQ_ON( IRQ_VBLANK, TRUE );

    display_village();

    sleep1khz( 4000 );
}

// EXIT WILL RETURN TO BIOS
