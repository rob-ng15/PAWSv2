// STANDARD CONSTANTS
#ifndef NULL
#define NULL 0
#endif

#define TRUE 1
#define FALSE 0

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

// COLOURS
#define TRANSPARENT 0x40
#define BLACK 0x00
#define VDKBLUE 0x01
#define DKBLUE 0x02
#define BLUE 0x03
#define LTBLUE 0x07
#define VDKGREEN 0x04
#define DKGREEN 0x08
#define GREEN 0x0c
#define LTGREEN 0x1d
#define DKCYAN 0x0b
#define CYAN 0x0f
#define LTCYAN 0x1f
#define DKRED 0x20
#define RED 0x30
#define LTRED 0x35
#define DKMAGENTA 0x22
#define MAGENTA 0x33
#define LTMAGENTA 0x37
#define DKPURPLE 0x11
#define PURPLE 0x13
#define LTPURPLE 0x17
#define DKYELLOW 0x28
#define YELLOW 0x3c
#define LTYELLOW 0x3d
#define DKORANGE 0x34
#define ORANGE 0x38
#define LTORANGE 0x39
#define DKBROWN 0x10
#define PEACH 0x3a
#define DKGREY 0x15
#define LTGREY 0x2a
#define WHITE 0x3f

// GRADIENTS
#define GREY10 0x40
#define GREY20 0x41
#define GREY30 0x42
#define GREY40 0x43
#define GREY50 0x44
#define GREY60 0x45
#define GREY70 0x46
#define GREY80 0x47

#define RED10 0x48
#define RED20 0x49
#define RED30 0x4a
#define RED40 0x4b
#define RED50 0x4c
#define RED60 0x4d
#define RED70 0x4e
#define RED80 0x4f

#define GREEN10 0x50
#define GREEN20 0x51
#define GREEN30 0x52
#define GREEN40 0x53
#define GREEN50 0x54
#define GREEN60 0x55
#define GREEN70 0x56
#define GREEN80 0x57

#define BLUE10 0x58
#define BLUE20 0x59
#define BLUE30 0x5a
#define BLUE40 0x5b
#define BLUE50 0x5c
#define BLUE60 0x5d
#define BLUE70 0x5e
#define BLUE80 0x5f

#define YELLOW10 0x60
#define YELLOW20 0x61
#define YELLOW30 0x62
#define YELLOW40 0x63
#define YELLOW50 0x64
#define YELLOW60 0x65
#define YELLOW70 0x66
#define YELLOW80 0x67

#define MAGENTA10 0x68
#define MAGENTA20 0x69
#define MAGENTA30 0x6a
#define MAGENTA40 0x6b
#define MAGENTA50 0x6c
#define MAGENTA60 0x6d
#define MAGENTA70 0x6e
#define MAGENTA80 0x6f

#define CYAN10 0x78
#define CYAN20 0x79
#define CYAN30 0x7a
#define CYAN40 0x7b
#define CYAN50 0x7c
#define CYAN60 0x7d
#define CYAN70 0x7e
#define CYAN80 0x7f

// SPECIAL COLOUR PALETTE
#define BROWN 0x78
#define GOLD 0x79
#define FORESTGREEN 0x7a
#define STEELBLUE 0x7b
#define VIOLET 0x7c
#define PINK 0x7d
#define SKYBLUE 0x7e
#define AQUAMARINE 0x7f

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

// TILEMAP DIRECTION FLAGS
#define TM_SCROLL_LEFT 1
#define TM_SCROLL_UP 2
#define TM_SCROLL_RIGHT 3
#define TM_SCROLL_DOWN 4
#define TM_WRAP_LEFT 5
#define TM_WRAP_UP 6
#define TM_WRAP_RIGHT 7
#define TM_WRAP_DOWN 8
#define TM_CLEAR 9

// CROP RECTANGLE
#define CROPFULLSCREEN 0,0,319,239

// KEYBOARD MODE
#define PS2_KEYBOARD 1
#define PS2_JOYSTICK 0

// FONT ATTRIBUTES
#define BOLD 1
#define NORMAL 0

// SCREEN MODES
#define MODE_COLOUR 0
#define MODE_RGB 1
#define MODE_PAWSv1 2
#define MODE_GREY 3

// FOR EASE OF PORTING
typedef unsigned int size_t;
typedef unsigned short bool;
typedef unsigned char   uint8, uint8_t;
typedef unsigned short  uint16, uint16_t;
typedef unsigned int    uint32;
typedef signed char     int8, int8_t;
typedef signed short    int16, int16_t;
typedef signed int      int32;

#define bool_t unsigned char
#define int_t int
#define real_t float

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
struct Point2D {
    short dx;
    short dy;
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
    struct Point2D  xy1;                // Vertex 1 or centre of circle
    struct Point2D  xy2;                // Vertex 2 or circle radius and sector mask
    struct Point2D  xy3;                // Vertex 3 or line width
    struct Point2D  xy4;                // Vertex 4
};

// FAT32 File System
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
    unsigned short  flags;
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

// SIMPLE CURSES
#define COLORS 64
#define A_NOACTION 1024
#define A_NORMAL 128
#define A_BOLD 256
#define A_STANDOUT 256
#define A_UNDERLINE A_NOACTION
#define A_REVERSE 512
#define A_BLINK A_NOACTION
#define A_DIM A_NORMAL
#define A_PROTECT A_NOACTION
#define A_INVIS A_NOACTION
#define A_ALTCHARSET A_NOACTION
#define A_CHARTEXT A_NOACTION
#define COLOR_PAIRS 64
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
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
