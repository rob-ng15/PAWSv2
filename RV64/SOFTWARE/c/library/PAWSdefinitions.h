#include <stdbool.h>

// STANDARD CONSTANTS
#ifndef NULL
#define NULL 0
#endif

#ifndef Bool
#define Bool int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef True
#define True 1
#endif

#ifndef False
#define False 0
#endif

// DISPLAY LAYERS
#define LOWER_LAYER 0
#define UPPER_LAYER 1

// BACKGROUND PATTERN GENERATOR
#define BKG_SOLID 0
#define BKG_5050_V 1
#define BKG_5050_H 2
#define BKG_CHKBRD_5 3
#define BKG_RAINBOW 4
#define BKG_SNOW 5
#define BKG_STATIC 6
#define BKG_CHKBRD_1 7
#define BKG_CHKBRD_2 8
#define BKG_CHKBRD_3 9
#define BKG_CHKBRD_4 10
#define BKG_HATCH 11
#define BKG_LSLOPE 12
#define BKG_RSLOPE 13
#define BKG_VSTRIPE 14
#define BKG_HSTRIPE 15

// DMA CONTROLLER
#define DMA_MULTI_TO_SINGLE 1
#define DMA_PIXEL_RGB 2
#define DMA_MEMCPY 3
#define DMA_MEMSET 4
#define DMA_SINGLE_TO_SINGLE 5
#define DMA_ADD 6
#define DMA_PIXELBLOCK 7
#define DMA_MEMCPY_RECTANGLE 8
#define DMA_MEMSET_RECTANGLE 9

// NEW CU BACKGROUND CO-PROCESSOR
// OPCODES, NOTE OPCODE 0 IS MULTI-USE, JPL JSL AND SLI REQUIRE 1 OTHER ARGUMENT, THE LITERAL VALUE, JPx AND JSx REQUIRE 1 OTHER ARGUMENT, THE REGISTER WITH ADDRESS TO JUMP TO
#define CU_JPL 0,0,1
#define CU_JPR 0,0,0
#define CU_JSL 0,1,1
#define CU_JSR 0,0,0
#define CU_RET 0,2,0,0
#define CU_SLI 0,3,1
#define CU_SR0 0,3,0,4
#define CU_SR1 0,3,0,5
#define CU_SR2 0,3,0,6
#define CU_SR3 0,3,0,7
#define CU_LR0 0,4,0,0
#define CU_LR1 0,5,0,0
#define CU_LR2 0,6,0,0
#define CU_LR3 0,7,0,0
#define CU_SET 1
#define CU_ADD 2
#define CU_SUB 3
#define CU_AND 4
#define CU_OR  5
#define CU_XOR 6
#define CU_SHL 7
#define CU_SHR 8
#define CU_SEQ 9
#define CU_SNE 10
#define CU_SLT 11
#define CU_SLE 12
#define CU_RND 13
#define CU_LFM 14
#define CU_STM 15

// REGISTER NAMES { VBLANK, X, Y, RC, R0, R1, R2, R3 } WHEN READING ( RC, R0, R1, R2, R3 ) ALSO WHEN WRITING
#define CU_RB 0
#define CU_RX 1
#define CU_RY 2
#define CU_RC 3
#define CU_R0 4
#define CU_R1 5
#define CU_R2 6
#define CU_R3 7

// BACKGROUND GENERATOR REGISTER NAMES { MODE, COLOUR, ALT COLOUR } REPLACE { VBLANK, X, Y } WHEN WRITING
#define CU_BM 0
#define CU_BC 1
#define CU_BA 2

// REG2 IS REG OR LITERAL
#define CU_RR 0
#define CU_RL 1

// COLOURS - RGBM MODE
#define TRANSPARENT 0x40
#define BLACK 0x00
#define BLUE 0x06
#define GREEN 0x38
#define CYAN 0x3f
#define RED 0xc1
#define MAGENTA 0xc7
#define YELLOW 0xf9
#define WHITE 0xff

#define BLUE1 0x02
#define BLUE2 0x04
#define BLUE3 0x06
#define BLUE4 0x0f

#define GREEN1 0x08
#define GREEN2 0x16
#define GREEN3 0x18
#define GREEN4 0x20
#define GREEN5 0x28
#define GREEN6 0x30
#define GREEN7 0x38

#define RED2 0x80
#define RED3 0xc0

#define GREY1 0x09
#define GREY2 0x52
#define GREY3 0x5b
#define GREY4 0xa4
#define GREY5 0xad
#define GREY6 0xf6

#define INDIGO 0x4d
#define PURPLE 0x86
#define VIOLET 0x87
#define PINK 0xef
#define ORANGE 0xe9

#define DKBLUE 0x04
#define DKGREEN 0x18
#define DKRED 0x80
#define DKPURPLE 0x84
#define DKMAGENTA 0xc6
#define DKYELLOW 0xf0

#define LTBLUE 0x1f

#define DKBROWN 0x49
#define FORESTGREEN 0x58
#define STEELBLUE 0x6f
#define BROWN 0x89
#define PEACH 0xe3

// DITHER MODES
#define DITHEROFF 0, BLACK
#define DITHERSOLID BLACK, 0
#define DITHERCHECK1 1
#define DITHERCHECK2 2
#define DITHERCHECK3 3
#define DITHERVSTRIPE 4
#define DITHERHSTRIPE 5
#define DITHERHATCH 6
#define DITHERLSLOPE 7
#define DITHERRSLOPE 8
#define DITHERLTRIANGLE 9
#define DITHERRTRIANGLE 10
#define DITHERX 11
#define DITHERCROSS 12
#define DITHERBRICK 13
#define DITHER64COLSTATIC 14
#define DITHER2COLSTATIC 15

// SPRITE ATTRIBUTE FLAGS
#define SPRITE_ACTIVE 0
#define SPRITE_TILE 1
#define SPRITE_COLOUR 2
#define SPRITE_X 3
#define SPRITE_Y 4
#define SPRITE_ACTION 5

// SPRITE OTHER LAYER COLLISION FLAGS
#define SPRITE_TO_BITMAP 8
#define SPRITE_TO_LOWER_TILEMAP 4
#define SPRITE_TO_UPPER_TILEMAP 2
#define SPRITE_TO_OTHER_SPRITES 1

// DOUBLE/QUADRUPAL SIZE OF SPRITE ( IN ADDITION TO THE FLAGS BELOW )
#define SPRITE_DOUBLE 8
#define SPRITE_QUAD 16

// FOR TILEMAP, BLITTERS, SPRITES
#define REFLECT_X 1
#define REFLECT_Y 2
#define ROTATE0 4
#define ROTATE90 5
#define ROTATE180 6
#define ROTATE270 7

#define SPRITE_SHOW 1
#define SPRITE_HIDE 0

// TILEMAP DIRECTION FLAGS
#define TM_LEFT 1
#define TM_UP 2
#define TM_RIGHT 3
#define TM_DOWN 4
#define TM_CLEAR 5,0
#define TM_0OFFSET 7,0

// CROP RECTANGLE
#define CROPFULLSCREEN 0,0,319,239

// KEYBOARD MODE
#define PS2_KEYBOARD 1
#define PS2_JOYSTICK 0

// FONT ATTRIBUTES
#define BOLD 1
#define NORMAL 0

// SCREEN MODES
#define MODE_RGBM 0
#define MODE_GREY 1
#define LTM_LOW 1
#define UTM_LOW 2
#define CM_LOW 4

// PIXEL BLOCK MODES
#define PB_REMAP 1
#define PB_WRITEALL 2

// SOUNDS
#define AUDIO_SILENCE 3,0,0,0
#define LEFT_SILENCE 1,0,0,0
#define RIGHT_SILENCE 2,0,0,0
#define CHANNEL_LEFT 1
#define CHANNEL_RIGHT 2
#define CHANNEL_BOTH 3
#define WAVE_SQUARE 0
#define WAVE_SAW 1
#define WAVE_TRIANGLE 2
#define WAVE_SINE 3
#define WAVE_NOISE 4
#define WAVE_USER 5
#define WAVE_PCM 6
#define WAVE_BITS 7
#define WAVE_TUNE 8
#define SAMPLE_REPEAT 16

// STRUCTURE OF THE SPRITE UPDATE FLAG
struct sprite_update_flag {
    unsigned int padding:3;
    unsigned int y_act:1;
    unsigned int x_act:1;
    unsigned int tile_act:1;
    int dy:5;
    int dx:5;
};

// FOR 2D SOFTWARE VECTORS
union Point2D {
    struct {
        int dx;
        int dy;
    };
    long packed;
};

// FOR SOFTWARE DRAWLISTS
#define DLLINE  0
#define DLRECT  1
#define DLCIRC  2
#define DLARC   3
#define DLTRI   4
#define DLQUAD  5
struct DrawList2D {
    unsigned char   shape;              // DLRECT, DLCIRC, DLTRI, DLQUAD are defined
    unsigned char   colour;             // PAWS colour code
    unsigned char   alt_colour;         // PAWS colour code
    unsigned char   dithermode;         // PAWS dithermode
    union Point2D   xy1;                // Vertex 1 or centre of circle
    union Point2D   xy2;                // Vertex 2 or circle radius and sector mask
    union Point2D   xy3;                // Vertex 3 or line width
    union Point2D   xy4;                // Vertex 4
};

// PAWS SYSTEMCLOCK
typedef struct paws_timeval {
    unsigned short  ptv_sec;
    unsigned int    ptv_usec;
} paws_timeval;

// PIXELBLOCK SCALEABLE SPRITES
typedef struct {
    int width;
    int height;
    unsigned char *bitmap;
} bitmap_sprite;

// PACK RGB MACRO
#define PACKRGB(r,g,b) _rv64_packw( _rv64_packh( b, g ), _rv64_packh( r, 0 ) )

// MIN MAX MACROS
#ifdef __cplusplus
#else
#ifndef min
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#endif
#ifndef max
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#endif
#endif
