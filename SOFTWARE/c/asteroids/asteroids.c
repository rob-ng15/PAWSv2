#include <stdlib.h>
#include <stdio.h>

// INCLUDE GALAXY BACKDROP
#include "graphics/GALAXY_BMP.h"

#include <PAWSlibrary.h>

// MACROS
// Convert asteroid number to sprite layer and number
#define ASN(a) ( a > 11 ) ? 1 : 0, ( a > 11 ) ? a - 12 : a
#define MAXASTEROIDS 24

#define SHIPSPRITE 15
#define BULLET1SPRITE 14
#define BULLET2SPRITE 13
#define UFOBULLETSPRITE 12

#define ASTEROIDCOLLISION 0xfff
#define BULLET1COLLISION 0x4000
#define BULLET2COLLISION 0x2000
#define UFOBULLETCOLLISION 0x1000
#define SHIPCOLLISION 0x8000

    // GLOBAL VARIABLES
    int counter = 0, score = 0;
    unsigned short lives = 0, level = 0, shield, fuel;

    // SHIP and BULLETS
    short shipx = 312, shipy = 232, shipdirection = 0, resetship = 0, shipexplode = 0, shipmove = 0, shipshield = 0, bulletdirection[2] = { 0, 0 };
    short last_fire;

    // ASTEROIDS and UFO
    unsigned char asteroid_active[MAXASTEROIDS], asteroid_direction[MAXASTEROIDS], ufo_sprite_number = 0xff, ufo_leftright = 0, ufo_bullet_direction = 0;

    // BEEP / BOOP TIMER
    short last_timer = 0;

    // GLOBAL SPRITE UPDATE VALUES
    unsigned short bullet_directions[] = {
        0b1111101000000,
        0b1111110000010,
        0b1111110000100,
        0b1111111000100,
        0b1110000000110,
        0b1110001000100,
        0b1110010000100,
        0b1110010000010,
        0b1110011000000,
        0b1110010011110,
        0b1110010011100,
        0b1110001011100,
        0b1110000011010,
        0b1111111011100,
        0b1111110011100,
        0b1111110011110
    };

    unsigned short asteroid_directions[] = {
        0x3e1, 0x21, 0x3f, 0x3ff, 0x3c1, 0x3e2, 0x22, 0x41, 0x5f, 0x3e, 0x3fe, 0x3df
    };

    unsigned short ufo_directions[] = {
        0x1802, 0x181e, 0x1803, 0x181d
    };

#include "graphics/asteroids-graphics.h"

// PROGRAM THE BACKGROUND COPPER FOR THE FALLING STARS
void program_background( void ) {
    copper_startstop( 0 );
    copper_program( 0, COPPER_WAIT_VBLANK, 7, 0, BKG_SNOW, BLACK, WHITE );
    copper_program( 1, COPPER_WAIT_X, 7, 0, BKG_SNOW, BLACK, WHITE );
    copper_program( 2, COPPER_JUMP, COPPER_JUMP_IF_Y_LESS, 64, 0, 0, 1 );
    copper_program( 3, COPPER_WAIT_X, 7, 0, BKG_SNOW, BLACK, RED );
    copper_program( 4, COPPER_JUMP, COPPER_JUMP_IF_Y_LESS, 128, 0, 0, 3 );
    copper_program( 5, COPPER_WAIT_X, 7, 0, BKG_SNOW, BLACK, ORANGE );
    copper_program( 6, COPPER_JUMP, COPPER_JUMP_IF_Y_LESS, 160, 0, 0, 5 );
    copper_program( 7, COPPER_WAIT_X, 7, 0, BKG_SNOW, BLACK, YELLOW );
    copper_program( 8, COPPER_JUMP, COPPER_JUMP_IF_Y_LESS, 192, 0, 0, 7 );
    copper_program( 9, COPPER_WAIT_X, 7, 0, BKG_SNOW, BLACK, GREEN );
    copper_program( 10, COPPER_JUMP, COPPER_JUMP_IF_Y_LESS, 224, 0, 0, 9 );
    copper_program( 11, COPPER_WAIT_X, 7, 0, BKG_SNOW, BLACK, LTBLUE );
    copper_program( 12, COPPER_JUMP, COPPER_JUMP_IF_Y_LESS, 256, 0, 0, 11 );
    copper_program( 13, COPPER_WAIT_X, 7, 0, BKG_SNOW, BLACK, PURPLE );
    copper_program( 14, COPPER_JUMP, COPPER_JUMP_IF_Y_LESS, 288, 0, 0, 13 );
    copper_program( 15, COPPER_WAIT_X, 7, 0, BKG_SNOW, BLACK, MAGENTA );
    copper_program( 16, COPPER_JUMP, COPPER_JUMP_ON_VBLANK_EQUAL, 0, 0, 0, 15 );
    copper_program( 17, COPPER_JUMP, COPPER_JUMP_ALWAYS, 0, 0, 0, 1 );
    copper_startstop( 1 );
}

void set_asteroid_sprites( void ) {
    for( unsigned char asteroid_number = 0; asteroid_number < MAXASTEROIDS; asteroid_number++ ) {
        set_sprite_bitmaps( ASN(asteroid_number), ( asteroid_number & 1 ) ? &asteroid_sprites_0[0] : &asteroid_sprites_1[0] );
    }
}

void set_ship_vector( void ) {
    set_vector_vertex( 0, 0, 1, 0, 0 );
    set_vector_vertex( 0, 1, 1, 5, 10 );
    set_vector_vertex( 0, 2, 1, 0, 6 );
    set_vector_vertex( 0, 3, 1, -5, 10 );
    set_vector_vertex( 0, 4, 1, 0, 0 );
    set_vector_vertex( 0, 5, 0, 0, 0 );
}

void set_bullet_sprites( void ) {
    set_sprite_bitmaps( 0, BULLET1SPRITE, &bullet_sprite[0] );
    set_sprite_bitmaps( 1, BULLET1SPRITE, &bullet_sprite[0] );
    set_sprite_bitmaps( 0, BULLET2SPRITE, &bullet_sprite[0] );
    set_sprite_bitmaps( 1, BULLET2SPRITE, &bullet_sprite[0] );
}

void set_ufo_bullet_sprites( void ) {
    set_sprite_bitmaps( 0, UFOBULLETSPRITE, &ufo_bullet_sprite[0] );
    set_sprite_bitmaps( 1, UFOBULLETSPRITE, &ufo_bullet_sprite[0] );
}

void set_tilemap( void ) {
    unsigned char i, x, y, colour;

    (void)tilemap_scrollwrapclear( LOWER_LAYER, TM_CLEAR );
    (void)tilemap_scrollwrapclear( UPPER_LAYER, TM_CLEAR );

    // SET JUPITER SATURN COMET
    set_tilemap_bitmap32x32( LOWER_LAYER, 1, &tilemap_bitmap_back[ 0 ] );
    set_tilemap_bitmap32x32( LOWER_LAYER, 5, &tilemap_bitmap_back[ 1024 ] );
    set_tilemap_bitmap32x32( LOWER_LAYER, 9, &tilemap_bitmap_back[ 2048 ] );

    // PLACE JUPITER, SATURN, COMET
    set_tilemap_tile32x32( LOWER_LAYER, 8, 8, 1 );
    set_tilemap_tile32x32( LOWER_LAYER, 33, 23, 5 );
    set_tilemap_tile32x32( LOWER_LAYER, 21,15, 9 );

    // SET AND PLACE ROCKET SHIPS
    for( unsigned char tile_number = 0; tile_number < 4; tile_number++ ) {
        set_tilemap_bitmap( UPPER_LAYER, tile_number + 1, &tilemap_bitmap_front[ tile_number * 256 ] );
    }
    for( i = 0; i < 4; i++ ) {
        x = rng( 18 ) + ( x&1 ? 21 : 1 );
        y = rng( 7 ) + i*7 + 1;
        set_tilemap_tile32x32( UPPER_LAYER, x, y, 1 );
    }
}

// CYCLE THROUGH NONE BLACK COLOURS
unsigned char swizzle( unsigned char colour ) {
    colour = ( ( colour & 1 ) << 5 ) +
                ( ( colour & 2 ) << 2 ) +
                ( ( colour & 4 ) >> 1 ) +
                ( ( colour & 8 ) << 1 ) +
                ( ( colour & 16 ) >> 4 ) +
                ( ( colour & 32 )  >> 3 );
    return( colour );
}

unsigned char next_colour( unsigned char colour_cycle, unsigned char position ) {
    if( ( colour_cycle + position ) <= 63 ) {
        return( colour_cycle + position );
    }
    return( colour_cycle + position - 62 );
}

// DRAW GAME OVER IN LARGE MULTICOLOURED LETTERS
unsigned char last_colour = 0;
void game_over( void ) {
    gpu_character_blit( swizzle(last_colour), 16, 116, 'G' + 256, 2, 0 );
    gpu_character_blit( swizzle(next_colour(last_colour,1)), 48, 124, 'A' + 256, 2, 0 );
    gpu_character_blit( swizzle(next_colour(last_colour,2)), 80, 116, 'M' + 256, 2, 0 );
    gpu_character_blit( swizzle(next_colour(last_colour,3)), 112, 124, 'E' + 256, 2, 0 );
    gpu_character_blit( swizzle(next_colour(last_colour,4)), 176, 116, 'O' + 256, 2, 0 );
    gpu_character_blit( swizzle(next_colour(last_colour,5)), 208, 124, 'V' + 256, 2, 0 );
    gpu_character_blit( swizzle(next_colour(last_colour,6)), 240, 116, 'E' + 256, 2, 0 );
    gpu_character_blit( swizzle(next_colour(last_colour,7)), 272, 124, 'R' + 256, 2, 0 );
    last_colour = ( last_colour == 63 ) ? 1 : last_colour + 1;
}

// DRAW A RISC-V LOGO AT THE TOP LEFT OF THE SCREEN
void risc_ice_v_logo( void ) {
    // DISPLAY GALAXY BITMAP
    gpu_pixelblock7( 0, 0, 320, 240, TRANSPARENT, galaxybitmap );

    gpu_rectangle( ORANGE, 0, 0, 100, 100 );
    gpu_triangle( WHITE, 100, 33, 100, 100, 50, 100 );
    gpu_triangle( DKBLUE, 100, 50, 100, 100, 66, 100 );
    gpu_rectangle( DKBLUE, 0, 0, 33, 50 );
    gpu_circle( WHITE, 25, 25, 26, 0xff, 1 );
    gpu_rectangle( WHITE, 0, 0, 25, 12 );
    gpu_circle( DKBLUE, 25, 25, 12, 0xff, 1 );
    gpu_triangle( WHITE, 0, 33, 67, 100, 0, 100 );
    gpu_triangle( DKBLUE, 0, 50, 50, 100, 0, 100 );
    gpu_rectangle( DKBLUE, 0, 12, 25, 37 );
    gpu_rectangle( DKBLUE, 0, 37, 8, 100 );

}

// DRAW FULL OR ERASE END OF FUEL AND SHIELD BARS
void drawfuel( unsigned char fullbar ) {

    if( fullbar ) {
        gpu_rectangle( RED, 62, 216, 319, 223 );
        gpu_printf( RED, 22, 216, NORMAL, 0, 0, "FUEL:" );
    }
    gpu_character_blit( RED, 63 + ( fuel >> 2 ), 216, 219, 0, 0 );
    gpu_character_blit( WHITE, 62 + ( fuel >> 2 ), 216, 30, 0, 0 );
}
void drawshield( unsigned char fullbar ) {
    if( fullbar ) {
        gpu_rectangle( BLUE, 62, 224, 319, 231 );
        gpu_printf( BLUE, 6, 224, NORMAL, 0, 0, "SHIELD:" );
    }
    gpu_character_blit( BLUE, 63 + shield, 224, 219, 0, 0 );
    gpu_character_blit( WHITE, 62 + shield, 224, 30, 0, 2 );
}

void setup_game() {
    program_background();
    // CLEAR ALL SPRITES
    for( unsigned char sprite_number = 0; sprite_number < 32; sprite_number++ ) {
        if( sprite_number < MAXASTEROIDS ) {
            asteroid_active[sprite_number] = 0; asteroid_direction[sprite_number] = 0;
        }
        set_sprite( ( sprite_number > 15 ) ? 1 : 0, ( sprite_number > 15 ) ? sprite_number - 16 : sprite_number, 0, 0, 0, 0, 0 );
    }

    // DROP THE BITMAP TO JUST ABOVE THE BACKGROUND AND PAWSv2 COLOUR PALETTE
    screen_mode( 2, MODE_COLOUR );

    // CLEAR and SET THE BACKGROUND
    gpu_cs();
    risc_ice_v_logo();
    set_tilemap();

    tpu_cs();
    set_asteroid_sprites();
    set_ship_vector();
    set_bullet_sprites();
    set_ufo_bullet_sprites();

    set_sprite_bitmaps( 0, SHIPSPRITE, &ship_sprite_lower[ 0 ] );
    set_sprite_bitmaps( 1, SHIPSPRITE, &ship_sprite_upper[ 0 ] );

    lives = 0; score = 0;
    fuel = 1000; shield = 250;
    drawfuel(1); drawshield(1);

    shipx = 312; shipy = 232; shipdirection = 0; resetship = 0; shipexplode = 0; shipmove = 0; shipshield = 0; bulletdirection[0] = 0; bulletdirection[1] = 0;
    last_fire = 0;

    counter = 0;
}

unsigned char find_asteroid_space( void ) {
    unsigned char asteroid_space = 0xff, spaces_free = 0;

    for( unsigned char asteroid_number = 0; asteroid_number < MAXASTEROIDS; asteroid_number++ ) {
        asteroid_space = ( asteroid_active[asteroid_number] == 0 ) ? asteroid_number : asteroid_space;
        spaces_free += ( asteroid_active[asteroid_number] == 0 ) ? 1 : 0;
    }
    return( ( spaces_free == 1 ) ? 0xff : asteroid_space );
}

void move_asteroids( void ) {
    while(1) {
        await_vblank();
        set_timer1khz( 4, 1 );

        for( unsigned char asteroid_number = 0; asteroid_number < MAXASTEROIDS; asteroid_number++ ) {
            if( ( asteroid_active[asteroid_number] != 0 ) && ( asteroid_active[asteroid_number] < 3 ) ) {
                update_sprite( ASN( asteroid_number ), asteroid_directions[ asteroid_direction[asteroid_number] ] );
            }

            // UFO
            if( asteroid_active[asteroid_number] == 3 ) {
                set_sprite_attribute( ASN( asteroid_number ), SPRITE_TILE, 6 );
                update_sprite( ASN( asteroid_number ), ufo_directions[ufo_leftright + ( level > 2 ? 2 : 0 )] );
                if( get_sprite_attribute( ASN( asteroid_number), 0 ) == 0 ) {
                    // UFO OFF SCREEN
                    asteroid_active[asteroid_number] = 0;
                    ufo_sprite_number = 0xff;
                }
            }

            // EXPLOSION - STATIC and countdown
            if( asteroid_active[asteroid_number] > 5 )
                asteroid_active[asteroid_number]--;

            if( asteroid_active[asteroid_number] == 5 ) {
                asteroid_active[asteroid_number] = 0;
                set_sprite( ASN( asteroid_number ), 0, 0, 0, 0, 0 );
            }
        }
        wait_timer1khz( 1 );
    }
}

unsigned short count_asteroids( void ) {
    short number_of_asteroids = 0;

    for( unsigned char asteroid_number = 0; asteroid_number < MAXASTEROIDS; asteroid_number++ ) {
        if( ( asteroid_active[asteroid_number] == 1 ) || ( asteroid_active[asteroid_number] == 2 ) ) {
            number_of_asteroids++;
        }
    }

    return( number_of_asteroids );
}

void draw_ship( void ) {
    set_sprite( 0, SHIPSPRITE, shipexplode ? 0 : 1, shipx, shipy, ( shipdirection & 3 ) + ( shipmove ? 4 : 0 ), ROTATE0 + ( ( shipdirection & 12 ) >> 2 ) );

    // ENABLE SHIELD IF RESET OR SHIELD ACTIVE, ENABLE EXPLOSION IF NEEDED
    set_sprite( 1, SHIPSPRITE, resetship | shipshield | shipexplode ? 1 : 0, shipx - ( resetship | shipshield ? 8 : 0 ), shipy - ( resetship | shipshield ? 8 : 0 ), ( counter & 3 ) + ( shipexplode ? 4 : 0 ), resetship | shipshield ? SPRITE_DOUBLE : 0 );
}

void move_ship() {
    switch( shipdirection ) {
        case 0:
            shipy = ( shipy > 0 ) ? shipy - 1 : 464;
            break;
        case 1:
            shipx = ( shipx < 624 ) ? shipx + ( counter & 1 ) : 0;
            shipy = ( shipy > 0 ) ? shipy - 1 : 464;
            break;
        case 2:
            shipx = ( shipx < 624 ) ? shipx + 1 : 0;
            shipy = ( shipy > 0 ) ? shipy - 1 : 464;
            break;
        case 3:
            shipx = ( shipx < 624 ) ? shipx + 1 : 0;
            shipy = ( shipy > 0 ) ? shipy - ( counter & 1 ) : 464;
            break;
        case 4:
            shipx = ( shipx < 624 ) ? shipx + 1 : 0;
            break;
        case 5:
            shipx = ( shipx < 624 ) ? shipx + 1 : 0;
            shipy = ( shipy < 464 ) ? shipy + ( counter & 1 ) : 0;
            break;
        case 6:
            shipx = ( shipx < 624 ) ? shipx + 1 : 0;
            shipy = ( shipy < 464 ) ? shipy + 1 : 0;
            break;
        case 7:
            shipx = ( shipx < 624 ) ? shipx + ( counter & 1 ) : 0;
            shipy = ( shipy < 464 ) ? shipy + 1 : 0;
            break;
        case 8:
            shipy = ( shipy < 464 ) ? shipy + 1 : 0;
            break;
        case 9:
            shipx = ( shipx > 0 ) ? shipx - ( counter & 1 ) : 624;
            shipy = ( shipy < 464 ) ? shipy + 1 : 0;
            break;
        case 10:
            shipx = ( shipx > 0 ) ? shipx - 1 : 624;
            shipy = ( shipy < 464 ) ? shipy + 1 : 0;
            break;
        case 11:
            shipx = ( shipx > 0 ) ? shipx - 1 : 624;
            shipy = ( shipy < 464 ) ? shipy + ( counter & 1 ) : 0;
            break;
        case 12:
            shipx = ( shipx > 0 ) ? shipx - 1 : 624;
            break;
        case 13:
            shipx = ( shipx > 0 ) ? shipx - 1 : 624;
            shipy = ( shipy > 0 ) ? shipy - ( counter & 1 ) : 464;
            break;
        case 14:
            shipx = ( shipx > 0 ) ? shipx - 1 : 624;
            shipy = ( shipy > 0 ) ? shipy - 1 : 464;
            break;
        case 15:
            shipx = ( shipx > 0 ) ? shipx - ( counter & 1 ) : 624;
            shipy = ( shipy > 0 ) ? shipy - 1 : 464;
            break;
    }
}

void draw_score( void ) {
    tpu_printf_centre( 1, TRANSPARENT, ( lives > 0 ) ? WHITE : GREY50, BOLD, "Score %5d", score );
}

void draw_lives( void ) {
    for( unsigned short i = 0; i < lives; i++ ) {
        draw_vector_block( 0, WHITE, 304, 16 + i * 16, 0, ROTATE0 + i );
    }
}

void fire_bullet( void ) {
    short bulletx, bullety, bulletnumber;

    bulletnumber = get_sprite_attribute( 0, BULLET1SPRITE, 0 ) ? 1 : 0;

    bulletdirection[bulletnumber] = shipdirection;
    switch( bulletdirection[bulletnumber] ) {
        case 0:
            bulletx = shipx; bullety = shipy - 10;
            break;
        case 1:
            bulletx = shipx + 5; bullety = shipy - 10;
            break;
        case 2:
            bulletx = shipx + 8; bullety = shipy - 10;
            break;
        case 3:
            bulletx = shipx + 8; bullety = shipy - 6;
            break;
        case 4:
            bulletx = shipx + 10; bullety = shipy;
            break;
        case 5:
            bulletx = shipx + 10; bullety = shipy + 6;
            break;
        case 6:
            bulletx = shipx + 10; bullety = shipy + 10;
            break;
        case 7:
            bulletx = shipx + 5; bullety = shipy + 10;
            break;
        case 8:
            bulletx = shipx; bullety = shipy + 10;
            break;
        case 9:
            bulletx = shipx - 5; bullety = shipy + 10;
            break;
        case 10:
            bulletx = shipx - 10; bullety = shipy + 10;
            break;
        case 11:
            bulletx = shipx - 10; bullety = shipy + 6;
            break;
        case 12:
            bulletx = shipx - 10; bullety = shipy;
            break;
        case 13:
            bulletx = shipx - 10; bullety = shipy - 6;
            break;
        case 14:
            bulletx = shipx - 10; bullety = shipy - 10;
            break;
        case 15:
            bulletx = shipx - 5; bullety = shipy - 10;
            break;
    }

    switch( bulletnumber ) {
        case 0:
            set_sprite( 0, BULLET1SPRITE, 1, bulletx, bullety, 2, 0);
            set_sprite( 1, BULLET1SPRITE, 1, bulletx, bullety, 0, 0);
            break;
        case 1:
            set_sprite( 0, BULLET2SPRITE, 1, bulletx, bullety, 2, 0);
            set_sprite( 1, BULLET2SPRITE, 1, bulletx, bullety, 0, 0);
            break;
    }
    beep( 2, 4, 61, 128 );

    last_fire = 25;
}

void update_bullet( void ) {
    // PLAYER BULLETS
    update_sprite( 0, BULLET1SPRITE, bullet_directions[ bulletdirection[0] ] );
    update_sprite( 1, BULLET1SPRITE, bullet_directions[ bulletdirection[0] ] );
    update_sprite( 0, BULLET2SPRITE, bullet_directions[ bulletdirection[1] ] );
    update_sprite( 1, BULLET2SPRITE, bullet_directions[ bulletdirection[1] ] );

    // UFO BULLET
    update_sprite( 0, UFOBULLETSPRITE, bullet_directions[ ufo_bullet_direction ] );
    update_sprite( 1, UFOBULLETSPRITE, bullet_directions[ ufo_bullet_direction ] );
}

void beepboop( void ) {
    if( last_timer != get_timer1hz( 0 ) ) {
        draw_score();

        last_timer = get_timer1hz( 0 );

        (void)tilemap_scrollwrapclear( LOWER_LAYER, TM_WRAP_LEFT );
        (void)tilemap_scrollwrapclear( UPPER_LAYER, TM_WRAP_RIGHT );

        switch( last_timer & 3 ) {
            case 0:
                if( lives == 0 ) {
                    tpu_print_centre( 6, TRANSPARENT, DKBLUE, 0, "Controls: Fire 1 - FIRE" );
                    tpu_print_centre( 52, TRANSPARENT, BLUE, 1, "Welcome to Risc-ICE-V Asteroids" );
                } else {
                    if( ufo_sprite_number != 0xff ) {
                        beep( 1, 3, 63, 32 );
                    } else {
                        beep( 1, 0, 1, 500 );
                    }
                }
                break;

            case 1:
                if( lives == 0 ) {
                    tpu_print_centre( 6, TRANSPARENT, PURPLE, 0, "Controls: Fire 2 - SHIELD" );
                    tpu_print_centre( 52, TRANSPARENT, CYAN, 0, "By @robng15 (Twitter) from Whitebridge, Scotland" );
                } else {
                    if( ufo_sprite_number != 0xff ) {
                        beep( 1, 3, 63, 32 );
                    }
                }
                break;

            case 2:
                if( lives == 0 ) {
                    tpu_print_centre( 6, TRANSPARENT, ORANGE, 0, "Controls: Left / Right - TURN" );
                    tpu_print_centre( 52, TRANSPARENT, YELLOW, 0, "Press UP to start" );
                } else {
                    if( ufo_sprite_number != 0xff ) {
                        beep( 1, 3, 63, 32 );
                    } else {
                        beep( 1, 0, 2, 500 );
                    }
                }
                break;

            case 3:
                // MOVE TILEMAP UP
                if( lives == 0 ) {
                    tpu_print_centre( 6, TRANSPARENT, DKRED, 0, "Controls: UP - MOVE" );
                    tpu_print_centre( 52, TRANSPARENT, RED, 0, "Written in Silice by @sylefeb" );
                } else {
                    if( ufo_sprite_number != 0xff ) {
                        beep( 1, 3, 63, 32 );
                    }
                }
                (void)tilemap_scrollwrapclear( LOWER_LAYER, TM_WRAP_DOWN );
                (void)tilemap_scrollwrapclear( UPPER_LAYER, TM_WRAP_UP );
                break;
        }
        if( lives == 0 ) { game_over(); }
    }
}

void spawn_asteroid( unsigned char asteroid_type, short xc, short yc ) {
    unsigned char potentialnumber;

    potentialnumber = find_asteroid_space();
    if( potentialnumber != 0xff ) {
        asteroid_active[ potentialnumber ] = asteroid_type;
        asteroid_direction[ potentialnumber ] = rng( ( asteroid_type == 2 ) ? 4 : 8 );

        set_sprite( ASN( potentialnumber ), 1, xc + rng(16) - 8, yc + rng(16) - 8, rng( 6 ), ( asteroid_type == 2 ? SPRITE_DOUBLE : 0 ) + rng( 8 ) );
    }
}

void check_ufo_bullet_hit( void ) {
    unsigned char asteroid_hit = 0xff, spawnextra;
    short x, y;

    if( ( ( get_sprite_collision( 0, UFOBULLETSPRITE ) & ASTEROIDCOLLISION ) != 0 ) || ( ( get_sprite_collision( 1, UFOBULLETSPRITE ) & ASTEROIDCOLLISION ) != 0 ) ) {
        beep( 2, 4, 8, 500 );
        for( unsigned char asteroid_number = 0; asteroid_number < MAXASTEROIDS; asteroid_number++ ) {
            if( get_sprite_collision( ASN( asteroid_number ) ) & UFOBULLETCOLLISION ) {
                asteroid_hit = asteroid_number;
            }
        }

        if( ( asteroid_hit != 0xff ) && ( asteroid_active[asteroid_hit] < 3 ) ) {
            // DELETE BULLET
            set_sprite_attribute( 0, UFOBULLETSPRITE, SPRITE_ACTIVE, 0 );
            set_sprite_attribute( 1, UFOBULLETSPRITE, SPRITE_ACTIVE, 0 );

            x = get_sprite_attribute( ASN( asteroid_hit ), SPRITE_X );
            y = get_sprite_attribute( ASN( asteroid_hit ), SPRITE_Y );

            // SPAWN NEW ASTEROIDS
            if( asteroid_active[asteroid_hit] == 2 ) {
                spawnextra = 1 + ( ( level < 2 ) ? level : 2 ) + ( ( level > 2 ) ? rng( 2 ) : 0 );
                for( int i=0; i < spawnextra; i++ ) {
                    spawn_asteroid( 1, x, y );
                }
            }

            // SET EXPLOSION TILE
            set_sprite_attribute( ASN( asteroid_hit ), SPRITE_TILE, 7 );
            asteroid_active[asteroid_hit] = 32;
        }
    }
}

void check_hit( void ) {
    unsigned char asteroid_hit, spawnextra;
    short x, y;

    for( short i = 0; i < 2; i++ ) {
        asteroid_hit = 0xff;
        if( ( ( get_sprite_collision( 0, BULLET2SPRITE + i ) & ASTEROIDCOLLISION ) != 0 ) || ( ( get_sprite_collision( 1, BULLET2SPRITE + i ) & ASTEROIDCOLLISION ) != 0 ) ) {
            beep( 2, 4, 8, 500 );
            for( unsigned char asteroid_number = 0; asteroid_number < MAXASTEROIDS; asteroid_number++ ) {
                if( get_sprite_collision( ASN( asteroid_number ) ) & ( i ? BULLET1COLLISION : BULLET2COLLISION ) ) {
                    asteroid_hit = asteroid_number;
                }
            }

            if( asteroid_hit != 0xff ) {
                switch( asteroid_active[asteroid_hit] ) {
                    case 0:
                        break;

                    case 1:
                    case 2:
                        // HIT ASTEROID - DELETE BULLET
                        set_sprite_attribute( 0, BULLET2SPRITE + i, SPRITE_ACTIVE, 0 );
                        set_sprite_attribute( 1, BULLET2SPRITE + i, SPRITE_ACTIVE, 0 );

                        score += ( 3 - asteroid_active[asteroid_hit] );

                        x = get_sprite_attribute( ASN( asteroid_hit ), SPRITE_X );
                        y = get_sprite_attribute( ASN( asteroid_hit ), SPRITE_Y );

                        // SPAWN NEW ASTEROIDS
                        if( asteroid_active[asteroid_hit] == 2 ) {
                            spawnextra = 1 + ( ( level < 2 ) ? level : 2 ) + ( ( level > 2 ) ? rng( 2 ) : 0 );
                            for( int i=0; i < spawnextra; i++ ) {
                                spawn_asteroid( 1, x, y );
                            }
                        }

                        set_sprite( ASN( asteroid_hit ), 1, x, y, 7, get_sprite_attribute( ASN( asteroid_hit ), SPRITE_ACTION ) );
                        asteroid_active[asteroid_hit] = 32;
                        break;

                    case 3:
                        // UFO
                        score += ( level < 2 ) ? 10 : 20;
                        // DELETE BULLET
                        set_sprite_attribute( 0, BULLET2SPRITE + i, SPRITE_ACTIVE, 0 );
                        set_sprite_attribute( 1, BULLET2SPRITE + i, SPRITE_ACTIVE, 0 );

                        x = get_sprite_attribute( ASN( asteroid_hit ), SPRITE_X );
                        y = get_sprite_attribute( ASN( asteroid_hit ), SPRITE_Y );
                        set_sprite_attribute( ASN( asteroid_hit ), SPRITE_TILE, 7 );
                        ufo_sprite_number = 0xff;
                        asteroid_active[asteroid_hit] = 32;
                        // AVOID BONUS FUEL AND SHIELD
                        fuel += 10 + rng( ( level < 2 ) ? 10 : 40 );
                        fuel = ( fuel > 1000 ) ? 1000 : fuel;
                        shield += 5 + rng( ( level < 2 ) ? 5 : 10 );
                        shield = ( shield > 250 ) ? 250 : shield;
                        drawfuel(1);
                        drawshield(1);
                        break;

                    default:
                        // EXPLOSION
                        break;

                }
            }
        }
    }
}

void check_crash( void ) {
    if( ( ( ( get_sprite_collision( 0, SHIPSPRITE ) | get_sprite_collision( 1, SHIPSPRITE ) ) & ( ASTEROIDCOLLISION | UFOBULLETSPRITE ) ) )
        || ( get_sprite_layer_collision( 0, SHIPSPRITE ) & 1 ) ) {
        if( ( get_sprite_collision( 0, UFOBULLETSPRITE ) | get_sprite_collision( 1, UFOBULLETSPRITE ) ) & SHIPCOLLISION ) {
            // DELETE UFO BULLET
            set_sprite_attribute( 0, UFOBULLETSPRITE, SPRITE_ACTIVE, 0 );
            set_sprite_attribute( 1, UFOBULLETSPRITE, SPRITE_ACTIVE, 0 );
        }
        beep( 2, 4, 1, 1000 );
        shipexplode = 1;
        set_sprite_attribute( 0, UFOBULLETSPRITE, SPRITE_TILE, 0 );
        set_sprite_attribute( 1, UFOBULLETSPRITE, SPRITE_TILE, 1 );
        resetship = 75;
    }
}

// MAIN GAME LOOP STARTS HERE
void smt_thread( void ) {
    // SETUP STACKPOINTER FOR THE SMT THREAD
    asm volatile ("li sp ,0x4000");

    while(1) move_asteroids();
}

int main( void ) {
    unsigned char potentialnumber = 0;
    short ufo_x = 0, ufo_y = 0, potentialx = 0, potentialy = 0;
    unsigned short placeAsteroids = 4, asteroid_number = 0;

    // INITIALISE ALL VARIABLES AND START THE ASTEROID MOVING THREAD
    setup_game();
    SMTSTART( (unsigned int )smt_thread );

    while(1) {
        last_fire = ( last_fire > 0 ) ? last_fire - 1 : 0;
        counter++;

        // PLACE NEW LARGE ASTEROIDS
        if( ( placeAsteroids > 0 ) && ( ( counter & 63 ) == 0 ) ) {
            potentialnumber = find_asteroid_space();
            if( potentialnumber != 0xff ) {
                switch( rng(4) ) {
                    case 0:
                        potentialx = -31;
                        potentialy = rng(480);
                        break;
                    case 1:
                        potentialx = -639;
                        potentialy = rng(480);
                        break;
                    case 2:
                        potentialx = rng(640);
                        potentialy = -31;
                        break;
                    case 3:
                        potentialx = rng(640);
                        potentialy = 479;
                        break;
                }
                spawn_asteroid( 2, potentialx, potentialy );
            }
            placeAsteroids--;
        }

        // NEW LEVEL NEEDED
        if( !count_asteroids() && !placeAsteroids ) {
            level++;
            placeAsteroids = 4 + ( ( level < 4 ) ? level : 4 );
        }

        // AWAIT VBLANK and SET DELAY
        await_vblank();
        set_timer1khz( 4, 0 );

        // BEEP / BOOP
        beepboop();

        if( ( rng( 512 ) == 1 ) && ( ufo_sprite_number == 0xff ) && ( get_sprite_attribute( 0, 10, 0 ) == 0 ) ) {
            // START UFO
            ufo_sprite_number = find_asteroid_space();

            if( ufo_sprite_number != 0xff ) {
                // ROOM for UFO
                do {
                    ufo_y = 32 + rng(  384 );
                } while( ( ufo_y >= shipy - 64 ) && ( ufo_y <= shipy + 64 ) );

                ufo_leftright = rng( 2 );
                set_sprite( ASN( ufo_sprite_number ), 1, ( ufo_leftright == 1 ) ? 639 : ( level < 2 ) ? -31 : -15, ufo_y, 6, ( level < 2 ) ? SPRITE_DOUBLE : 0 );
                asteroid_active[ ufo_sprite_number ] = 3;
            }
        }

        if( ( rng( ( level > 3 ) ? 64 : 128 ) == 1 ) && ( get_sprite_attribute( 0, UFOBULLETSPRITE, 0 ) == 0 ) && ( ufo_sprite_number != 0xff ) && ( ( level != 0 ) || ( lives == 0 ) ) ) {
            // START UFO BULLET
            beep( 2, 4, 63, 32 );

            ufo_x = get_sprite_attribute( ASN( ufo_sprite_number ), SPRITE_X ) + ( ( level < 2 ) ? 16 : 8 );
            ufo_y = get_sprite_attribute( ASN( ufo_sprite_number ), SPRITE_Y );
            if( ufo_y > shipy ) {
                ufo_y -= 10;
            } else {
                ufo_y += ( ( level < 2 ) ? 20 : 10 );
            }
            ufo_bullet_direction = ( ufo_x > shipx ) ? 12 : 4;

            switch( ufo_bullet_direction ) {
                case 4:
                    ufo_bullet_direction += ( ufo_y > shipy ) ? -2 : 2;
                    break;

                case 12:
                    ufo_bullet_direction += ( ufo_y > shipy ) ? 2 : -2;
                    break;

                default:
                    break;
            }
            set_sprite( 0, UFOBULLETSPRITE, 1, ufo_x, ufo_y, 0, 0 );
            set_sprite( 1, UFOBULLETSPRITE, 1, ufo_x, ufo_y, 1, 0 );
        }

        if( ( lives > 0 ) && ( resetship == 0) ) {
            // GAME IN ACTION

            // EVERY 4th CYCLE
            if( ( counter & 3 ) == 0 ) {
                // TURN LEFT
                if( ( get_buttons() & 32 ) != 0 )
                    shipdirection = ( shipdirection == 0 ) ? 15 : shipdirection - 1;
                // TURN RIGHT
                if( ( get_buttons() & 64 ) != 0 )
                    shipdirection = ( shipdirection == 15 ) ? 0 : shipdirection + 1;
            }

            // EVERY CYCLE
            // FIRE?
            if( ( last_fire == 0 ) && ( ( get_sprite_attribute( 0, BULLET1SPRITE, 0 ) == 0 ) || ( get_sprite_attribute( 0, BULLET2SPRITE, 0 ) == 0 ) )  && ( get_buttons() & 2 ) != 0 )
                fire_bullet();

            // MOVE SHIP, IF FUEL LEFT
            if( ( ( get_buttons() & 8 ) != 0 ) && ( fuel > 0 ) ) {
                shipmove = 1;
                move_ship();
                fuel--;
                drawfuel(0);
            } else {
                shipmove = 0;
            }

            // CHECK IF CRASHED ASTEROID -> SHIP, IF SHIELD BUTTON NOT HELD DOWN
            if( ( ( get_buttons() & 4 ) != 0 ) && ( shield > 0 ) ) {
                shipshield = 1;
                shield--;
                drawshield(0);
            } else {
                shipshield = 0;
                check_crash();
            }
        } else {
            // GAME OVER OR EXPLODING SHIP
            // SEE IF NEW GAME
            if( ( lives == 0 ) && ( ( get_buttons() & 8 ) != 0 ) ) {
                // CLEAR ASTEROIDS
                for( asteroid_number = 0; asteroid_number < MAXASTEROIDS; asteroid_number++ ) {
                    asteroid_active[asteroid_number] = 0; asteroid_direction[asteroid_number] = 0;
                    set_sprite_attribute( ASN(asteroid_number), SPRITE_ACTIVE, 0 );
                }

                // CLEAR BULLETS
                set_sprite_attribute( 0, UFOBULLETSPRITE, SPRITE_ACTIVE, 0 );
                set_sprite_attribute( 1, UFOBULLETSPRITE, SPRITE_ACTIVE, 0 );
                set_sprite_attribute( 0, BULLET1SPRITE, SPRITE_ACTIVE, 0 );
                set_sprite_attribute( 1, BULLET1SPRITE, SPRITE_ACTIVE, 0 );
                set_sprite_attribute( 0, BULLET2SPRITE, SPRITE_ACTIVE, 0 );
                set_sprite_attribute( 1, BULLET2SPRITE, SPRITE_ACTIVE, 0 );

                // CLEAR SCREEN
                gpu_cs(); tpu_cs();

                counter = 0;

                lives = 4; score = 0; level = 0;

                shield = 250; fuel = 1000;
                drawfuel(1); drawshield(1);

                shipx = 312; shipy = 232; shipdirection = 0; resetship = 16; shipmove = 0; shipshield = 0; bulletdirection[0] = 0; bulletdirection[1] = 0;
                placeAsteroids = 4;
                ufo_sprite_number = 0xff; ufo_leftright = 0;
                draw_lives();
                set_asteroid_sprites();
                shipexplode = 0;
                set_bullet_sprites();
                set_ufo_bullet_sprites();
            }

            if( ( ( resetship >= 1 ) && ( resetship <= 16 ) ) || ( lives == 0 ) ) {
                if( ( resetship >= 1 ) && ( resetship <= 16 ) ) {
                    if( !( ( get_sprite_collision( 0, SHIPSPRITE ) | get_sprite_collision( 1, SHIPSPRITE ) ) & ASTEROIDCOLLISION ) ) {
                        resetship--;
                        if( resetship == 0 ) {
                            gpu_cs();
                            lives--;
                            draw_lives();
                            fuel = 1000;
                            drawfuel(1); drawshield(1);
                        }

                        if( lives == 0 ) {
                            placeAsteroids = 4;
                            risc_ice_v_logo();
                        }
                    }
                }
            }

            if( resetship > 16 ) {
                // EXPLODING SHIP
                shipexplode = 1; shipshield = 0; shipmove = 0;

                resetship--;
                if( resetship == 16 ) {
                    shipexplode = 0;
                    shipx = 312; shipy = 232; shipdirection = 0;
                }
            }
        }

        // DRAW THE SHIP
        draw_ship();

        // UPDATE BULLET
        update_bullet();

        // CHECK IF HIT BULLET -> ASTEROID
        check_hit();
        check_ufo_bullet_hit();

        wait_timer1khz( 0 );
    }
}
