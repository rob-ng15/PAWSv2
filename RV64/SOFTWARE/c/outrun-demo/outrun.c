// PSEUDO 3D RACER BASED UPON:
// https://www.lexaloffle.com/bbs/?tid=35767

#include <math.h>
#include <stdio.h>
#include <PAWSlibrary.h>

#include "graphics/outrun-graphics.h"

unsigned char *cityscape[]={
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "  Aa                                                          ",
    "  Bb                         JjJj    JjJj                     ",
    "  Cc                      D  KkKk    KkKk                     ",
    "  Cc                      E  LlLl    LlLl                     ",
    "  Cc  OTTTTo              F  MmMm    MmMm              OTTTTo ",
    "  Cc  PUUUUp  V        v  G  NnNn    NnNn  V        v  PUUUUp ",
    "  Cc  QqQqQq  W        w  H  NnNnNnNnNnNn  W        w  QqQqQq ",
    "  Cc  RrRrRr  X        x  I  NnNnNnNnNnNn  X        x  RrRrRr ",
    "  Cc  SsSsSs  yYyY  yYyY  I  NnNn    NnNn  yYyY  yYyY  SsSsSs ",
    "  Cc  QqQqQq  YyYyYyYyYy  I  NnNnNnNnNnNn  YyYyYyYyYy  QqQqQq ",
    "  Cc  RrRrRr  yYyYyYyYyY  I  NnNnNnNnNnNn  yYyYyYyYyY  RrRrRr ",
    "  Cc  SsQsSs  YyYy  YyYy  I  NnNn    NnNn  YyYy  YyYy  SsQsSs ",
    "##############################################################",
    "**************************************************************",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
};

unsigned char *backdrop[]={
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "      afk  afk                 afk            afk  afk  afk   ",
    " ADH  bgl  bgl  ADH  ADH  ADH  bgl  ADH  ADH  bgl  bgl  bgl   ",
    " BEI  chm  chm  BEI  BEI  BEI  chm  BEI  BEI  chm  chm  chm   ",
    " CFJ  din  din  CFJ  CFJ  CFJ  din  CFJ  CFJ  din  din  din   ",
    "  G   ejo  ejo   G    G    G   ejo   G    G   ejo  ejo  ejo   ",
    "##############################################################",
    "**************************************************************",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
    "                                                              ",
};

// Music
unsigned char *passing_breeze;
unsigned int music_size;

void set_tilemaps( void ) {
    for( int i = 0; i < 4; i++ ) tm_cs( i );

    // SET BUILDINGS TILEMAPS
    set_tilemap_bitamps_from_spritesheet( 1, &building_graphics[ 0 ] );

    // SET CLOUD TILEMAPS
    for( int i = 0; i < 4; i++ ) {
        set_tilemap_bitmap32x32( 0, 1 + ( i * 4 ), &cloud_graphics[ i * 1024 ] );
    }

    for( int y = 0; y < 32; y++ ) {
        for( int x = 0; x < 63; x++ ) {
            switch( cityscape[y][x] ) {
                case 0:
                case ' ':
                case '@':
                    break;
                case '#':
                    set_tilemap_tile_abs( 3, x, y+1, 62, 0 );
                    break;
                case '*':
                    set_tilemap_tile_abs( 3, x, y+1, 63, 0 );
                    break;
                default:
                    if( cityscape[y][x] > 'Z' ) {
                        // LOWER CASE - REFLECTION
                        set_tilemap_tile_abs( 3, x, y+1, cityscape[y][x] - 96, REFLECT_X );
                    } else {
                        // UPPER CASE - NO REFLECTION
                        set_tilemap_tile_abs( 3, x, y+1, cityscape[y][x] - 64, 0 );
                    }
                    break;
            }
            switch( backdrop[y][x] ) {
                case 0:
                case ' ':
                case '@':
                    break;
                case '#':
                    set_tilemap_tile_abs( 2, x, y+1, 62, 0 );
                    break;
                case '*':
                    set_tilemap_tile_abs( 2, x, y+1, 63, 0 );
                    break;
                default:
                    if( backdrop[y][x] > 'Z' ) {
                        // LOWER CASE - BIG TREE
                        set_tilemap_tile_abs( 2, x, y+1, backdrop[y][x] - 96 + 35, 0 );
                    } else {
                        // UPPER CASE - SMALL TREE
                        set_tilemap_tile_abs( 2, x, y+1, backdrop[y][x] - 64 + 25, 0 );
                    }
                    break;
            }
        }
    }

    // DRAW THE CLOUD
    set_tilemap_32x32tile_abs( 0, 17, 5, 1 );
    set_tilemap_32x32tile_abs( 0, 17, 7, 5 );
    set_tilemap_32x32tile_abs( 0, 19, 5, 9 );
    set_tilemap_32x32tile_abs( 0, 19, 7, 13 );
}


// DRAW THE TRAFFIC IN FRONT OF THE CITY ( use sprites 16 - 31, sprite layer 1 ) and BEHIND THE CITY ( uses sprites 0 - 15, sprite layer 0 )
#define SPRITE_YPOS 280
unsigned char updateflags[] = { 31, 31, 31, 30, 30, 29, 29, 3, 3, 2, 2, 1, 1, 1 };
void set_sprites() {
    for( int i = 0; i < 8; i++ ) {
        set_sprite_bitmaps( i * 2, &traffic_sprites[0] );
        set_sprite_bitmaps( i * 2 + 16, &traffic_sprites[0] );
    }

    unsigned short xpos[] = { 48, 64, 80, 48, 64, 48, 64, 64, 48, 64, 48, 80, 64, 48 };
    unsigned char tiles[] = { 5, 6, 7, 3, 4, 1, 2, 1, 2, 3, 4, 5, 6, 7 };

    for( int i = 0; i < 7; i++ ) {
        set_sprite( i, 1, xpos[i] + 32, SPRITE_YPOS - 32, tiles[i], 0 );
        set_sprite( i + 7, 1, xpos[ i + 7 ] + 32, SPRITE_YPOS - 16, tiles[ i + 7 ], REFLECT_X );

        set_sprite( i + 16, 1, xpos[i], SPRITE_YPOS, tiles[i], 0 );
        set_sprite( i + 16 + 7, 1, xpos[ i + 7 ], SPRITE_YPOS + 16, tiles[ i + 7 ], REFLECT_X );
    }
}

// https://www.spriters-resource.com/fullview/25458/
unsigned char car_spritesheet[] = {
    #include "graphics/car-spritesheet.h"
};

#define FERRARI_y 400
// DRAW THE FERRARI IN FRONT OF EVERYTHING ( use sprites 48 - 63, sprite layer 3 )
void set_car_sprites( void ) {
    set_sprite_bitamps_from_spritesheet( 48, 8, car_spritesheet, TRUE );

    set_sprite( 48, 0, 320-64, FERRARI_y, 0, SPRITE_DOUBLE | SPRITE_32X32 );                                        // LEVEL
    set_sprite( 49, 0, 320, FERRARI_y, 1, SPRITE_DOUBLE | SPRITE_32X32 );
    set_sprite( 50, 0, 320-64, FERRARI_y, 0, SPRITE_DOUBLE | SPRITE_32X32 );
    set_sprite( 51, 0, 320, FERRARI_y, 1, SPRITE_DOUBLE | SPRITE_32X32 );
    set_sprite( 52, 0, 320-64, FERRARI_y, 0, SPRITE_DOUBLE | SPRITE_32X32 );
    set_sprite( 53, 0, 320, FERRARI_y, 1, SPRITE_DOUBLE | SPRITE_32X32 );
}

// ROADSIDE ITEMS - AS DRAWLISTS FOR EASIER PLACEMENT AND SCALING
#define NONE 0
#define TREE 1
#define SIGN 2
#define BILLBOARD 3
#define BILLBOARD1 4
#define BILLBOARD2 5
#define BILLBOARD3 6
#define BILLBOARD4 7
#define BEAMS 8
#define TALLBEAM 9
#define SMALLBEAM 10

struct DrawList2D LEFTCHEVRON[] = {
    { DLRECT, GREY3, DITHERSOLID, { -4, 0 }, { 4, -32 }, },
    { DLRECT, BLACK, DITHERSOLID, { -32, -32 }, { 32, -64 }, },
    { DLQUAD, WHITE, DITHERSOLID, { -16, -48 }, { 0, -48 }, { 8, -32 }, { -8, -32 } },
    { DLQUAD, WHITE, DITHERSOLID, { -8, -64 }, { 8, -64 }, { 0, -48 }, { -16, -48 } },
};
struct DrawList2D RIGHTCHEVRON[] = {
    { DLRECT, GREY3, DITHERSOLID, { -4, 0 }, { 4, -32 }, },
    { DLRECT, BLACK, DITHERSOLID, { -32, -32 }, { 32, -64 }, },
    { DLQUAD, WHITE, DITHERSOLID, { 0, -48 }, { 16, -48 }, { 8, -32 }, { -8, -32 } },
    { DLQUAD, WHITE, DITHERSOLID, { -8, -64 }, { 8, -64 }, { 16, -48 }, { 0, -48 } },
};

struct DrawList2D PINETREE[] = {
    { DLRECT, BROWN, DKBROWN, DITHERCHECK1, { -8, 0 }, { 8, -32 }, },
    { DLTRI, FORESTGREEN, DITHERSOLID, { 0, -96 }, { 32, -32 }, { -32, -32 } },
};

struct DrawList2D LEFTBEAM[] = {
    { DLRECT, STEELBLUE, DITHERSOLID, { -4, 0 }, { 4, -128 } },
    { DLRECT, STEELBLUE, DITHERSOLID, { -36, 0 }, { -28, -128 } },
    { DLRECT, STEELBLUE, DITHERSOLID, { -36, -124 }, { 160, -132 } },
    { DLLINE, DKRED, DITHERSOLID, { 0, -128 }, { -32, -96 }, { 5, 0 } },
    { DLLINE, DKRED, DITHERSOLID, { -32, -128 }, { 0, -96 }, { 5, 0 } },
    { DLLINE, DKRED, DITHERSOLID, { 0, -64 }, { -32, -32 }, { 5, 0 } },
    { DLLINE, DKRED, DITHERSOLID, { -32, -64 }, { 0, -32 }, { 5, 0 } },
    { DLRECT, BLACK, DITHERSOLID, { 112, -136}, { 96, -120 }, },
    { DLTRI, GREEN, DITHERSOLID, { 95, -128 }, { 111, -128 }, { 103, -121 } },
    { DLRECT, GREEN, DITHERSOLID, { 101, -128 }, { 105, -135 } }
};

struct DrawList2D LEFTBEAMSMALL[] = {
    { DLRECT, STEELBLUE, DITHERSOLID, { -4, 0 }, { 4, -32 } },
};

struct DrawList2D RIGHTBEAM[] = {
    { DLRECT, BLACK, DITHERSOLID, { -112, -136 }, { -96, -120 }, },
    { DLRECT, STEELBLUE, DITHERSOLID, { -4, 0 }, { 4, -128 } },
    { DLRECT, STEELBLUE, DITHERSOLID, { 28, 0 }, { 36, -128 } },
    { DLRECT, STEELBLUE, DITHERSOLID, { -160, -124 }, { 36, -132 } },
    { DLLINE, DKRED, DITHERSOLID, { 0, -128 }, { 32, -96 }, { 5, 0 } },
    { DLLINE, DKRED, DITHERSOLID, { 32, -128 }, { 0, -96 }, { 5, 0 } },
    { DLLINE, DKRED, DITHERSOLID, { 0, -64 }, { 32, -32 }, { 5, 0 } },
    { DLLINE, DKRED, DITHERSOLID, { 32, -64 }, { 0, -32 }, { 5, 0 } },
};

struct DrawList2D RIGHTBEAMSMALL[] = {
    { DLRECT, STEELBLUE, DITHERSOLID, { -4, 0 }, { 4, -32 } },
};

// BILLBOARD - 4 DESIGNS, TO BE DISPLAYED IN ORDER
struct DrawList2D LEFTBILLBOARD1[] = {
    { DLRECT, GREY6, DITHERSOLID, { 0, 0 }, { -128, -96 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, 0 }, { 0, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, -96 }, { -128, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { -128, -96 }, { -128, 0 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { -128, 0 }, { 0, 0 }, { 3, 0 }, },
    { DLCIRC, YELLOW, DITHERSOLID, { -92, -48 }, { 32, 0b11111001 }, },
    { DLCIRC, WHITE, DITHERSOLID, { -60, -48 }, { 8, 0xff }, }
};
struct DrawList2D LEFTBILLBOARD2[] = {
    { DLRECT, GREY6, DITHERSOLID, { 0, 0 }, { -128, -96 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, 0 }, { 0, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, -96 }, { -128, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { -128, -96 }, { -128, 0 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { -128, 0 }, { 0, 0 }, { 3, 0 }, },
    { DLRECT, RED, DITHERSOLID, { -40, -30 }, { -88, -66 }, },
    { DLCIRC, RED, DITHERSOLID, { -64, -66 }, { 24, 0b11000011 }, },
    { DLCIRC, RED, DITHERSOLID, { -80, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, RED, DITHERSOLID, { -64, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, RED, DITHERSOLID, { -48, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, WHITE, DITHERSOLID, { -80, -56 }, { 8, 0xff }, },
    { DLCIRC, BLUE, DITHERSOLID, { -80, -56 }, { 4, 0xff }, },
    { DLCIRC, WHITE, DITHERSOLID, { -48, -56 }, { 8, 0xff }, },
    { DLCIRC, BLUE, DITHERSOLID, { -48, -56 }, { 4, 0xff }, },
};
struct DrawList2D LEFTBILLBOARD3[] = {
    { DLRECT, GREY6, DITHERSOLID, { 0, 0 }, { -128, -96 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, 0 }, { 0, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, -96 }, { -128, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { -128, -96 }, { -128, 0 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { -128, 0 }, { 0, 0 }, { 3, 0 }, },
    { DLCIRC, YELLOW, DITHERSOLID, { -60, -48 }, { 32, 0xff }, },
};
struct DrawList2D LEFTBILLBOARD4[] = {
    { DLRECT, GREY6, DITHERSOLID, { 0, 0 }, { -128, -96 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, 0 }, { 0, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, -96 }, { -128, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { -128, -96 }, { -128, 0 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { -128, 0 }, { 0, 0 }, { 3, 0 }, },
    { DLRECT, DKBLUE, DITHERSOLID, { -40, -30 }, { -88, -66 }, },
    { DLCIRC, DKBLUE, DITHERSOLID, { -64, -66 }, { 24, 0b11000011 }, },
    { DLCIRC, DKBLUE, DITHERSOLID, { -80, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, DKBLUE, DITHERSOLID, { -64, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, DKBLUE, DITHERSOLID, { -48, -30 }, { 8, 0b00111100 }, },
    { DLARC, PINK, DITHERSOLID, { -80, -39 }, { 4, 0b11000011 }, },
    { DLARC, PINK, DITHERSOLID, { -72, -39 }, { 4, 0b00111100 }, },
    { DLARC, PINK, DITHERSOLID, { -64, -39 }, { 4, 0b11000011 }, },
    { DLARC, PINK, DITHERSOLID, { -56, -39 }, { 4, 0b00111100 }, },
    { DLARC, PINK, DITHERSOLID, { -48, -39 }, { 4, 0b11000011 }, },
    { DLRECT, PINK, DITHERSOLID, { -84, -52 }, { -76, -60 }, },
    { DLRECT, PINK, DITHERSOLID, { -52, -52 }, { -44, -60 }, },
};

struct DrawList2D RIGHTBILLBOARD1[] = {
    { DLRECT, GREY6, DITHERSOLID, { 0, 0 }, { 128, -96 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, 0 }, { 0, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, -96 }, { 128, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 128, -96 }, { 128, 0 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 128, 0 }, { 0, 0 }, { 3, 0 }, },
    { DLCIRC, YELLOW, DITHERSOLID, { 92, -48 }, { 32, 0b10011111 }, },
    { DLCIRC, WHITE, DITHERSOLID, { 60, -48 }, { 8, 0xff }, }
};
struct DrawList2D RIGHTBILLBOARD2[] = {
    { DLRECT, GREY6, DITHERSOLID, { 0, 0 }, { 128, -96 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, 0 }, { 0, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, -96 }, { 128, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 128, -96 }, { 128, 0 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 128, 0 }, { 0, 0 }, { 3, 0 }, },
    { DLRECT, PEACH, DITHERSOLID, { 40, -30 }, { 88, -66 }, },
    { DLCIRC, PEACH, DITHERSOLID, { 64, -66 }, { 24, 0b11000011 }, },
    { DLCIRC, PEACH, DITHERSOLID, { 80, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, PEACH, DITHERSOLID, { 64, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, PEACH, DITHERSOLID, { 48, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, WHITE, DITHERSOLID, { 80, -56 }, { 8, 0xff }, },
    { DLCIRC, BLUE, DITHERSOLID, { 80, -56 }, { 4, 0xff }, },
    { DLCIRC, WHITE, DITHERSOLID, { 48, -56 }, { 8, 0xff }, },
    { DLCIRC, BLUE, DITHERSOLID, { 48, -56 }, { 4, 0xff }, },
};
struct DrawList2D RIGHTBILLBOARD3[] = {
    { DLRECT, GREY6, DITHERSOLID, { 0, 0 }, { 128, -96 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, 0 }, { 0, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, -96 }, { 128, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 128, -96 }, { 128, 0 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 128, 0 }, { 0, 0 }, { 3, 0 }, },
    { DLCIRC, YELLOW, DITHERSOLID, { 60, -48 }, { 32, 0xff }, }
};
struct DrawList2D RIGHTBILLBOARD4[] = {
    { DLRECT, GREY6, DITHERSOLID, { 0, 0 }, { 128, -96 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, 0 }, { 0, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 0, -96 }, { 128, -96 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 128, -96 }, { 128, 0 }, { 3, 0 }, },
    { DLLINE, BLUE3, DITHERSOLID, { 128, 0 }, { 0, 0 }, { 3, 0 }, },
    { DLRECT, DKBLUE, DITHERSOLID, { 40, -30 }, { 88, -66 }, },
    { DLCIRC, DKBLUE, DITHERSOLID, { 64, -66 }, { 24, 0b11000011 }, },
    { DLCIRC, DKBLUE, DITHERSOLID, { 80, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, DKBLUE, DITHERSOLID, { 64, -30 }, { 8, 0b00111100 }, },
    { DLCIRC, DKBLUE, DITHERSOLID, { 48, -30 }, { 8, 0b00111100 }, },
    { DLARC, PINK, DITHERSOLID, { 80, -39 }, { 4, 0b11000011 }, },
    { DLARC, PINK, DITHERSOLID, { 72, -39 }, { 4, 0b00111100 }, },
    { DLARC, PINK, DITHERSOLID, { 64, -39 }, { 4, 0b11000011 }, },
    { DLARC, PINK, DITHERSOLID, { 56, -39 }, { 4, 0b00111100 }, },
    { DLARC, PINK, DITHERSOLID, { 48, -39 }, { 4, 0b11000011 }, },
    { DLRECT, PINK, DITHERSOLID, { 84, -52 }, { 76, -60 }, },
    { DLRECT, PINK, DITHERSOLID, { 52, -52 }, { 44, -60 }, },
};

// ROAD SEGMENTS, DEFINING NUMBER OF SECTIONS BEFORE NEXT TURN, TURN ANGLE, AND SIDE OBJECTS
#define MAXSEGMENT 17
typedef struct {
    int     ct;
    float   tu;
    float   pi;
    int     tnl;
    int     bgl;
    int     bgr;
} roadsegment;

roadsegment road[]={
    {10,0,0,0,TREE,TREE},
    {6,-.25,0,0,TREE,SIGN},
    {8,0,-0.75,0,TREE,TREE},

    { 4,0,0,1, NONE, NONE },
    { 8,0,0.75,1, NONE, NONE },
    { 8,-0.50,0.75,1, NONE, NONE },
    { 4,0,0,1, NONE, NONE },
    { 8,0.5,0,1, NONE, NONE },
    { 4,0,-0.5,1,NONE, NONE },
    { 8,0,-0.5,1, NONE, NONE },

    {4,.375,0,0,SIGN,TREE},
    {10,0.05,0.75,0,TREE,NONE},
    {4,0,0,0,TREE,TREE},
    {5,-.25,0,0,TREE,SIGN},
    {15,0,-0.5,0,BEAMS,BEAMS},
    {12,0,0,0,BILLBOARD,BILLBOARD},
    {8,-.5,0,0,BILLBOARD,SIGN},
    {8,.5,0,0,SIGN,BILLBOARD}
};
int corner[MAXSEGMENT]; float pitch[MAXSEGMENT], slope[MAXSEGMENT];

// VECTOR HELPERS FOR 2D to 3D PROJECTION
typedef struct { float x,y,z; }   vec3;
typedef struct { int x,y,z; }   ivec3;
typedef struct { float x,y,z,w; } vec4;
typedef struct { int x,y,z,w; } ivec4;

static inline vec3 make_vec3(float x, float y, float z) {
  vec3 V;
  V.x = x; V.y = y; V.z = z;
  return V;
}

static inline vec4 make_vec4(float x, float y, float z, float w ) {
  vec4 V;
  V.x = x; V.y = y; V.z = z; V.w = w;
  return V;
}

// PROJECT - TO HORIZONTAL CENTRE OF THE SCREEN, MOVE SLIGHTLY DOWN FOR VERTICAL CENTRE
vec3 project( float x, float y, float z ) {
    float scale = 120/z;
    return( make_vec3( x * scale + 160, y * scale + 120, scale ) );
}

vec3 skew( float x, float y, float z, float xd, float yd ) {
    return( make_vec3( x+z*xd, y+z*yd, z ) );
}

int iterations = 0;
int camcnr = 0, camseg = 0;
float camx = 0, camy = 0, camz = 0;

void init( void ) {
    int sumct = 0; float pi, nextpi, dpi;
    for( int i = 0; i < MAXSEGMENT; i++ ) {
        corner[i] = sumct; sumct += road[i].ct;
        pi = road[i].pi; nextpi = road[ i%MAXSEGMENT + 1 ].pi;
        pitch[i] = pi; slope[i] = ( nextpi - pi ) / road[i].ct;
    }
}

void advance( int *cnr, int *seg ) {
    *seg = *seg + 1;
    if( *seg > road[ *cnr ].ct ) {
        *seg = 0;
        *cnr = *cnr + 1;
        if( *cnr > MAXSEGMENT ) {
            *cnr = 0;
        }
    }
}

void update() {
    camz += 0.1;
    if( camz > 1 ) {
        camz = 0;
        advance( &camcnr, &camseg );
    }
}

// NUMBER OF SEGMENTS TO DRAW EACH ITERATION
#define DRAWSEGMENTS 28

void drawtrapezium( unsigned char colour, int x1, int y1, int w1, int x2, int y2, int w2 ) {
    if( (((x1-w1)<0) && ((x1+w1)<0) && ((x2+w2)<0) && ((x2-w2)<0)) ||
        (((x1-w1)>319) && ((x1+w1)>319) && ((x2+w2)>319) && ((x2-w2)>319)) ) return;
    gpu_quadrilateral( colour, x1-w1, y1, x1+w1, y1, x2+w2, y2, x2-w2, y2 );
}

void gettunnelrectangle( float px, float py, float scale, int *x1, int *y1, int *x2, int *y2 )  {
    int w = 6.4 * scale, h = 4 * scale;
    *x1 = px - w/2;
    *y1 = py -h;
    *x2 = px + w/2;
    *y2 = py;
}

void drawtunnelface( float px, float py, float scale ) {
    int x1, y1, x2, y2;
    gettunnelrectangle( px, py, scale, &x1, &y1, &x2, &y2 );
    int wh = 4.5 * scale, wy = py - wh;
    gpu_dither( DITHERBRICK, GREY2 );
    if( y1 > 0 ) gpu_rectangle( GREY3, 0, wy, 319, y1 - 1 );
    if( x1 > 0 ) gpu_rectangle( GREY3, 0, y1, x1 -1, y2 - 1 );
    if( x2 < 319 ) gpu_rectangle( GREY3, x2, y1, 319, y2 - 1 );
    gpu_dither( DITHEROFF );
}

void drawroad( float x1, float y1, float scale1, float x2, float y2, float scale2, int sumct, int tnl ) {
    // DRAW GRASS
    if( !tnl ) {
        gpu_dither( DITHERHSTRIPE, (sumct&1) ? GREEN5 : GREEN3 );
        gpu_rectangle( (sumct&1) ? GREEN6 : GREEN4, 0, y1, 319, y2 );
        gpu_dither( DITHEROFF );
    }

    int w1 = 3 * scale1, w2 = 3 * scale2;
    drawtrapezium( tnl ? GREY2 : GREY3, x1, y1, w1, x2, y2, w2 );

    // CENTRE LINE MARKINGS
    if( !(sumct&3) ) {
        int mw1 = .1 * scale1, mw2 = .1 * scale2;
        drawtrapezium( tnl ? GREY5 : WHITE, x1, y1, mw1, x2, y2, mw2 );
    }

    // SHOULDER MARKINGS
    int sw1 = .2 * scale1, sw2 = .2 * scale2;
    drawtrapezium( (sumct&1) ? tnl ? GREY5 : WHITE : tnl ? RED2 : RED, x1-w1, y1, sw1 ,x2-w2, y2, sw2 );
    drawtrapezium( (sumct&1) ? tnl ? GREY5 : WHITE : tnl ? RED2 : RED, x1+w1, y1, sw1, x2+w2, y2, sw2 );
}

void draw() {
    float camang = camz * road[camcnr].tu;
    float xd = -camang, yd = road[camcnr].pi + slope[camcnr]*(camseg-1), zd = 1;
    float x, y, z;

    int cnr = camcnr, seg = camseg, sumct;
    vec3 c, p, pp;

    // SPRIATES TO DRAW, ALONG WITH
    int lsprites[ DRAWSEGMENTS ], rsprites[ DRAWSEGMENTS ];
    vec3 spritesxyz[DRAWSEGMENTS];
    int spriteclip[DRAWSEGMENTS][4];

    // SKEY CAMERA TO ACCOUNT FOR DIRECTION
    c = skew( camx, camy, camz, xd, yd );
    x = -c.x; y =-c.y+2; z = -c.z + 2;

    // CROPPING RECTANGLE
    int crop[4] = { CROPFULLSCREEN }; gpu_crop( CROPFULLSCREEN ); gpu_cs();
    int x1, y1, x2, y2, px1, py1, px2, py2;

    pp = project( x, y, z );

    int ptnl = road[cnr].tnl, tnl;

    // MOVE THE TILEMAPS
    if( road[cnr].tu < 0 ) {
        tilemap_scroll( 0, TM_LEFT, 1 );
        tilemap_scroll( 2, TM_LEFT, ( road[cnr].tu <= -0.5 ) ? 2 : 1 );
        tilemap_scroll( 3, TM_LEFT, ( road[cnr].tu <= -0.5 ) ? 3 : 2 );
    }
    if( road[cnr].tu > 0 ) {
        tilemap_scroll( 0, TM_RIGHT, 1 );
        tilemap_scroll( 2, TM_RIGHT, ( road[cnr].tu >= 0.5 ) ? 2 : 1 );
        tilemap_scroll( 3, TM_RIGHT, ( road[cnr].tu >= 0.5 ) ? 3 : 2 );
    }

    for( int i = 0; i < DRAWSEGMENTS; i++ ) {
        x += xd; y += yd; z += zd;
        p = project( x, y, z );

        tnl = road[cnr].tnl;
        if( tnl && !ptnl ) {
            drawtunnelface( pp.x, pp.y, pp.z );
            gettunnelrectangle( pp.x, pp.y, pp.z, &x1, &y1, &x2, &y2 );
            crop[0] = max( crop[0], x1 );
            crop[1] = max( crop[1], y1 );
            crop[2] = min( crop[2], x2 );
            crop[3] = min( crop[3], y2 );
            gpu_crop( crop[0], crop[1], crop[2], crop[3] );
        }

        sumct = corner[cnr] + seg - 1;
        if( tnl ) {
            unsigned char wallcol = ( sumct&1 ) ? 9 : 10;
            gettunnelrectangle( p.x, p.y, p.z, &x1, &y1, &x2, &y2 );
            gettunnelrectangle( pp.x, pp.y, pp.z, &px2, &py1, &px2, &py2 );
            if( y1 > py1 ) { gpu_dither( DITHERHATCH, 8 ); gpu_rectangle( wallcol, px1, py1, px2-1, y1-1 ); }
            if( x1 > px1 ) { gpu_dither( DITHERRSLOPE, 8 ); gpu_rectangle( wallcol, px1, y1,x1-1,py2-1 ); }
            if( x2 < px2 ) { gpu_dither( DITHERLSLOPE, 8 ); gpu_rectangle( wallcol, x2, y1, px2-1,py2-1 ); }
            gpu_dither( DITHEROFF );
        }
        drawroad( p.x, p.y, p.z, pp.x, pp.y, pp.z, sumct, tnl );

        // ADD BACKGROUND GRAPHICS, BEAMS ONLY AT START OF SECTION, SIGNS EVERY OTHER GAP, BILLBOARDS EVERY 4th GAP
        spriteclip[i][0] = crop[0]; spriteclip[i][1] = crop[1]; spriteclip[i][2] = crop[2]; spriteclip[i][3] = crop[3]; spritesxyz[i] = p;
        lsprites[i] = NONE; rsprites[i] = NONE;
        switch( road[cnr].bgl ) {
            case BEAMS:
                lsprites[ i ] = ( !seg ) ? TALLBEAM : SMALLBEAM;
                break;
            case SIGN:
                if( seg & 1 ) {
                   lsprites[ i ] = road[cnr].bgl;
                }
                break;
            case BILLBOARD:
                if( !( seg & 3 ) ) {
                    lsprites[ i ] = BILLBOARD1 + ((seg&12)>>2);
                }
                break;
            default:
                lsprites[ i ] = road[cnr].bgl;
        }
        switch( road[cnr].bgr ) {
            case BEAMS:
                rsprites[ i ] = ( !seg ) ? TALLBEAM : SMALLBEAM;
                break;
            case SIGN:
                if( seg & 1 ) {
                   rsprites[ i ] = road[cnr].bgr;
                }
                break;
            case BILLBOARD:
                if( !( seg & 3 ) ) {
                    rsprites[ i ] = BILLBOARD1 + ((seg&12)>>2);
                }
                break;
            default:
                rsprites[ i ] = road[cnr].bgr;
        }

        xd += road[cnr].tu; yd += slope[cnr];
        advance( &cnr, &seg );
        pp = p;
        ptnl = tnl;

        if( tnl ) {
            gettunnelrectangle( p.x, p.y, p.z, &x1, &y1, &x2, &y2 );
            crop[0] = max( crop[0], x1 );
            crop[1] = max( crop[1], y1 );
            crop[2] = min( crop[2], x2 );
            crop[3] = min( crop[3], y2 );
        } else {
            crop[3] = min( crop[3], p.y );
        }
        gpu_crop( crop[0], crop[1], crop[2], crop[3] );
    }

    for( int i = DRAWSEGMENTS -1; i >= 0; i-- ) {
        // DRAW SPRITES IN REVERSE ORDER
        float scale = spritesxyz[i].z / 36, offset = 3 * spritesxyz[i].z + ( 32 * scale );
        gpu_crop( spriteclip[i][0], spriteclip[i][1], spriteclip[i][2], spriteclip[i][3] );
        switch( lsprites[i] ) {
            case TREE:
                DoDrawList2Dscale( PINETREE, 2, spritesxyz[i].x - offset, spritesxyz[i].y, scale );
                break;
            case SIGN:
                DoDrawList2Dscale( RIGHTCHEVRON, 4, spritesxyz[i].x - offset, spritesxyz[i].y, scale );
                break;
            case TALLBEAM:
                DoDrawList2Dscale( LEFTBEAM, 10, spritesxyz[i].x - offset, spritesxyz[i].y, scale );
                break;
            case SMALLBEAM:
                    DoDrawList2Dscale( LEFTBEAMSMALL, 1, spritesxyz[i].x - offset, spritesxyz[i].y, scale );
                 break;
            case BILLBOARD1:
                DoDrawList2Dscale( LEFTBILLBOARD1, 7, spritesxyz[i].x - offset, spritesxyz[i].y, scale );
                break;
            case BILLBOARD2:
                DoDrawList2Dscale( LEFTBILLBOARD2, 14, spritesxyz[i].x - offset, spritesxyz[i].y, scale );
                break;
            case BILLBOARD3:
                DoDrawList2Dscale( LEFTBILLBOARD3, 6, spritesxyz[i].x - offset, spritesxyz[i].y, scale );
                break;
            case BILLBOARD4:
                DoDrawList2Dscale( LEFTBILLBOARD4, 17, spritesxyz[i].x - offset, spritesxyz[i].y, scale );
                break;
            default:
        }
        switch( rsprites[i] ) {
            case TREE:
                DoDrawList2Dscale( PINETREE, 2, spritesxyz[i].x + offset, spritesxyz[i].y, scale );
                break;
            case SIGN:
                DoDrawList2Dscale( LEFTCHEVRON, 4, spritesxyz[i].x + offset, spritesxyz[i].y, scale );
                break;
            case TALLBEAM:
                DoDrawList2Dscale( RIGHTBEAM, 8, spritesxyz[i].x + offset, spritesxyz[i].y, scale );
                break;
            case SMALLBEAM:
                DoDrawList2Dscale( RIGHTBEAMSMALL, 1, spritesxyz[i].x + offset, spritesxyz[i].y, scale );
                break;
            case BILLBOARD1:
                DoDrawList2Dscale( RIGHTBILLBOARD1, 7, spritesxyz[i].x + offset, spritesxyz[i].y, scale );
                break;
            case BILLBOARD2:
                DoDrawList2Dscale( RIGHTBILLBOARD2, 14, spritesxyz[i].x + offset, spritesxyz[i].y, scale );
                break;
            case BILLBOARD3:
                DoDrawList2Dscale( RIGHTBILLBOARD3, 6, spritesxyz[i].x + offset, spritesxyz[i].y, scale );
                break;
            case BILLBOARD4:
                DoDrawList2Dscale( RIGHTBILLBOARD4, 17, spritesxyz[i].x + offset, spritesxyz[i].y, scale );
                break;
            default:
        }
    }
}

void set_background_generator( void ) {
    set_background( DKBLUE, BLUE, BKG_HATCH );
}

void __attribute__((interrupt ("machine"))) move_sprites() {
    IRQ_ACK( IRQ_VBLANK );
    for( int i = 0; i < 14; i++ ) {
        update_sprite_compat( i, updateflags[i] );
        update_sprite_compat( 16 + i, updateflags[i] );
    }
}

int main( int argc, char **argv ) {
    unsigned char dimmerlevel = 8, counter = 3;

    // Choose Music
    screen_order( LAYER_CHARACTERMAP, LAYER_BITMAP_0, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE );
    tpu_cs();
    tpu_print_centre( 4, TRANSPARENT, WHITE,  TPU_BOLD + TPU_Y2, "Music Converted From https://archive.org/details/OutRunGEN" );
    tpu_print_centre( 6, TRANSPARENT, WHITE,  TPU_BOLD + TPU_X2, "8 bit Mono @ 11025 KHz WAV" );

    passing_breeze = sdcard_selectfile( "Please select Music", "WAV", &music_size, "Loading" );
    if( music_size )
        pcmsample_start( 1, music_size - 544, &passing_breeze[78], KHz11025, TRUE );

    // SETUP SCREEN
    screen_dimmer( dimmerlevel );
    screen_order( LAYER_CHARACTERMAP, LAYER_SPRITES_3, LAYER_BITMAP_0, LAYER_SPRITES_1, LAYER_TILEMAP_3, LAYER_SPRITES_0, LAYER_TILEMAP_2, LAYER_TILEMAP_0, FALSE, FALSE, FALSE );

    tpu_cs();
    tpu_print_centre( 2, TRANSPARENT, WHITE,  TPU_BOLD + TPU_Y2 + TPU_BLINK, "Based upon https://www.lexaloffle.com/bbs/?tid=35767" );
    tpu_print_centre( 4, TRANSPARENT, WHITE,  TPU_BOLD + TPU_X2 + TPU_BLINK, "Written by @tommulgrew" );

    bitmap_draw( 3 ); gpu_cs();
    set_background_generator();
    set_tilemaps();
    set_sprites();
    set_car_sprites();

    // INTERRUPT HANDLER SETUP
    IRQ_VECTOR( (void *)move_sprites ); IRQ_ON( IRQ_VBLANK, TRUE );

    // PREPARE ROAD
    init();

    unsigned char framebuffer = 1;
    while( !( get_buttons() & 4 ) ) {
        bitmap_draw( 3 - framebuffer ); draw(); update();

        framebuffer = 3 - framebuffer; await_vblank();
        screen_order( LAYER_CHARACTERMAP, LAYER_SPRITES_3, framebuffer, LAYER_SPRITES_1, LAYER_TILEMAP_3, LAYER_SPRITES_0, LAYER_TILEMAP_2, LAYER_TILEMAP_0, FALSE, FALSE, FALSE );

        // SET FERRARI SPRITE
        set_sprite_attribute( 48, ATTR_SPRITE_ACTIVE, road[camcnr].pi == 0 );
        set_sprite_attribute( 49, ATTR_SPRITE_ACTIVE, road[camcnr].pi == 0 );
        set_sprite_attribute( 50, ATTR_SPRITE_ACTIVE, road[camcnr].pi < 0 );
        set_sprite_attribute( 51, ATTR_SPRITE_ACTIVE, road[camcnr].pi < 0 );
        set_sprite_attribute( 52, ATTR_SPRITE_ACTIVE, road[camcnr].pi > 0 );
        set_sprite_attribute( 53, ATTR_SPRITE_ACTIVE, road[camcnr].pi > 0 );

        if( dimmerlevel ) {
            if( !--counter ) { screen_dimmer( --dimmerlevel ); counter = 3; }
        }
    }
}
