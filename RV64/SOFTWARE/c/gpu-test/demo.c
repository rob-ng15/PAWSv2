#include <PAWSlibrary.h>

// BLITTER BITMAPS - ALIENS FROM SPACE INVADERS
unsigned short blitter_bitmaps[] = {
    0b0000011000000000,
    0b0000111110000000,
    0b0001111111000000,
    0b0011011011000000,
    0b0011111111000000,
    0b0001011010000000,
    0b0010000001000000,
    0b0001000010000000,
    0,0,0,0,0,0,0,0,

    0b0000011000000000,
    0b0000111110000000,
    0b0001111111000000,
    0b0011011011000000,
    0b0011111111000000,
    0b0000100100000000,
    0b0001011010000000,
    0b0010100101000000,
    0,0,0,0,0,0,0,0,

    0b0010000010000000,
    0b0001000100000000,
    0b0011111110000000,
    0b0110111011000000,
    0b1111111111100000,
    0b1011111110100000,
    0b1010000010100000,
    0b0001101100000000,
    0,0,0,0,0,0,0,0,

    0b0010000010000000,
    0b1001000100100000,
    0b1011111110100000,
    0b1110111011100000,
    0b1111111111100000,
    0b0111111111000000,
    0b0010000010000000,
    0b0100000001000000,
    0,0,0,0,0,0,0,0,

    0b0000111100000000,
    0b0111111111100000,
    0b1111111111110000,
    0b1110011001110000,
    0b1111111111110000,
    0b0011100111000000,
    0b0110011001100000,
    0b0011000011000000,
    0,0,0,0,0,0,0,0,

    0b0000111100000000,
    0b0111111111100000,
    0b1111111111110000,
    0b1110011001110000,
    0b1111111111110000,
    0b0001100110000000,
    0b0011011011000000,
    0b1100000000110000,
    0,0,0,0,0,0,0,0
};

// STORAGE FOR COLOUR BLITTER
unsigned char colour_blitter_strings[][16] = {

    "....C.C.........",
    "....C.C.........",
    "C..CCCCC..C.....",
    "CCCCYCYCCCC.....",
    "...CCCCC........",
    ".BBBCCCBBB......",
    "BBB.CCC.BBB.....",
    "BB...C...BB.....",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",

    "....C.C.........",
    "C...C.C...C.....",
    "CCCCCCCCCCC.....",
    "...CYCYC........",
    "BBBCCCCCBBB.....",
    "BB.CCCCC.BB.....",
    "B...CCC...B.....",
    ".....C..........",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",

    "C...C.C...C.....",
    "C..CCCCC..C.....",
    ".CCCYCYCCC......",
    "B..CCCCC..B.....",
    "BBBBCCCBBBB.....",
    ".BB.CCC.BB......",
    ".....C..........",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................"
};

unsigned char colour_blitter_bitmap[ 256 ];

void displayreset( void ) {
    // RESET THE DISPLAY
    screen_mode( 0, MODE_RGBM, 0 ); bitmap_256( false );
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

// CHARACTER MAP DISPLAY
void charactermapdemo( void ) {
    displayreset();

    unsigned char foreground = 0, x = 0, y = 0;

    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "Character Map Test" );

    for( int c = 3; c < 7; c++ ) {
        for( int i = 0; i < 256; i++ ) {
            tpu_set( x++, y, TRANSPARENT, foreground++, TPU_NORMAL );
            tpu_output_character( c );
            if( x == 80 ) { x = 0; y++; }
        }
        y++;
    }

    x = 0; y += 2;
    for( int i = 0; i < 256; i++ ) {
        tpu_set( x++, y, BLACK, WHITE, TPU_BOLD );
        tpu_output_character( i );
        if( x == 80 ) { x = 0; y++; }
    }

    x = 0; y += 2;
    for( int i = 0; i < 256; i++ ) {
        tpu_set( x, y, BLACK, WHITE, TPU_X2 );
        tpu_output_character( i ); tpu_output_character( i );
        x += 2; if( x == 80 ) { x = 0; y++; }
    }

    x = 0; y += 2;
    for( int i = 0; i < 256; i++ ) {
        tpu_set( x++, y, BLACK, WHITE, TPU_BLINK );
        tpu_output_character( i );
        if( x == 80 ) { x = 0; y++; }
    }

    x = 0; y += 2;
    for( int i = 0; i < 256; i++ ) {
        tpu_set( x++, y, BLACK, WHITE, TPU_UNDER );
        tpu_output_character( i );
        if( x == 80 ) { x = 0; y++; }
    }

    sleep1khz( 50, 0 );
}

// WORK THROUGH THE VARIOUS GPU FUNCTIONS
void gpudemo( void ) {
    unsigned short i;
    short x1, y1, x2, y2, x3, y3;
    unsigned char colour;

    displayreset();

    // POINTS
    gpu_cs();
    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "GPU Plot Pixels Test" );
    for( i = 0; i < 64; i++ ) {
        gpu_pixel( rng( 256 ), rng( 320 ), rng( 240 ) );
    }
    sleep1khz( 50, 0 );

    // LINES
    gpu_cs();
    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "GPU Line Drawing Test" );
    for( i = 0; i < 64; i++ ) {
        gpu_line( rng( 256 ), rng( 320 ), rng( 240 ), rng( 320 ), rng( 240 ) );
    }
    sleep1khz( 50, 0 );

    // WIDE LINES
    gpu_cs();
    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "GPU Wide Line Drawing Test" );
    for( i = 0; i < 64; i++ ) {
        gpu_wideline( rng( 256 ), rng( 320 ), rng( 240 ), rng( 320 ), rng( 240 ), rng(8) + 1 );
    }
    sleep1khz( 50, 0 );

    // RECTANGLES
    gpu_cs();
    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "GPU Rectangle Drawing Test - Solid & Dither" );
    for( i = 0; i < 64; i++ ) {
        gpu_dither( rng(16), rng( 256 ) );
        gpu_rectangle( rng( 256 ), rng( 352 ) - 16, rng( 256 ) - 8, rng( 352 ) - 16, rng( 256 ) - 8 );
    }
    gpu_dither( DITHEROFF );
    sleep1khz( 50, 0 );

    // CIRCLES
    gpu_cs();
    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "GPU Circle Drawing Test - Solid & Dither" );
    for( i = 0; i < 64; i++ ) {
        gpu_dither( rng(16), rng( 256 ) );
        gpu_circle( rng( 256 ), rng( 352 ) - 16, rng( 256 ) - 8, rng( 32 ), 255, rng( 1 ) );
    }
    gpu_dither( DITHEROFF );
    sleep1khz( 50, 0 );

    // TRIANGLES
    gpu_cs();
    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "GPU Triangle Drawing Test - Solid & Dither" );
    for( i = 0; i < 64; i++ ) {
        x1 = rng( 352 ) - 16; y1 = rng( 256 ) - 8;
        x2 = x1 + rng( 100 ); y2 = y1 + rng( 100 );
        x3 = x2 - rng( 100 ); y3 = y1 + rng( 100 );
        gpu_dither( rng(16), rng( 256 ) );
        gpu_triangle( rng( 256 ), x1, y1, x2, y2, x3, y3 );
    }
    gpu_dither( DITHEROFF );
    sleep1khz( 50, 0 );

    // BLITTER
    // SET BLITTER OBJECTS - ALIENS
    for( short i = 0; i < 6; i++ ) {
        set_blitter_bitmap( i, &blitter_bitmaps[ 16 * i ] );
    }
    gpu_cs();
    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "GPU Blitter Test - REFLECT & ROTATE" );
    for( i = 0; i < 64; i++ ) {
        gpu_blit( rng( 256 ), rng( 352 ) - 16, rng( 256 ) - 8, rng( 6 ), rng( 4 ), rng(8) );
    }
    sleep1khz( 50, 0 );

    // CHARACTER BLITTER
    gpu_cs();
    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "GPU Character Blitter Test - REFLECT & ROTATE" );
    for( i = 0; i < 64; i++ ) {
        gpu_character_blit( rng( 256 ), rng( 352 ) - 16, rng( 256 ) - 8, rng( 256 ), rng( 4 ), rng(8) );
    }
    sleep1khz( 50, 0 );

    // COLOUR BLITTER
    // SET COLOUR BLITTER OBJECTS - ALIENS FROM GALAXIAN
    // SET COLOUR BLITTER OBJECTS - ALIENS FROM GALAXIAN
    for( short i = 0; i < 3; i++ ) {
        for( short j = 0; j < 3; j++ ) {
            for( short y = 0; y < 16; y++ ) {
                for( short x = 0; x < 16; x++ ) {
                    switch( colour_blitter_strings[ j * 16 + y ][x] ) {
                        case '.':
                            colour = TRANSPARENT;
                            break;
                        case 'B':
                            colour = BLUE3;
                            break;
                        case 'C':
                            switch( i ) {
                                case 0:
                                    colour = RED3;
                                    break;
                                case 1:
                                    colour = PURPLE;
                                    break;
                                case 2:
                                    colour = 0x24;
                                    break;
                            }
                            break;
                        case 'Y':
                            colour = ( i == 0 ) ? 0xf0 : 0xc0;
                            break;
                    }
                    colour_blitter_bitmap[ y * 16 + x ] = colour;
                }
            }
            set_colourblitter_bitmap( i * 3 + j, &colour_blitter_bitmap[ 0 ] );
        }
    }

    gpu_cs();
    tpu_print_centre( 59, TRANSPARENT, WHITE, 1, "GPU Colour Blitter Test - REFLECT & ROTATE" );
    for( i = 0; i < 64; i++ ) {
        gpu_colourblit( rng( 352 ) - 16, rng( 256 ) - 8, rng( 9 ), rng( 4 ), rng(8) );
    }
    sleep1khz( 50, 0 );
}

int main( int argc, char **argv ) {
	while(1) {
        charactermapdemo();

        gpudemo();
    }
}
