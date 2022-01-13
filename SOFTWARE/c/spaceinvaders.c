// PAWS implementation of http://nicktasios.nl/posts/space-invaders-from-scratch-part-1.html

#include "library/PAWSlibrary.h"
#include <stdio.h>

// INCLUDE GRAPHICS
#include "graphics/spaceinvader-graphics.h"

// STORAGE FOR CONVERTING ALIENS TO COLOUR BLITTER OBJECTS
unsigned char colour_blitter_bitmap[ 256 ];

#define MAXALIENS 55
struct Alien {
    short x, y;
    short type, animation_count;
};
struct Alien Aliens[ MAXALIENS ];

struct Swarm {
    short leftcolumn, rightcolumn, toprow, bottomrow, bottompixel;
    short row, column, direction, newdirection;
    short lastbombtimer;
    short count;
};
struct Swarm AlienSwarm;

#define UFOONSCREEN 1
#define UFOEXPLODE 2
struct Ufo {
    short active, score;
    short counter;
    short x, direction;
    short lastufo;
    short pitchcount;
};
struct Ufo UFO;

// CURRENT FRAMEBUFFER
unsigned short framebuffer = 0;

// PLAYER
#define SHIPPLAY 0
#define SHIPRESET 1
#define SHIPEXPLODE 2
#define SHIPEXPLODE2 3
struct Player {
    int score;
    short x, y;
    short level, life;
    short state, counter;
};
struct Player Ship;

char moonscape_back[][42] = {
    "..........................................",
    "..........................................",
    ".12.......................................",
    "1882..12..............................12..",
    "888821882............................1882.",
    "888888888888888888888888888888888888888888",
    "888888888888888888888888888888888888888888",
    "888888888888888888888888888888888888888888",
    "888888888888888888888888888888888888888888",
    "888888888888888888888888888888888888888888",
    "888888888888888888888888888888888888888888",
    "888888888888888888888888888888888888888888",
    "888888888888888888888888888888888888888888",
    "888888888888888888888888888888888888888888",
};

char moonscape_front[][42] = {
    ".....................12...................",
    "....................1772..................",
    ".........12......1217777212......12......1",
    "........1772..1217777777777212..1772....17",
    "2......177772177777777777777772177772..177",
    "72....17777777734........56777777777721777",
    "772..1777777734............567777777777777",
    "7772177777734................5677777777777",
    "77777777734....................56777777777",
    "777777734........................567777777",
    "7777734............................5677777",
    "77734................................56777",
    "734....................................567",
    "4........................................5"
};

// HELPER TO REMOVE ALL/SOME SPRITES
void remove_sprites( short start_sprite ) {
    for( short i = start_sprite; i < 16; i++ ) {
        set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, 0 );
    }
}

// HELPER FOR PLACING A 4 TILE 32 x 32 TILE TO THE TILEMAPS
void set_tilemap_32x32tile( unsigned char tm_layer, short x, short y, unsigned char start_tile ) {
    set_tilemap_tile( tm_layer, x, y, start_tile, 0 );
    set_tilemap_tile( tm_layer, x, y + 1, start_tile + 1, 0 );
    set_tilemap_tile( tm_layer, x + 1, y, start_tile + 2, 0 );
    set_tilemap_tile( tm_layer, x + 1, y + 1, start_tile + 3, 0 );
}

// HELPER FOR PLACING A 2 TILE 16 x 32 TILE TO THE TILEMAPS with REFLECTION for right hand side
void set_tilemap_16x32tile( unsigned char tm_layer, short x, short y, unsigned char start_tile ) {
    set_tilemap_tile( tm_layer, x, y, start_tile, 0 );
    set_tilemap_tile( tm_layer, x, y + 1, start_tile + 1, 0 );
    set_tilemap_tile( tm_layer, x + 1, y, start_tile, REFLECT_X );
    set_tilemap_tile( tm_layer, x + 1, y + 1, start_tile + 1, REFLECT_X );
}

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

// SETUP THE BACKDROP - MOUNTAINS, LANDING AREA, ROCKETS AND PLANETS
void draw_moonscape( void ) {
    // PLACE MOONSCAPE ON THE TILEMAPS
    for( short y = 0; y < 13; y++ ) {
        for( short x = 0; x < 42; x++ ) {
            switch( moonscape_back[y][x] ) {
                case '.':
                    set_tilemap_tile( LOWER_LAYER, x, 18 + y, 0, 0 );
                    break;
                case '1':
                    set_tilemap_tile( LOWER_LAYER, x, 18 + y, 1, 0 );
                    break;
                case '2':
                    set_tilemap_tile( LOWER_LAYER, x, 18 + y, 1, REFLECT_X );
                    break;
                case '8':
                    set_tilemap_tile( LOWER_LAYER, x, 18 + y, 2, 0 );
                    break;
            }
            switch( moonscape_front[y][x] ) {
                case '.':
                    set_tilemap_tile( UPPER_LAYER, x, 18 + y, 0, 0 );
                    break;
                case '1':
                    set_tilemap_tile( UPPER_LAYER, x, 18 + y, 20, 0 );
                    break;
                case '2':
                    set_tilemap_tile( UPPER_LAYER, x, 18 + y, 20, REFLECT_X );
                    break;
                case '3':
                    set_tilemap_tile( UPPER_LAYER, x, 18 + y, 18, 0 );
                    break;
                case '4':
                    set_tilemap_tile( UPPER_LAYER, x, 18 + y, 19, 0 );
                    break;
                case '5':
                    set_tilemap_tile( UPPER_LAYER, x, 18 + y, 19, REFLECT_X );
                    break;
                case '6':
                    set_tilemap_tile( UPPER_LAYER, x, 18 + y, 18, REFLECT_X );
                    break;
                case '7':
                    set_tilemap_tile( UPPER_LAYER, x, 18 + y, 21, 0 );
                    break;
            }
        }
    }

    // PLACE PLANETS AND COMET
    set_tilemap_32x32tile( LOWER_LAYER, 4, 4, 3 );
    set_tilemap_32x32tile( LOWER_LAYER, 36,4, 7 );
    set_tilemap_32x32tile( LOWER_LAYER, 21, 6, 11 );

    // PLACE CRATERS
    set_tilemap_32x32tile( UPPER_LAYER, 15, 25, 25 );
    set_tilemap_32x32tile( UPPER_LAYER, 26, 25, 25 );

    // PLACE LAUNCHED ROCKETS
    set_tilemap_32x32tile( UPPER_LAYER, 3, 18, 14 );
    set_tilemap_32x32tile( UPPER_LAYER, 30, 20, 14 );

    // PLACE GROUNDED ROCKETS using reflection
    set_tilemap_16x32tile( UPPER_LAYER, 17, 23, 12);
    set_tilemap_16x32tile( UPPER_LAYER, 23, 24, 12 );
    set_tilemap_16x32tile( UPPER_LAYER, 20, 25, 12 );
}

void initialise_graphics( void ) {
    unsigned char colour;

    // SET THE BACKGROUND - MULTICOLOURED STARFIELD VIA COPPER
    program_background();
    // CLEAR THE TILEMAPS
    tilemap_scrollwrapclear( LOWER_LAYER, 9 );
    tilemap_scrollwrapclear( UPPER_LAYER, 9 );

    // SET BLITTER OBJECTS - ALIENS, EXPLOSIONS, UFO AND BUNKERS
    for( short i = 0; i < 15; i++ ) {
        set_blitter_bitmap( i + 2, &blitter_bitmaps[ 16 * i ] );
    }

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
                            colour = DKBLUE;
                            break;
                        case 'C':
                            switch( i ) {
                                case 0:
                                    colour = RED;
                                    break;
                                case 1:
                                    colour = DKMAGENTA;
                                    break;
                                case 2:
                                    colour = DKCYAN - 1;
                                    break;
                            }
                            break;
                        case 'Y':
                            colour = YELLOW;
                            break;
                    }
                    colour_blitter_bitmap[ y * 16 + x ] = colour;
                }
            }
            set_colourblitter_bitmap( 3 + i * 3 + j, &colour_blitter_bitmap[ 0 ] );
        }
    }

    // SET SPRITES - 1:0 is player, 1:1 is bullet, 1:2-15 are bombs
    set_sprite_bitmaps( UPPER_LAYER, 0, &player_bitmaps[0] );
    set_sprite_bitmaps( UPPER_LAYER, 1, &bullet_bitmaps[0] );
    for( short i = 2; i < 16; i++ ) {
        set_sprite_bitmaps( UPPER_LAYER, i, &bomb_bitmaps[0] );
    }

    // SET TILEMAP TILES - NUMBERS AND SHIP GRAPHIC + ROCKET + WEDGES FOR MOONSCAPE
    for( short i = 0; i < 2; i++ ) {
        set_tilemap_bitmap( LOWER_LAYER, i + 1, &tilemap_bitmaps_back[ 256 * i ] );
    }
    for( short i = 0; i < 25; i++ ) {
        set_tilemap_bitmap( UPPER_LAYER, i + 1, &tilemap_bitmaps_front[ 256 * i ] );
    }

    // SET JUPITER SATURN COMET CRATER
    set_tilemap_bitmap32x32( LOWER_LAYER, 3, &jupiter_tilemap32x32[ 0 ] );
    set_tilemap_bitmap32x32( LOWER_LAYER, 7, &saturn_tilemap32x32[ 0 ] );
    set_tilemap_bitmap32x32( LOWER_LAYER, 11, &comet_tilemap32x32[ 0 ] );
    set_tilemap_bitmap32x32( UPPER_LAYER, 25, &crater_tilemap32x32[ 0 ] );

    draw_moonscape();
}

void reset_aliens( void ) {
    bitmap_draw( 0 ); gpu_cs();
    bitmap_draw( 1 ); gpu_cs();

    // SET THE ALIENS
    for( short y = 0; y < 5; y++ ) {
        for( short x = 0; x < 11; x++ ) {
            Aliens[ y * 11 + x ].x = 16 * x + 8;
            Aliens[ y * 11 + x ].y = 16 * ( ( Ship.level < 4 ) ? Ship.level : 4 ) + 16 * y + 24;
            switch( y ) {
                case 0:
                    Aliens[ y * 11 + x ].type = 1;
                    break;
                case 1:
                case 2:
                    Aliens[ y * 11 + x ].type = 2;
                    break;
                default:
                    Aliens[ y * 11 + x ].type = 3;
                    break;
            }
            Aliens[ y * 11 + x ].animation_count = 0;
        }
    }

    AlienSwarm.leftcolumn = 0;
    AlienSwarm.rightcolumn = 10;
    AlienSwarm.toprow = 0;
    AlienSwarm.bottomrow = 4;
    AlienSwarm.row = 0;
    AlienSwarm.column = 10;
    AlienSwarm.direction = 1;
    AlienSwarm.newdirection = 1;
    AlienSwarm.lastbombtimer = 0;
    AlienSwarm.count = 55;

    // REMOVE THE PLAYER, MISSILE AND BOMBS
    remove_sprites( 0 );

    // DRAW BUNKERS
    for( short i = 0; i < 2; i++ ) {
        bitmap_draw( i );
        for( short j = 0; j < 4; j++ ) {
            gpu_blit( GREEN, 24 + j * 80, 208, 12, 0, 0 );
            gpu_blit( GREEN, 40 + j * 80, 208, 13, 0, 0 );
        }
    }

    bitmap_draw( !framebuffer );
}

void reset_player( void ) {
    Ship.state = SHIPRESET;
    Ship.counter = 32;
    Ship.x = 320 - 12;
    Ship.y = 465;
}

void reset_game( void ) {
    // SET THE PLAYER
    reset_player();
    Ship.score = 0;
    Ship.level = 0;
    Ship.life = 3;

    reset_aliens();
    UFO.active = 0;
    UFO.lastufo = 1000;
}

void trim_aliens( void ) {
    short left = 10, right = 0, top = 4, bottom = 0, pixel = 16;

    // CHECK IF ANY ALIENS LEFT
    if( !AlienSwarm.count ) {
        Ship.level++;
        reset_aliens();
    }

    // TRIM SWARM
    for( short y = 0; y < 5; y++ ) {
        for( short x = 0; x < 11; x++ ) {
            switch( Aliens[ y * 11 + x ].type ) {
                case 0:
                    break;
                case 4:
                    Aliens[ y * 11 + x ].type = 0;
                    AlienSwarm.count--;
                default:
                    left = min( left, x );
                    right = max( right, x );
                    top = min( top, y );
                    bottom = max( bottom, y );
                    pixel =  max( pixel, Aliens[ y * 11 + x ].y + 7 );
            }
        }
    }
    AlienSwarm.leftcolumn = left;
    AlienSwarm.rightcolumn = right;
    AlienSwarm.toprow = top;
    AlienSwarm.bottomrow = bottom;
    AlienSwarm.bottompixel = pixel;
}

void draw_aliens( void ) {
    // DRAW ALIEN SWARM
    for( short y = AlienSwarm.toprow; y <= AlienSwarm.bottomrow; y++ ) {
        for( short x = AlienSwarm.leftcolumn; x <= AlienSwarm.rightcolumn; x++ ) {
            switch( Aliens[ y * 11 + x ].type ) {
                case 0:
                    break;
                case 1:
                case 2:
                case 3:
                    if( Ship.level & 1 ) {
                        gpu_colourblit( Aliens[ y * 11 + x ].x, Aliens[ y * 11 + x ].y, Aliens[ y * 11 + x ].type * 3 +  Aliens[ y * 11 + x ].animation_count, 0, 0 );
                    } else {
                        gpu_blit( WHITE, Aliens[ y * 11 + x ].x, Aliens[ y * 11 + x ].y, Aliens[ y * 11 + x ].type * 2 +  Aliens[ y * 11 + x ].animation_count, 0, 0 );
                    }
                    break;
                case 4:
                    break;
                case 5:
                case 6:
                    Aliens[ y * 11 + x ].type--;
                    break;
                default:
                    gpu_blit( RED, Aliens[ y * 11 + x ].x, Aliens[ y * 11 + x ].y, 8 + framebuffer, 0, 0 );
                    Aliens[ y * 11 + x ].type--;
                    break;
            }
        }
    }

    // DRAW UFO
    switch( UFO.active ) {
        case UFOONSCREEN:
            gpu_blit( MAGENTA, UFO.x, 16, 10 + framebuffer, 0, 0 );
            if( !get_beep_active( 1 ) ) {
                beep( 1, 2, UFO.pitchcount ? 25 : 37, 100 );
                UFO.pitchcount = !UFO.pitchcount;
            }
            break;
        case UFOEXPLODE:
            gpu_printf_centre( framebuffer ? RED : LTRED, UFO.x + 7, 16, BOLD, 0, 0, "%d", UFO.score );
            if( !get_beep_active( 1 ) ) {
                beep( 1, 1, UFO.pitchcount ? 37 : 49, 25 );
                UFO.pitchcount = !UFO.pitchcount;
            }
            break;
        default:
            break;
    }
}

void move_aliens( void ) {
    // CHECK IF ANY ALIENS LEFT
    if( !AlienSwarm.count ) {
        Ship.level++;
        reset_aliens();
        trim_aliens();
    }

    // FIND AN ALIEN
    if( ( Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].type != 0 ) && ( AlienSwarm.newdirection ) ) {
        AlienSwarm.newdirection = 0;
    } else {
        do {
            switch( AlienSwarm.direction ) {
                case 1:
                    AlienSwarm.column--;
                    if( AlienSwarm.column < AlienSwarm.leftcolumn ) {
                        AlienSwarm.column = AlienSwarm.rightcolumn;
                        AlienSwarm.row++;
                    }
                    if( AlienSwarm.row > AlienSwarm.bottomrow ) {
                        if( !UFO.active ) {
                            beep( 1, 0, 1, 100 );
                        }
                        AlienSwarm.row = AlienSwarm.toprow;
                        AlienSwarm.column = AlienSwarm.rightcolumn;
                        for( short y = AlienSwarm.toprow; y <= AlienSwarm.bottomrow; y++ ) {
                            if( ( Aliens[ y * 11 + AlienSwarm.rightcolumn ].x >= 304 ) && ( Aliens[ y * 11 + AlienSwarm.rightcolumn ].type != 0 ) ) {
                                AlienSwarm.direction = 2;
                                AlienSwarm.column = AlienSwarm.leftcolumn;
                            }
                        }
                    }
                    break;
                case 0:
                    AlienSwarm.column++;
                    if( AlienSwarm.column > AlienSwarm.rightcolumn ) {
                        AlienSwarm.column = AlienSwarm.leftcolumn;
                        AlienSwarm.row++;
                    }
                    if( AlienSwarm.row > AlienSwarm.bottomrow ) {
                        if( !UFO.active ) {
                            beep( 1, 0, 1, 100 );
                        }
                        AlienSwarm.row = AlienSwarm.toprow;
                        AlienSwarm.column = AlienSwarm.leftcolumn;
                        for( short y = AlienSwarm.toprow; y <= AlienSwarm.bottomrow; y++ ) {
                            if( ( Aliens[ y * 11 + AlienSwarm.leftcolumn ].x <= 8 ) && ( Aliens[ y * 11 + AlienSwarm.leftcolumn ].type != 0 ) ) {
                                AlienSwarm.direction = 3;
                                AlienSwarm.column = AlienSwarm.rightcolumn;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        } while( ( Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].type == 0 ) && ( AlienSwarm.direction < 2 ) );
    }

    switch( AlienSwarm.direction ) {
        // MOVE LEFT OR RIGHT
        case 0:
        case 1:
            if( ( AlienSwarm.count > 1 ) || ( ( AlienSwarm.count == 1 ) && framebuffer ) ) {
                Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].x += ( AlienSwarm.direction == 1 ) ? 8 : -8;
                if( Ship.level & 1 ) {
                    switch( Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].animation_count ) {
                        case 2:
                            Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].animation_count = 0;
                            break;
                        default:
                            Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].animation_count++;
                            break;
                    }
                } else {
                    Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].animation_count = !Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].animation_count;
                }
            }
            break;

        // MOVE DOWN AND CHANGE DIRECTION
        case 2:
        case 3:
            for( short y = AlienSwarm.toprow; y <= AlienSwarm.bottomrow; y++ ) {
                for( short x = AlienSwarm.leftcolumn; x <= AlienSwarm.rightcolumn; x++ ) {
                    Aliens[ y * 11 + x ].y += 8;
                }
            }
            AlienSwarm.direction -= 2;
            AlienSwarm.newdirection = 1;
            break;
    }
}

void ufo_actions( void ) {
    switch( UFO.active ) {
        case 0:
            if( !UFO.lastufo ) {
                if( !rng(8) ) {
                    UFO.active = UFOONSCREEN;
                    switch( rng(1) ) {
                        case 0:
                            UFO.x = -15;
                            UFO.direction = 1;
                            break;
                        case 1:
                            UFO.x = 320;
                            UFO.direction = 0;
                            break;
                    }
                }
            } else {
                UFO.lastufo--;
            }
            break;
        case UFOONSCREEN:
            // MOVE THE UFO
            UFO.x += ( ( UFO.direction ) ? 1 : -1 ) * ( ( Ship.level > 0 ) ? 1 : framebuffer );
            if( ( UFO.x < -15 ) || ( UFO.x > 320 ) ) {
                UFO.active = 0;
                UFO.lastufo = 1000;
            }
            break;
        case UFOEXPLODE:
            if( !UFO.counter ) {
                UFO.active = 0;
                UFO.lastufo = 1000;
            } else {
                UFO.counter--;
            }
            break;
    }
}


void bomb_actions( void ) {
    short bombdropped = 0, bombcolumn, bombrow, attempts = 8;
    short bomb_x, bomb_y;

    // CHECK IF HIT AND MOVE BOMBS
    for( short i = 2; i < 16; i++ ) {
        if( get_sprite_layer_collision( UPPER_LAYER, i ) & SPRITE_TO_BITMAP ) {
            // HIT THE BUNKER
            bomb_x = get_sprite_attribute( UPPER_LAYER, i , 3 ) / 2 - 2;
            bomb_y = get_sprite_attribute( UPPER_LAYER, i , 4 ) / 2;
            set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, 0 );
            bitmap_draw( 0 ); gpu_blit( TRANSPARENT, bomb_x, bomb_y, 14, 0, 0 );
            bitmap_draw( 1 ); gpu_blit( TRANSPARENT, bomb_x, bomb_y, 14, 0, 0 );
            bitmap_draw( !framebuffer );
        } else {
            update_sprite( UPPER_LAYER, i, 0b1110010000000 );
        }
        if( get_sprite_collision( UPPER_LAYER, i ) & 2 ) {
            // HIT THE PLAYER MISSILE
            set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, 0 );
            set_sprite_attribute( UPPER_LAYER, 1, SPRITE_ACTIVE, 0 );
        }
        if( get_sprite_collision( UPPER_LAYER,i ) & 1 ) {
            // HIT THE PLAYER
            Ship.state = SHIPEXPLODE;
            Ship.counter = 100;
            remove_sprites( 1 );
        }
    }

    // CHECK IF FIRING
    AlienSwarm.lastbombtimer -= ( AlienSwarm.lastbombtimer ) > 0 ? 1 : 0;
    if( !AlienSwarm.lastbombtimer && !rng(4) ) {
        for( short i = 2; ( i < 16 ) && !bombdropped; i++ ) {
            if( !get_sprite_attribute( UPPER_LAYER, i, 0 ) ) {
                // BOMB SLOT FOUND
                // FIND A COLUMN AND BOTTOM ROW ALIEN
                while( !bombdropped && attempts ) {
                    bombcolumn = rng(11);
                    for( bombrow = 4; ( bombrow >= 0 ) && attempts && !bombdropped; bombrow-- ) {
                        switch( Aliens[ bombrow * 11 + bombcolumn ].type ) {
                            case 1:
                            case 2:
                            case 3:
                                set_sprite( UPPER_LAYER, i, 1, 2 * Aliens[ bombrow * 11 + bombcolumn ].x + 4 + ( rng(5) - 2 ), 2 * Aliens[ bombrow * 11 + bombcolumn ].y + 12, 0, SPRITE_DOUBLE );
                                AlienSwarm.lastbombtimer = ( Ship.level == 0 ) ? 32 : ( Ship.level > 2 ) ? 8 : 16;
                                bombdropped = 1;
                                break;
                            default:
                                break;
                        }
                        attempts--;
                    }
                }
            }
        }
    }
}

short missile_actions( void ) {
    short missile_x, missile_y, alien_hit = 0, points = 0;

    // CHECK IF PLAYER MISSILE HAS HIT
    if( get_sprite_layer_collision( UPPER_LAYER, 1 ) & SPRITE_TO_BITMAP ) {
        missile_x = get_sprite_attribute( UPPER_LAYER, 1, 3 ) / 2;
        missile_y = get_sprite_attribute( UPPER_LAYER, 1, 4 ) / 2;
        for( short y = AlienSwarm.toprow; y <= AlienSwarm.bottomrow && !alien_hit; y++ ) {
            for( short x = AlienSwarm.leftcolumn; x <= AlienSwarm.rightcolumn && !alien_hit; x++ ) {
                switch( Aliens[ y * 11 + x ].type ) {
                    case 1:
                    case 2:
                    case 3:
                        if( ( missile_x >= Aliens[ y * 11 + x ].x - 3 ) && ( missile_x <= Aliens[ y * 11 + x ].x + 13 ) && ( missile_y >= Aliens[ y * 11 + x ].y - 4 ) && ( missile_y <= Aliens[ y * 11 + x ].y + 12 ) ) {
                            beep( 2, 4, 8, 500 );
                            points = ( 4 - Aliens[ y * 11 + x ].type ) * 10;
                            set_sprite_attribute( UPPER_LAYER, 1, SPRITE_ACTIVE, 0 );
                            Aliens[ y * 11 + x ].type = 16;
                            alien_hit = 1;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        if( !alien_hit ) {
            set_sprite_attribute( UPPER_LAYER, 1, SPRITE_ACTIVE, 0 );
            if( missile_y > 24 ) {
                // HIT A BUNKER
                missile_x = missile_x - 2;
                missile_y = missile_y - 2;
                bitmap_draw( 0 ); gpu_blit( TRANSPARENT, missile_x, missile_y, 14, 0, 0 );
                bitmap_draw( 1 ); gpu_blit( TRANSPARENT, missile_x, missile_y, 14, 0, 0 );
                bitmap_draw( !framebuffer );
            } else {
                // HIT UFO
                if( UFO.active == UFOONSCREEN ) {
                    UFO.active = UFOEXPLODE;
                    UFO.counter = 100;
                    UFO.score = ( rng(3) + 1 ) * 50;
                    points = UFO.score;
                }
            }
        }
    }

    // FIRE? OR MOVE MISSILE
    if( !get_sprite_attribute( UPPER_LAYER, 1, 0 ) ) {
        // NO MISSILE, CHECK IF FIRE
        if( ( get_buttons() & 2 ) && ( Ship.state == SHIPPLAY ) ) {
            set_sprite( UPPER_LAYER, 1, 1, Ship.x + 8, Ship.y - 10, 0, SPRITE_DOUBLE );
            if( !get_beep_active( 2 ) ) {
                beep( 2, 4, 61, 128 );
            }
        }
    } else {
        // MOVE MISSILE
        update_sprite( UPPER_LAYER, 1, 0b1111100100000 );
    }

    return( points );
}

void player_actions( void ) {
    if( ( get_sprite_layer_collision( UPPER_LAYER, 0 ) & SPRITE_TO_BITMAP ) && ( Ship.state != SHIPEXPLODE2 ) ) {
        // ALIEN HAS HIT SHIP
        Ship.state = SHIPEXPLODE2;
        Ship.counter = 100;
        remove_sprites( 1 );
    }
    switch( Ship.state ) {
        case SHIPPLAY:
            if( ( get_buttons() & 32 ) && ( Ship.x > 0 ) )
                Ship.x -= 2;
            if( ( get_buttons() & 64 ) && ( Ship.x < 617 ) )
                Ship.x += 2;
            set_sprite( UPPER_LAYER, 0, 1, Ship.x, Ship.y, 0, SPRITE_DOUBLE );
            break;
        case SHIPRESET:
            // RESET
            set_sprite( UPPER_LAYER, 0, 0, Ship.x, Ship.y, 0, SPRITE_DOUBLE );
            Ship.counter--;
            if( !Ship.counter ) Ship.state = SHIPPLAY;
            break;
        case SHIPEXPLODE:
            // EXPLODE
            beep( 2, 4, 1 + framebuffer, 25 );
            set_sprite( UPPER_LAYER, 0, 1, Ship.x, Ship.y, 1 + framebuffer, SPRITE_DOUBLE );
            Ship.counter--;
            if( !Ship.counter ) {
                Ship.life--;
                reset_player();
            }
           break;
        case SHIPEXPLODE2:
            // EXPLODE
            beep( 2, 4, 1 + framebuffer, 25 );
            set_sprite( UPPER_LAYER, 0, 1, Ship.x, Ship.y, 1 + framebuffer, SPRITE_DOUBLE );
            Ship.counter--;
            if( !Ship.counter ) {
                Ship.life--;
                reset_aliens();
                trim_aliens();
                reset_player();
            }
           break;
    }
}

void draw_status( void ) {
    char scorestring[9];

    // GENERATE THE SCORE STRING
    sprintf( &scorestring[0], "%8u", Ship.score );

    // PRINT THE SCORE
    for( short i = 0; i < 8; i++ ) {
        set_tilemap_tile( UPPER_LAYER, 17 + i, 1,  ( scorestring[i] == ' ' ) ? 1 : scorestring[i] - 47, 0 );
    }
    // PRINT THE LIVES LEFT
    set_tilemap_tile( UPPER_LAYER, 35, 1,  Ship.life + 1, 0 );
    for( short i = 0; i < 3; i++ ) {
        set_tilemap_tile( UPPER_LAYER, 37 + i, 1,  ( i < Ship.life ) ? 11 : 0, 0 );
    }
    // PRINT THE LEVEL ( 2 DIGITS )
    set_tilemap_tile( UPPER_LAYER, 2, 1,  ( Ship.level / 10 ) + 1, 0 );
    set_tilemap_tile( UPPER_LAYER, 3, 1,  ( Ship.level % 10 ) + 1, 0 );
}

void play( void ) {
    reset_game();

    while( Ship.life > 0 ) {
        // DRAW TO HIDDEN FRAME BUFFER
        bitmap_draw( !framebuffer );

        // ADJUST SIZE OF ALIEN GRID
        trim_aliens();
        if( Ship.state < SHIPEXPLODE ) {
            // HANDLE MISSILES AND BOMBS
            Ship.score += missile_actions();
            bomb_actions();

            // MOVE ALIENS
            move_aliens();
        }

        player_actions();
        ufo_actions();

        // UPDATE THE SCREEN
        gpu_rectangle( TRANSPARENT, 0, 16, 319, AlienSwarm.bottompixel );
        draw_aliens();

        // SWITCH THE FRAMEBUFFER
        framebuffer = !framebuffer; bitmap_display( framebuffer );

        draw_status();
    }
}

void missile_demo( void ) {
    short missile_x, missile_y, alien_hit = 0;

    // CHECK IF PLAYER MISSILE HAS HIT
    if( get_sprite_layer_collision( UPPER_LAYER, 1 ) & SPRITE_TO_BITMAP ) {
        missile_x = get_sprite_attribute( UPPER_LAYER, 1, 3 ) / 2;
        missile_y = get_sprite_attribute( UPPER_LAYER, 1, 4 ) / 2;
        for( short y = AlienSwarm.toprow; y <= AlienSwarm.bottomrow && !alien_hit; y++ ) {
            for( short x = AlienSwarm.leftcolumn; x <= AlienSwarm.rightcolumn && !alien_hit; x++ ) {
                switch( Aliens[ y * 11 + x ].type ) {
                    case 1:
                    case 2:
                    case 3:
                        if( ( missile_x >= Aliens[ y * 11 + x ].x - 3 ) && ( missile_x <= Aliens[ y * 11 + x ].x + 13 ) && ( missile_y >= Aliens[ y * 11 + x ].y - 4 ) && ( missile_y <= Aliens[ y * 11 + x ].y + 12 ) ) {
                            beep( 2, 4, 8, 500 );
                            set_sprite_attribute( UPPER_LAYER, 1, SPRITE_ACTIVE, 0 );
                            Aliens[ y * 11 + x ].type = 16;
                            alien_hit = 1;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        if( !alien_hit && ( missile_y >= 208 ) ) {
            set_sprite_attribute( UPPER_LAYER, 1, SPRITE_ACTIVE, 0 );
            // HIT A BUNKER
            missile_x = missile_x - 2;
            missile_y = missile_y - 2;
            bitmap_draw( 0 ); gpu_blit( TRANSPARENT, missile_x, missile_y, 14, 0, 0 );
            bitmap_draw( 1 ); gpu_blit( TRANSPARENT, missile_x, missile_y, 14, 0, 0 );
            bitmap_draw( !framebuffer );
        }
    }

    // FIRE? OR MOVE MISSILE
    if( !get_sprite_attribute( UPPER_LAYER, 1, 0 ) ) {
        // NO MISSILE, CHECK IF FIRE
        if( ( Ship.state == SHIPPLAY ) && !rng(8) ) {
            set_sprite( UPPER_LAYER, 1, 1, Ship.x + 8, Ship.y - 10, 0, SPRITE_DOUBLE );
            if( !get_beep_active( 2 ) ) {
                beep( 2, 4, 61, 128 );
            }
        }
    } else {
        // MOVE MISSILE
        update_sprite( UPPER_LAYER, 1, 0b1111100100000 );
    }
}

void demo_actions( void ) {
    if( ( get_sprite_layer_collision( UPPER_LAYER, 0 ) & SPRITE_TO_BITMAP ) && ( Ship.state != SHIPEXPLODE2 ) ) {
        // ALIEN HAS HIT SHIP
        Ship.state = SHIPEXPLODE2;
        Ship.counter = 100;
        for( short i = 1; i < 16; i++ ) {
            set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, 0 );
        }
    }
    switch( Ship.state ) {
        case SHIPPLAY:
            // CODE TO MOVE
            set_sprite( UPPER_LAYER, 0, 1, Ship.x, Ship.y, 0, SPRITE_DOUBLE );
            break;
        case SHIPRESET:
            // RESET
            set_sprite( UPPER_LAYER, 0, 0, Ship.x, Ship.y, 0, SPRITE_DOUBLE );
            Ship.counter--;
            if( !Ship.counter ) Ship.state = SHIPPLAY;
            break;
        case SHIPEXPLODE:
            // EXPLODE
            beep( 2, 4, 1 + framebuffer, 25 );
            set_sprite( UPPER_LAYER, 0, 1, Ship.x, Ship.y, 1 + framebuffer, SPRITE_DOUBLE );
            Ship.counter--;
            if( !Ship.counter ) {
                reset_player();
            }
           break;
        case SHIPEXPLODE2:
            // EXPLODE
            beep( 2, 4, 1 + framebuffer, 25 );
            set_sprite( UPPER_LAYER, 0, 1, Ship.x, Ship.y, 1 + framebuffer, SPRITE_DOUBLE );
            Ship.counter--;
            if( !Ship.counter ) {
                reset_aliens();
                trim_aliens();
                reset_player();
            }
           break;
    }
}

void attract( void ) {
    short mode = 0, animation = 0, move_amount = 0;

    UFO.active = 0;
    while( !( get_buttons() & 8 ) ) {
        bitmap_draw( 0 );gpu_cs();
        bitmap_draw( 1 );gpu_cs();
        tpu_cs();
        // CLEAR THE SPRITES
        for( short i = 0; i < 13; i++ ) {
            set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, 0 );
        }
        set_timer1khz( 16000, 0 );
        if( mode ) {
            reset_aliens();
            trim_aliens();
            reset_player();
        } else {
        }
        while( get_timer1khz( 0 ) && !( get_buttons() & 8 ) ) {
            if( !get_timer1khz( 1 ) ) {
                animation = !animation;
                set_timer1khz( 1000, 1 );
            }
            switch( mode ) {
                case 0:
                    // WELCOME SCREEN
                    // DRAW TO HIDDEN FRAME BUFFER
                    bitmap_draw( !framebuffer ); gpu_cs();
                    gpu_blit( WHITE, 128, 64, 2 + animation, 1, 0 ); gpu_printf_centre( RED, 176, 64, BOLD, 1, 0, "%d", 30, 0 );
                    gpu_blit( WHITE, 128, 96, 4 + animation, 1, 0 ); gpu_printf_centre( RED, 176, 96, BOLD, 1, 0, "%d", 20, 0 );
                    gpu_blit( WHITE, 128, 128, 6 + animation, 1, 0 ); gpu_printf_centre( RED, 176, 128, BOLD, 1, 0, "%d", 10, 0 );
                    gpu_blit( MAGENTA, 126, 160, 10 + animation, 1, 0 ); gpu_printf_centre( RED, 176, 160, BOLD, 1, ROTATE0 + ( systemclock() & 3 ), "?", 0 );

                    switch( animation ) {
                        case 0:
                            gpu_printf_centre( WHITE, 160, 32, NORMAL, 1, 0, "PAWS" );
                            gpu_printf_centre( WHITE, 160, 208, NORMAL, 0, 0, "BY @ROBNG15 WHITEBRIDGE, SCOTLAND" );
                            break;
                        case 1:
                            gpu_printf_centre( WHITE, 160, 32, NORMAL, 1, 0, "SPACE INVADERS" );
                            gpu_printf_centre( WHITE, 160, 208, NORMAL, 0, 0, "PRESS UP TO START" );
                            break;
                    }

                    // SWITCH THE FRAMEBUFFER
                    framebuffer = !framebuffer;
                    bitmap_display( framebuffer );

                    draw_status();
                    break;
                case 1:
                    // MINI DEMO
                    // DRAW TO HIDDEN FRAME BUFFER
                    bitmap_draw( !framebuffer );
                    // ADJUST SIZE OF ALIEN GRID
                    trim_aliens();
                    // MOVE ALIENS
                    if( Ship.state < SHIPEXPLODE ) {
                        // HANDLE BOMBS
                        missile_demo();
                        bomb_actions();
                        // MOVE ALIENS
                        move_aliens();
                    }
                    // MOVE THE DEMO SHIP
                    while( !move_amount ) { move_amount = rng( 64 ) - 32; }
                    if( move_amount < 0 ) {
                        Ship.x += ( Ship.x > 0 ) ? -2 : 0;
                        move_amount = ( Ship.x > 0 ) ? move_amount + 1 : 0;
                    } else {
                        Ship.x += ( Ship.x < 617 ) ? 2 : 0;
                        move_amount = ( Ship.x < 617 ) ? move_amount - 1 : 0;
                    }
                    demo_actions();
                    // UPDATE THE SCREEN
                    gpu_rectangle( TRANSPARENT, 0, 16, 319, AlienSwarm.bottompixel );
                    draw_aliens();

                    // MESSAGE
                    gpu_rectangle( TRANSPARENT, 0, 120, 319, 128 );
                    switch( animation ) {
                        case 0:
                            gpu_printf_centre( WHITE, 160, 120, NORMAL, 0, 0, "PAWS SPACE INVADERS" );
                            break;
                        case 1:
                            gpu_printf_centre( WHITE, 160, 120, NORMAL, 0, 0, "PRESS UP TO START" );
                            break;
                    }

                    // SWITCH THE FRAMEBUFFER
                    framebuffer = !framebuffer;
                    bitmap_display( framebuffer );

                    draw_status();
                    break;
            }
        }
        mode = !mode;
    }
    tpu_cs();
}

int main( void ) {
    INITIALISEMEMORY();
    initialise_graphics();

    while(1) {
        bitmap_draw( 0 );gpu_cs();
        bitmap_draw( 1 );gpu_cs();

        attract();
        play();
    }
}
