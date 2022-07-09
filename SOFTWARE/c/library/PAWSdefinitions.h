#include <stdbool.h>

// STANDARD CONSTANTS
#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
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

// BACKGROUND COPPER COMMANDS
#define COPPER_JUMP 0
#define COPPER_JUMP_ALWAYS 0
#define COPPER_JUMP_ON_VBLANK_EQUAL 1
#define COPPER_JUMP_ON_HBLANK_EQUAL 2
#define COPPER_JUMP_IF_Y_LESS 3
#define COPPER_JUMP_IF_X_LESS 4
#define COPPER_JUMP_IF_VARIABLE_LESS 5
#define COPPER_VARIABLE 6
#define COPPER_SET 7
#define COPPER_WAIT_VBLANK 1
#define COPPER_WAIT_HBLANK 2
#define COPPER_WAIT_Y 3
#define COPPER_WAIT_X 4
#define COPPER_WAIT_VARIABLE 5
#define COPPER_VARIABLE 6
#define COPPER_SET_VARIABLE 1
#define COPPER_ADD_VARIABLE 2
#define COPPER_SUB_VARIABLE 4
#define COPPER_SET_FROM_VARIABLE 7
#define COPPER_USE_CPU_INPUT 0x400

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

// DOUBLE SIZE OF SPRITE ( IN ADDITION TO THE FLAGS BELOW )
#define SPRITE_DOUBLE 8

// FOR TILEMAP, BLITTERS, SPRITES AND VECTOR BLOCK
#define REFLECT_X 1
#define REFLECT_Y 2
#define ROTATE0 4
#define ROTATE90 5
#define ROTATE180 6
#define ROTATE270 7

#define TM_LOWER_DOUBLE 1
#define TM_UPPER_DOUBLE 2

// TILEMAP DIRECTION FLAGS
#define TM_LEFT 1
#define TM_UP 2
#define TM_RIGHT 3
#define TM_DOWN 4
#define TM_CLEAR 5,0

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
#define CHANNEL_LEFT 1
#define CHANNEL_RIGHT 2
#define CHANNEL_BOTH 3
#define WAVE_SQUARE 0
#define WAVE_SAW 1
#define WAVE_TRIANGLE 2
#define WAVE_SINE 3
#define WAVE_NOISE 4
#define WAVE_SAMPLE 7

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
        short dx;
        short dy;
    };
    int packed;
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

// FAT32 File System for the file selector
typedef struct {
    unsigned char first_byte;
    unsigned char start_chs[3];
    unsigned char partition_type;
    unsigned char end_chs[3];
    unsigned int start_sector;
    unsigned int length_sectors;
} __attribute((packed)) PartitionTable;

typedef struct {
    unsigned char   jmp[3];
    unsigned char   oem[8];
    unsigned short  sector_size;
    unsigned char   sectors_per_cluster;
    unsigned short  reserved_sectors;
    unsigned char   number_of_fats;
    unsigned short  root_dir_entries;
    unsigned short  total_sectors_short; // if zero, later field is used
    unsigned char   media_descriptor;
    unsigned short  fat16_size_sectors;
    unsigned short  sectors_per_track;
    unsigned short  number_of_heads;
    unsigned int    hidden_sectors;
    unsigned int    total_sectors_long;
    unsigned int    fat32_size_sectors;
    unsigned short  fat32_flags;
    unsigned short  version;
    unsigned int    startof_root;
    unsigned short  filesystem_information;
    unsigned short  backupboot_sector;
    unsigned char   reserved[12];
    unsigned char   logical_drive_number;
    unsigned char   unused;
    unsigned char   extended_signature;
    unsigned int    volume_id;
    char            volume_label[11];
    char            fs_type[8];
    char            boot_code[420];
    unsigned short  boot_sector_signature;
} __attribute((packed)) Fat32VolumeID;

typedef struct {
    unsigned char   filename[8];
    unsigned char   ext[3];
    unsigned char   attributes;
    unsigned char   reserved[8];
    unsigned short  starting_cluster_high;
    unsigned short  modify_time;
    unsigned short  modify_date;
    unsigned short  starting_cluster_low;
    unsigned int    file_size;
} __attribute((packed)) FAT32DirectoryEntry;

typedef struct {
    unsigned char   filename[8];
    unsigned char   ext[3];
    unsigned char   type;
    unsigned int    starting_cluster;
    unsigned int    file_size;
} __attribute((packed)) DirectoryEntry;

// PAWS SYSTEMCLOCK
typedef struct paws_timeval {
    unsigned short  ptv_sec;
    unsigned int    ptv_usec;
} paws_timeval;

// SIMPLE CURSES
typedef unsigned char chtype;
typedef void WINDOW;

#define COLORS 256
#define A_NOACTION 2048
#define A_NORMAL 256
#define A_BOLD 512
#define A_STANDOUT 512
#define A_UNDERLINE A_NOACTION
#define A_REVERSE 1024
#define A_BLINK A_NOACTION
#define A_DIM A_NORMAL
#define A_PROTECT A_NOACTION
#define A_INVIS A_NOACTION
#define A_ALTCHARSET A_NOACTION
#define A_CHARTEXT A_NOACTION
#define COLOR_PAIRS 256
#define COLOR_PAIRS_MASK (COLOR_PAIRS-1)
#define COLOR_PAIR(a) a|COLORS

// COLOURS
#define COLOR_BLACK BLACK
#define COLOR_BLUE BLUE
#define COLOR_GREEN GREEN
#define COLOR_CYAN CYAN
#define COLOR_RED RED
#define COLOR_MAGENTA MAGENTA
#define COLOR_YELLOW YELLOW
#define COLOR_WHITE WHITE
#define COLOR_ORANGE ORANGE

#define COLS 80
#define LINES 60

// MIN MAX MACROS
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
