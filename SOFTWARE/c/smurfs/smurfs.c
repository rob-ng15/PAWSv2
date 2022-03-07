#include <PAWSlibrary.h>

unsigned char BACKDROPS[][320*240] = {
#include "graphics/BD-map1.h"
,
#include "graphics/BD-map2.h"
,
#include "graphics/BD-village.h"
};

// RESET THE DISPLAY
void displayreset( void ) {
    screen_mode( 0, 0, 0 ); bitmap_draw( 0 ); bitmap_display( 0 );
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

int main( void ) {
    displayreset();

    for( short i = 0; i < 3; i++ ) {
        gpu_pixelblock( 0, 0, 320, 240, TRANSPARENT, BACKDROPS[i] );
        sleep1khz( 4000, 0 );
    }
}

// EXIT WILL RETURN TO BIOS
