// PAWS implementation of http://nicktasios.nl/posts/space-invaders-from-scratch-part-1.html

#include <stdio.h>
#include <PAWSlibrary.h>

// INCLUDE GRAPHICS
unsigned char backdrop[] = {
    #include "graphics/backdrop.h"
};

unsigned char alien_sprites[] = {
    #include "graphics/aliens.h"
};

unsigned char bullet_sprites[] = {
    #include "graphics/bullets.h"
};

unsigned char player_ufo[] = {
    #include "graphics/player_ufo.h"
};

unsigned char bunker_pixelblock[] = {
    #include "graphics/bunker.h"
};

// BLITTER OBJECT FOR ERASING PART OF THE BUNKERS WHEN BOMB HITS THE BUNKERS
unsigned short bomb_hit[] = {
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000100000000,
    0b0000010001000000,
    0b0000000110100000,
    0b0000001111000000,
    0b0000010111000000,
    0b0000001111100000,
    0b0000010111000000,
    0b0000001010100000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000
};

#include "graphics/tilemap_numbers.h"

// INCLUDE WAVE FILES
#include "sounds/8_explode.h"
#include "sounds/8_hit.h"
#include "sounds/8_shoot.h"

#define MAXALIENS 55
struct Alien {
    short x, y;
    short type, animation_count;
    short layer, sprite;
};
struct Alien Aliens[ MAXALIENS ];

struct Swarm {
    short leftcolumn, rightcolumn, toprow, bottomrow, leftpixel, rightpixel, bottompixel;
    short row, column, direction, newdirection;
    short lastbombtimer;
    short count;
};
struct Swarm AlienSwarm;

#define UFOONSCREEN 1
#define UFOEXPLODE 2
#define UFOSCORE 3
struct Ufo {
    short active;
    short counter;
    short x, direction;
    short lastufo;
    short pitchcount;
    short score;
};
struct Ufo UFO;

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

// BOMBS
#define BOMB_START 24
#define BOMB_END 28
#define MAXBOMBS 5
#define BOMB_INACTIVE 0
#define BOMB_TYPE1 1
#define BOMB_TYPE2 2
#define BOMB_EXPLODE 3
struct Bomb {
    short x, y;
    short type;
    short frame;
    short counter;
};
struct Bomb Bombs[ MAXBOMBS ];
unsigned short bomb_timers[] = { 32, 30, 28, 26, 24, };

#define MISSILESPRITE UPPER_LAYER,29
#define UFOSPRITE UPPER_LAYER,30
#define PLAYERSPRITE UPPER_LAYER,31

unsigned int framecount;

// HELPER TO REMOVE ALL/SOME SPRITES
void remove_sprites( int start_sprite ) {
    for( int i = start_sprite; i < 32; i++ ) {
        set_sprite_attribute( LOWER_LAYER, i, SPRITE_ACTIVE, SPRITE_HIDE );
        set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, SPRITE_HIDE );
    }
}

// HELPER TO REMOVE BULLETS
void remove_bullets() {
    for( int i = 24; i < 30; i++ )
        set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, SPRITE_HIDE );
}

// SETUP THE BACKDROP
void draw_moonscape( void ) {
    bitmap_draw( 1 );
    gpu_pixelblock( 0, 0, 320, 240, TRANSPARENT, backdrop );
    bitmap_draw( 2 );
    bitmap_display( 3 );
}

void initialise_graphics( void ) {
    // DROP THE BITMAP TO JUST ABOVE THE BACKGROUND AND PAWSv2 COLOUR PALETTE
    screen_mode( 1, MODE_RGBM, 0 );

    // SET THE ALIEN SPRITES
    for( int i = 0; i < 16; i++ ) {
        set_sprite_bitmaps( LOWER_LAYER, i, alien_sprites );
        set_sprite_bitmaps( UPPER_LAYER, i, alien_sprites );
    }

    // SET THE BOMB/MISSILE SPRITES
    for( int i = 12; i < 15; i++ ) {
        set_sprite_bitmaps( UPPER_LAYER, i, bullet_sprites );
    }

    // SET THE PLAYER AND UFO SPRITES
    set_sprite_bitmaps( UPPER_LAYER, 15, player_ufo );
    draw_moonscape();

    // SET THE UPPER TILEMAP TO THE NUMBERS AND PLAYER SHIP
    for( short i = 0; i < 12; i++ ) {
        set_tilemap_bitmap( UPPER_LAYER, i + 1, &tilemap_bitmaps_front[ 256 * i ] );
    }

    set_blitter_bitmap( 0, bomb_hit );
}

void reset_aliens( void ) {
    // REMOVE THE PLAYER, MISSILE AND BOMBS
    remove_sprites( 0 );

    int alien = 0, layer = 0;

    // SET THE ALIENS
    for( short y = 0; y < 5; y++ ) {
        for( short x = 0; x < 11; x++ ) {
            Aliens[ alien ].x = 32 * x + 16;
            Aliens[ alien ].y = 32 * ( ( Ship.level < 4 ) ? Ship.level : 8 ) + 32 * y + 48;
            switch( y ) {
                case 0:
                    Aliens[ alien ].type = 1;
                    break;
                case 1:
                case 2:
                    Aliens[ alien ].type = 2;
                    break;
                default:
                    Aliens[ alien ].type = 3;
                    break;
            }
            Aliens[ alien ].animation_count = 0;
            Aliens[ alien ].layer = ( ( alien < 32 ) ? LOWER_LAYER :  UPPER_LAYER );
            Aliens[ alien ].sprite = ( alien & 31 );

            set_sprite( Aliens[ alien ].layer, Aliens[ alien ].sprite, SPRITE_SHOW, Aliens[ alien ].x, Aliens[ alien ].y, ( Aliens[ alien ].type - 1 ) * 2, SPRITE_DOUBLE );
            alien++;
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

    // DRAW BUNKERS
    for( short j = 0; j < 4; j++ ) {
        gpu_pixelblock( 24 + j * 80,  208, 22, 16, TRANSPARENT, bunker_pixelblock );
    }
}

void reset_player( void ) {
    Ship.state = SHIPRESET;
    Ship.counter = 32;
    Ship.x = 320 - 12;
    Ship.y = 447;
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
    short left = 10, right = 0, top = 4, bottom = 0, pixel_l = 639, pixel_r = 0, pixel_b = 16;

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
                case 1:
                case 2:
                case 3:
                    left = min( left, x );
                    right = max( right, x );
                    top = min( top, y );
                    bottom = max( bottom, y );
                    pixel_l = min ( pixel_l, Aliens[ y * 11 + x ].x );
                    pixel_r = max ( pixel_r, Aliens[ y * 11 + x ].x + 16 );
                    pixel_b =  max( pixel_b, Aliens[ y * 11 + x ].y + 14 );
                    break;
                case 4:
                    Aliens[ y * 11 + x ].type = 0;
                    set_sprite_attribute( Aliens[ y * 11 + x ].layer, Aliens[ y * 11 + x ].sprite, SPRITE_ACTIVE, SPRITE_HIDE );
                    AlienSwarm.count--;
                    break;
                default:
                    Aliens[ y * 11 + x ].type--;
                    break;
            }
        }
    }
    AlienSwarm.leftcolumn = left;
    AlienSwarm.rightcolumn = right;
    AlienSwarm.toprow = top;
    AlienSwarm.bottomrow = bottom;
    AlienSwarm.leftpixel = pixel_l;
    AlienSwarm.rightpixel = pixel_r;
    AlienSwarm.bottompixel = pixel_b;
}

unsigned char note_table[] = { 17, 13, 11, 7 };
unsigned int note_position = 0;
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
                case 0:
                    AlienSwarm.column++;
                    if( AlienSwarm.column > AlienSwarm.rightcolumn ) {
                        AlienSwarm.column = AlienSwarm.leftcolumn;
                        AlienSwarm.row++;
                    }
                    if( AlienSwarm.row > AlienSwarm.bottomrow ) {
                        beep( 1, 0, note_table[ note_position ], 100 );
                        note_position = ( note_position + 1 ) & 3;
                        AlienSwarm.row = AlienSwarm.toprow;
                        AlienSwarm.column = AlienSwarm.leftcolumn;
                        for( short y = AlienSwarm.toprow; y <= AlienSwarm.bottomrow; y++ ) {
                            if( ( Aliens[ y * 11 + AlienSwarm.leftcolumn ].x <= 16 ) && ( Aliens[ y * 11 + AlienSwarm.leftcolumn ].type != 0 ) ) {
                                AlienSwarm.direction = 3;
                                AlienSwarm.column = AlienSwarm.rightcolumn;
                            }
                        }
                    }
                    break;
                case 1:
                    AlienSwarm.column--;
                    if( AlienSwarm.column < AlienSwarm.leftcolumn ) {
                        AlienSwarm.column = AlienSwarm.rightcolumn;
                        AlienSwarm.row++;
                    }
                    if( AlienSwarm.row > AlienSwarm.bottomrow ) {
                        beep( 1, 0, note_table[ note_position ], 100 );
                        note_position = ( note_position + 1 ) & 3;
                        AlienSwarm.row = AlienSwarm.toprow;
                        AlienSwarm.column = AlienSwarm.rightcolumn;
                        for( short y = AlienSwarm.toprow; y <= AlienSwarm.bottomrow; y++ ) {
                            if( ( Aliens[ y * 11 + AlienSwarm.rightcolumn ].x >= 608 ) && ( Aliens[ y * 11 + AlienSwarm.rightcolumn ].type != 0 ) ) {
                                AlienSwarm.direction = 2;
                                AlienSwarm.column = AlienSwarm.leftcolumn;
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
            Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].x += ( AlienSwarm.direction == 1 ) ? 16 : -16;
            Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].animation_count = !Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].animation_count;
            set_sprite_attribute( Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].layer, Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].sprite, SPRITE_X, Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].x );
            set_sprite_attribute( Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].layer, Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].sprite, SPRITE_TILE, Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].animation_count + 2 * ( Aliens[ AlienSwarm.row * 11 + AlienSwarm.column ].type - 1) );
            trim_aliens();
            break;

        // MOVE DOWN AND CHANGE DIRECTION, WIPE BOTTOM LINE TO CLEAR BUNKERS
        case 2:
        case 3:
            if( Ship.life > 0 ) {
                for( short y = AlienSwarm.toprow; y <= AlienSwarm.bottomrow; y++ ) {
                    for( short x = AlienSwarm.leftcolumn; x <= AlienSwarm.rightcolumn; x++ ) {
                        Aliens[ y * 11 + x ].y += 16;
                        set_sprite_attribute( Aliens[ y * 11 + x ].layer, Aliens[ y * 11 + x ].sprite, SPRITE_Y, Aliens[ y * 11 + x ].y );
                    }
                }
                AlienSwarm.direction -= 2;
                AlienSwarm.newdirection = 1;
                trim_aliens();
                if( AlienSwarm.bottompixel > 414 ) gpu_rectangle( TRANSPARENT, 0, 207, 319, AlienSwarm.bottompixel >> 1 );
            } else {
                AlienSwarm.direction -= 2;
                AlienSwarm.newdirection = 1;
            }
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
                            UFO.x = -31;
                            UFO.direction = 1;
                            break;
                        case 1:
                            UFO.x = 640;
                            UFO.direction = 0;
                            break;
                    }
                    set_sprite( UFOSPRITE, SPRITE_SHOW, UFO.x, 8, 3, SPRITE_DOUBLE );
                }
            } else {
                UFO.lastufo--;
            }
            break;
        case UFOONSCREEN:
            // MOVE THE UFO
            UFO.x += ( ( UFO.direction ) ? 2 : -2 );
            if( ( UFO.x < -31 ) || ( UFO.x > 640 ) ) {
                UFO.active = 0;
                UFO.lastufo = 1000;
            }
            set_sprite_attribute( UFOSPRITE, SPRITE_X, UFO.x );
            if( !get_beep_active( 2 ) ) {
                beep( 2, 2, UFO.pitchcount ? 25 : 37, 100 );
                UFO.pitchcount = !UFO.pitchcount;
            }
            break;
        case UFOEXPLODE:
            if( !UFO.counter ) {
                UFO.active = UFOSCORE;
                UFO.counter = 30;
                UFO.lastufo = 1000;
                set_sprite_attribute( UFOSPRITE, SPRITE_TILE, 4 + UFO.score );
            } else {
                UFO.counter--;
            }
            if( !get_beep_active( 2 ) ) {
                beep( 2, 1, UFO.pitchcount ? 37 : 49, 25 );
                UFO.pitchcount = !UFO.pitchcount;
            }
            break;
        case UFOSCORE:
            if( !UFO.counter ) {
                UFO.active = 0;
                UFO.lastufo = 1000;
                set_sprite_attribute( UFOSPRITE, SPRITE_ACTIVE, SPRITE_HIDE );
            } else {
                UFO.counter--;
            }
            break;
    }
}


void bomb_actions( void ) {
    short bombdropped = 0, bombcolumn, bombrow, attempts = 8;
    short bomb_x, bomb_y;
    static short lasttype = 0;

    // CHECK IF HIT AND MOVE BOMBS
    for( int i = BOMB_START; i <= BOMB_END; i++ ) {
        if( get_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE ) ) {
            switch( Bombs[ i - BOMB_START ].type ) {
                case 0:
                    set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, SPRITE_HIDE );
                    break;
                case 1:
                case 2:
                    if( get_sprite_collision( UPPER_LAYER, i ) & ( 1 << 29 ) ) {
                        // HIT THE PLAYER MISSILE
                        set_sprite_attribute( UPPER_LAYER, i, SPRITE_TILE, 7 );
                        set_sprite_attribute( MISSILESPRITE, SPRITE_ACTIVE, 0 );
                        Bombs[ i - BOMB_START ].type = 3;
                        Bombs[ i - BOMB_START ].counter = 8;
                    } else {
                        if( get_sprite_collision( UPPER_LAYER,i ) & ( 1 << 31 ) ) {
                            // HIT THE PLAYER
                            beep( 3, 0, 0, 0 ); pcmsample_stop( 3 );
                            pcmsample_start( 1, 14625, &wave_explode[78], KHz11025, FALSE );
                            Ship.state = SHIPEXPLODE;
                            Ship.counter = 100;
                            remove_bullets();
                        } else {
                            if( get_sprite_layer_collision( UPPER_LAYER, i ) & SPRITE_TO_BITMAP_1 ) {
                                // HIT THE BUNKER
                                bomb_x = get_sprite_attribute( UPPER_LAYER, i , SPRITE_X ) / 2 - ( (rng(5) - 2 ));
                                bomb_y = get_sprite_attribute( UPPER_LAYER, i , SPRITE_Y ) / 2;
                                set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, 0 );
                                gpu_blit( TRANSPARENT, bomb_x, bomb_y, 0, 0, 0 );
                            } else {
                                update_sprite( UPPER_LAYER, i, ( Ship.level > 1 ) ? 4 : 3, 0, 2, 0 );
                                Bombs[ i - BOMB_START ].frame = 1 - Bombs[ i - BOMB_START ].frame;
                                set_sprite_attribute( UPPER_LAYER, i, SPRITE_TILE, ( Bombs[ i - BOMB_START ].type - 1 ) * 2 + Bombs[ i - BOMB_START ].frame );
                            }
                        }
                    }
                    break;
                case 3:
                    if( Bombs[ i - BOMB_START ].counter ) {
                        Bombs[ i - BOMB_START ].counter--;
                        set_sprite_attribute( UPPER_LAYER, i, SPRITE_TILE, 7 );
                    } else {
                        Bombs[ i - BOMB_START ].type = 0;
                    }
                    break;
            }
        }
    }

    // CHECK IF FIRING
    AlienSwarm.lastbombtimer -= ( AlienSwarm.lastbombtimer ) > 0 ? 1 : 0;
    if( !AlienSwarm.lastbombtimer && !rng(4) ) {
        for( short i = BOMB_START; ( i <= BOMB_END ) && !bombdropped; i++ ) {
            if( !get_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE ) ) {
                // BOMB SLOT FOUND
                // FIND A COLUMN AND BOTTOM ROW ALIEN
                while( !bombdropped && attempts ) {
                    bombcolumn = rng(11);
                    for( bombrow = 4; ( bombrow >= 0 ) && attempts && !bombdropped; bombrow-- ) {
                        switch( Aliens[ bombrow * 11 + bombcolumn ].type ) {
                            case 1:
                            case 2:
                            case 3:
                                Bombs[ i - BOMB_START ].x = Aliens[ bombrow * 11 + bombcolumn ].x + 4 + ( rng(5) - 2 );
                                Bombs[ i - BOMB_START ].y = Aliens[ bombrow * 11 + bombcolumn ].y + 16;
                                Bombs[ i - BOMB_START ].type = lasttype + 1;
                                Bombs[ i - BOMB_START ].frame = 0;
                                Bombs[ i - BOMB_START ].counter = 0;
                                set_sprite( UPPER_LAYER, i, 1, Bombs[ i - BOMB_START ].x, Bombs[ i - BOMB_START ].y, ( Bombs[ i - BOMB_START ].type - 1 ) * 2, SPRITE_DOUBLE );
                                AlienSwarm.lastbombtimer = ( Ship.level < 10 ) ? 34 - Ship.level * 2 : 8;
                                bombdropped = 1; lasttype = 1 - lasttype;
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
    short missile_x, missile_y, points = 0;
    int missile_collision = get_sprite_collision( MISSILESPRITE ), missile_layer_collision = get_sprite_layer_collision( MISSILESPRITE );

    // CHECK IF PLAYER MISSILE HAS HIT A BUNKER
    if( missile_layer_collision & SPRITE_TO_BITMAP_1 ) {
        missile_x = get_sprite_attribute( MISSILESPRITE, SPRITE_X ) / 2 - ( (rng(5) - 2 ));
        missile_y = get_sprite_attribute( MISSILESPRITE, SPRITE_Y ) / 2 - rng(8);
        set_sprite_attribute( MISSILESPRITE, SPRITE_ACTIVE, 0 );
        gpu_blit( TRANSPARENT, missile_x, missile_y, 0, 0, 0 );
    } else {
        // CHECK IF PLAYER MISSILE HAS HIT THE UFO
        if( missile_collision & ( 1 << 30 ) ) {
            UFO.active = UFOEXPLODE;
            beep( 2, 0, 0, 0 );
            UFO.counter = 30;
            UFO.score = rng(3) + 1;
            points = UFO.score * 50;
            set_sprite_attribute( UFOSPRITE, SPRITE_TILE, 4 );
        } else {
            // CHECK IF PLAYER MISSILE HAS HIT AN ALIEN
            if( ( missile_collision & 0xffffff ) || ( missile_layer_collision & SPRITE_TO_OTHER_SPRITES ) ) {
                if( missile_collision & 0xffffff ) {
                    // HIT AN UPPER SPRITE ALIEN, FIND
                    for( int i = 0; ( i < BOMB_START ) && ( points == 0 ); i++ ) {
                        if( get_sprite_collision( UPPER_LAYER, i ) & ( 1 << 29 ) ) {
                            pcmsample_start( 2, 5058, &wave_hit[78], KHz11025, FALSE );
                            points = ( 4 - Aliens[ 32 + i ].type ) * 10;
                            Aliens[ 32 + i ].type = 16;
                            set_sprite_attribute( UPPER_LAYER, i, SPRITE_TILE, 6 );
                            set_sprite_attribute( MISSILESPRITE, SPRITE_ACTIVE, 0 );
                        }
                    }
                } else {
                    if( missile_layer_collision & SPRITE_TO_OTHER_SPRITES ) {
                        // HIT A LOWER SPRITE ALIEN, FIND
                        for( int i = 0; ( i < 32 ) && ( points == 0 ); i++ ) {
                            if( get_sprite_layer_collision( LOWER_LAYER, i ) & SPRITE_TO_OTHER_SPRITES ) {
                                pcmsample_start( 2, 5058, &wave_hit[78], KHz11025, FALSE );
                                points = ( 4 - Aliens[ i ].type ) * 10;
                                Aliens[ i ].type = 16;
                                set_sprite_attribute( LOWER_LAYER, i, SPRITE_TILE, 6 );
                                set_sprite_attribute( MISSILESPRITE, SPRITE_ACTIVE, 0 );
                            }
                        }
                    }
                }
            }
        }
    }

    // FIRE? OR MOVE MISSILE
    if( !get_sprite_attribute( MISSILESPRITE, SPRITE_ACTIVE ) ) {
        // NO MISSILE, CHECK IF FIRE
        if( ( get_buttons() & 2 ) && ( Ship.state == SHIPPLAY ) ) {
            set_sprite( MISSILESPRITE, 1, Ship.x, Ship.y - 2, 4, SPRITE_DOUBLE );
            pcmsample_start( 1, 3822, &wave_shoot[78], KHz11025, FALSE );
        }
    } else {
        // MOVE MISSILE
        update_sprite( MISSILESPRITE, 3, 0, -5, 0 );
        set_sprite_attribute( MISSILESPRITE, SPRITE_TILE, framecount & 1 );
    }

    return( points );
}

void player_actions( void ) {
    if( ( AlienSwarm.bottompixel > 470 ) && ( Ship.state != SHIPEXPLODE2 ) ) {               // ALIEN HAS LANDED
        beep( 3, 0, 0, 0 ); pcmsample_stop( 3 );
        pcmsample_start( 1, 14625, &wave_explode[78], KHz11025, FALSE );
        Ship.state = SHIPEXPLODE2;
        Ship.counter = 100;
        remove_bullets();
    }
    switch( Ship.state ) {
        case SHIPPLAY:
            if( ( get_buttons() & 32 ) && ( Ship.x > 0 ) )
                Ship.x -= 2;
            if( ( get_buttons() & 64 ) && ( Ship.x < 615 ) )
                Ship.x += 2;
            set_sprite( PLAYERSPRITE, 1, Ship.x, Ship.y, 0, SPRITE_DOUBLE );
            break;
        case SHIPRESET:
            // RESET
            set_sprite( PLAYERSPRITE, 0, Ship.x, Ship.y, 0, SPRITE_DOUBLE );
            Ship.counter--;
            if( !Ship.counter ) Ship.state = SHIPPLAY;
            break;
        case SHIPEXPLODE:
            // EXPLODE
            set_sprite( PLAYERSPRITE, 1, Ship.x, Ship.y, 1 + ( framecount & 1 ), SPRITE_DOUBLE );
            Ship.counter--;
            if( !Ship.counter ) {
                Ship.life--;
                reset_player();
            }
            break;
        case SHIPEXPLODE2:
            // EXPLODE
            set_sprite( PLAYERSPRITE, 1, Ship.x, Ship.y, 1 + ( framecount & 1 ), SPRITE_DOUBLE );
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
        set_tilemap_tile_abs( UPPER_LAYER, 17 + i, 2,  ( scorestring[i] == ' ' ) ? 1 : scorestring[i] - 47, 0 );
    }
    // PRINT THE LIVES LEFT
    set_tilemap_tile_abs( UPPER_LAYER, 35, 2,  Ship.life + 1, 0 );
    for( short i = 0; i < 3; i++ ) {
        set_tilemap_tile_abs( UPPER_LAYER, 37 + i, 2,  ( i < Ship.life ) ? 11 : 0, 0 );
    }
    // PRINT THE LEVEL ( 2 DIGITS )
    set_tilemap_tile_abs( UPPER_LAYER, 2, 2,  ( Ship.level / 10 ) + 1, 0 );
    set_tilemap_tile_abs( UPPER_LAYER, 3, 2,  ( Ship.level % 10 ) + 1, 0 );
}

void play( void ) {
    reset_aliens();
    reset_game();
    reset_player();

    UFO.active = 0;
    UFO.lastufo = 1000;

    tpu_print_centre( 30, TRANSPARENT, WHITE, TPU_BOLD | TPU_X2 | TPU_Y2, "READY!" );
    sleep1khz( 1000, 0 );
    tpu_cs();

    while( Ship.life > 0 ) {
        framecount = get_framecount();

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

        // SWITCH THE FRAMEBUFFER
        while( get_framecount() == framecount ) {}

        draw_status();
    }

    tpu_cs();
    tpu_print_centre( 30, TRANSPARENT, WHITE, TPU_BOLD | TPU_X2 | TPU_Y2, "GAME OVER" );
    sleep1khz( 4000, 0 );
    tpu_cs();
}

void demo( void ) {
    initialise_graphics();
    reset_aliens();
    reset_player();

    UFO.active = 0;
    UFO.lastufo = 1000;

    tpu_cs();
    tpu_print_centre( 30, TRANSPARENT, WHITE, TPU_BOLD | TPU_X2 | TPU_Y2 | TPU_BLINK, "PAWS Space Invaders" );
    tpu_print_centre( 34, TRANSPARENT, WHITE, TPU_X2 | TPU_Y2 | TPU_BLINK, "Press UP to PLAY" );
    tpu_print_centre( 36, TRANSPARENT, WHITE, TPU_X2 | TPU_Y2 | TPU_BLINK, "Press DOWN to EXIT" );
    tpu_print_centre( 59, TRANSPARENT, WHITE, TPU_BLINK, "By Rob S, Whitebridge, Scotland");

    while( !( get_buttons() & 0x18 ) ) {
        framecount = get_framecount();
            if( Ship.state < SHIPEXPLODE ) {
                move_aliens();
                bomb_actions();
                ufo_actions();
            }
            draw_status();
        while ( get_framecount() == framecount ) {}
    }

    tpu_cs();
}

int main( int argc, char **argv ) {
    initialise_graphics();
    reset_aliens();
    reset_game(); Ship.life = 0;
    reset_player();

    do {
        demo();
        if( get_buttons() & 0x08 ) play();
    } while( !( get_buttons() & 0x10 ) );
}
