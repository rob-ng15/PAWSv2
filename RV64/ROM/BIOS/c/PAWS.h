// I/O MEMORY MAPPED REGISTER DEFINITIONS
unsigned char volatile *UART_DATA = (unsigned char volatile *) 0xf000;
unsigned char volatile *UART_STATUS = (unsigned char volatile *) 0xf002;
unsigned short volatile *BUTTONS = (unsigned short volatile *) 0xf200;
unsigned char volatile *LEDS = (unsigned char volatile *) 0xf300;

// PS/2 KEYBOARD
unsigned char volatile *PS2_AVAILABLE = (unsigned char volatile *) 0xf100;
unsigned char volatile *PS2_MODE = (unsigned char volatile *) 0xf100;
unsigned char volatile *PS2_CAPSLOCK = (unsigned char volatile *) 0xf102;
unsigned char volatile *PS2_NUMLOCK = (unsigned char volatile *) 0xf104;
unsigned short volatile *PS2_DATA = (unsigned short volatile *) 0xf102;

// SDCARD
unsigned char volatile *SDCARD_READY = (unsigned char volatile *) 0xf400;
unsigned char volatile *SDCARD_READSTART = (unsigned char volatile *) 0xf400;
unsigned char volatile *SDCARD_WRITESTART = (unsigned char volatile *) 0xf402;
unsigned int volatile *SDCARD_SECTOR = (unsigned int *) 0xf404;
unsigned char volatile *SDCARD_RESET_BUFFERADDRESS = (unsigned char volatile *) 0xf500;
unsigned char volatile *SDCARD_DATA = (unsigned char volatile *) 0xf502;

// DISPLAY UNITS
unsigned char volatile *VBLANK = (unsigned char volatile *) 0xdf00;
unsigned char volatile *SCREENMODE = (unsigned char volatile *) 0xdf00;
unsigned char volatile *COLOUR = (unsigned char volatile *) 0xdf01;
unsigned char volatile *REZ = (unsigned char volatile *) 0xdf02;
unsigned char volatile *DIMMER = (unsigned char volatile *) 0xdf03;
unsigned char volatile *STATUS_DISPLAY = (unsigned char volatile *) 0xdf04;
unsigned char volatile *STATUS_BACKGROUND = (unsigned char volatile *) 0xdf05;
unsigned int volatile *PALETTERGB = (unsigned int volatile *) 0xdf10;
unsigned char volatile *PALETTEENTRY = (unsigned char volatile *) 0xdf14;
unsigned char volatile *PALETTEACTIVE = (unsigned char volatile *) 0xdf15;

// BACKGROUND AND COPPER - BASE 0xd000
unsigned char volatile *BACKGROUND_COLOUR = (unsigned char volatile *) 0xd000;
unsigned char volatile *BACKGROUND_ALTCOLOUR = (unsigned char volatile *) 0xd002;
unsigned char volatile *BACKGROUND_MODE = (unsigned char volatile *) 0xd004;
unsigned char volatile *BACKGROUND_COPPER_STARTSTOP = (unsigned char volatile *) 0xd006;
unsigned short volatile *BACKGROUND_COPPER_CPUINPUT = (unsigned short volatile *) 0xd008;
unsigned char volatile *BACKGROUND_COPPER_PROGRAM = (unsigned char volatile *) 0xd00a;
unsigned char volatile *BACKGROUND_COPPER_ADDRESS = (unsigned char volatile *) 0xd00c;
unsigned char volatile *BACKGROUND_COPPER_OP = (unsigned char volatile *) 0xd00e;
unsigned char volatile *BACKGROUND_COPPER_OPD = (unsigned char volatile *) 0xd010;
unsigned short volatile *BACKGROUND_COPPER_OPF = (unsigned short volatile *) 0xd012;
unsigned char volatile *BACKGROUND_COPPER_OPL = (unsigned char volatile *) 0xd014;
unsigned char volatile *BACKGROUND_COPPER_MEMRESET = (unsigned char volatile *) 0xd016;
unsigned short volatile *BACKGROUND_COPPER_MEMVINIT = (unsigned short volatile *) 0xd018;

unsigned char volatile *LOWER_TM_X = (unsigned char volatile *) 0xd100;
unsigned char volatile *LOWER_TM_Y = (unsigned char volatile *) 0xd102;
unsigned char volatile *LOWER_TM_TILE = (unsigned char volatile *) 0xd104;
unsigned char volatile *LOWER_TM_ACTION = (unsigned char volatile *) 0xd106;
unsigned char volatile *LOWER_TM_COMMIT = (unsigned char volatile *) 0xd108;
unsigned char volatile *LOWER_TM_STATUS = (unsigned char volatile *) 0xd108;
unsigned char volatile *LOWER_TM_WRITER_TILE_NUMBER = (unsigned char volatile *) 0xd10a;
unsigned char volatile *LOWER_TM_WRITER_COLOUR = (unsigned char volatile *) 0xd10c;
unsigned char volatile *LOWER_TM_SCROLLWRAPCLEAR = (unsigned char volatile *) 0xd10e;
unsigned char volatile *LOWER_TM_SCROLLAMOUNT = (unsigned char volatile *) 0xd10f;

unsigned char volatile *UPPER_TM_X = (unsigned char volatile *) 0xd200;
unsigned char volatile *UPPER_TM_Y = (unsigned char volatile *) 0xd202;
unsigned char volatile *UPPER_TM_TILE = (unsigned char volatile *) 0xd204;
unsigned char volatile *UPPER_TM_ACTION = (unsigned char volatile *) 0xd206;
unsigned char volatile *UPPER_TM_COMMIT = (unsigned char volatile *) 0xd208;
unsigned char volatile *UPPER_TM_STATUS = (unsigned char volatile *) 0xd208;
unsigned char volatile *UPPER_TM_WRITER_TILE_NUMBER = (unsigned char volatile *) 0xd20a;
unsigned char volatile *UPPER_TM_WRITER_COLOUR = (unsigned char volatile *) 0xd20c;
unsigned char volatile *UPPER_TM_SCROLLWRAPCLEAR = (unsigned char volatile *) 0xd20e;
unsigned char volatile *UPPER_TM_SCROLLWRAPAMOUNT = (unsigned char volatile *) 0xd20f;

short volatile *GPU_X = (short volatile *) 0xd600;
short volatile *GPU_Y = (short volatile *) 0xd602;
short volatile *GPU_PARAM0 = (short volatile *) 0xd604;
short volatile *GPU_PARAM1 = (short volatile *) 0xd606;
short volatile *GPU_PARAM2 = (short volatile *) 0xd608;
short volatile *GPU_PARAM3 = (short volatile *) 0xd60a;
short volatile *GPU_PARAM4 = (short volatile *) 0xd60c;
short volatile *GPU_PARAM5 = (short volatile *) 0xd60e;
unsigned char volatile *GPU_COLOUR = (unsigned char volatile *) 0xd610;
unsigned char volatile *GPU_COLOUR_ALT = (unsigned char volatile *) 0xd612;
unsigned char volatile *GPU_DITHERMODE = (unsigned char volatile *) 0xd614;
unsigned char volatile *GPU_WRITE = (unsigned char volatile *) 0xd616;
unsigned char volatile *GPU_STATUS = (unsigned char volatile *) 0xd616;
unsigned char volatile *GPU_FINISHED = (unsigned char volatile *) 0xd618;

unsigned char volatile *VECTOR_DRAW_BLOCK = (unsigned char volatile *) 0xd620;
unsigned char volatile *VECTOR_DRAW_COLOUR = (unsigned char volatile *) 0xd622;
short volatile *VECTOR_DRAW_XC = (short volatile *) 0xd624;
short volatile *VECTOR_DRAW_YC = (short volatile *) 0xd626;
unsigned char volatile *VECTOR_DRAW_SCALE = (unsigned char volatile *) 0xd628;
unsigned char volatile *VECTOR_DRAW_ACTION = (unsigned char volatile *) 0xd62a;
unsigned char volatile *VECTOR_DRAW_START = (unsigned char volatile *) 0xd62c;
unsigned char volatile *VECTOR_DRAW_STATUS = (unsigned char volatile *) 0xd62a;
unsigned char volatile *VECTOR_WRITER_BLOCK = (unsigned char volatile *) 0xd630;
unsigned char volatile *VECTOR_WRITER_VERTEX = (unsigned char volatile *) 0xd632;
char volatile *VECTOR_WRITER_DELTAX = (char volatile *) 0xd634;
char volatile *VECTOR_WRITER_DELTAY = (char volatile *) 0xd636;
unsigned char volatile *VECTOR_WRITER_ACTIVE = (unsigned char volatile *) 0xd638;

unsigned char volatile *BLIT_WRITER_TILE = (unsigned char volatile *) 0xd640;
unsigned short volatile *BLIT_WRITER_BITMAP = (unsigned short volatile *) 0xd642;
unsigned char volatile *BLIT_CHWRITER_TILE = (unsigned char volatile *) 0xd650;
unsigned char volatile *BLIT_CHWRITER_BITMAP = (unsigned char volatile *) 0xd652;
unsigned char volatile *COLOURBLIT_WRITER_TILE = (unsigned char volatile *) 0xd660;
unsigned char volatile *COLOURBLIT_WRITER_COLOUR = (unsigned char volatile *) 0xd662;

unsigned char volatile *PB_COLOUR = (unsigned char volatile *) 0xd670;
unsigned char volatile *PB_COLOUR8R = (unsigned char volatile *) 0xd672;
unsigned char volatile *PB_COLOUR8G = (unsigned char volatile *) 0xd674;
unsigned char volatile *PB_COLOUR8B = (unsigned char volatile *) 0xd676;
unsigned char volatile *PB_STOP = (unsigned char volatile *) 0xd678;
unsigned char volatile *PB_MODE = (unsigned char volatile *) 0xd67a;
unsigned char volatile *PB_CMNUMBER = (unsigned char volatile *) 0xd67c;
unsigned char volatile *PB_CMENTRY = (unsigned char volatile *) 0xd67e;
unsigned int volatile *PB_ARGB = (unsigned int volatile *) 0xd680;
unsigned int volatile *PB_RGBA = (unsigned int volatile *) 0xd684;

unsigned short volatile *CROP_LEFT = (unsigned short volatile *) 0xd6e0;
unsigned short volatile *CROP_RIGHT = (unsigned short volatile *) 0xd6e2;
unsigned short volatile *CROP_TOP = (unsigned short volatile *) 0xd6e4;
unsigned short volatile *CROP_BOTTOM = (unsigned short volatile *) 0xd6e6;

unsigned char volatile *FRAMEBUFFER_DISPLAY = (unsigned char volatile *) 0xd6f0;
unsigned char volatile *FRAMEBUFFER_DRAW = (unsigned char volatile *) 0xd6f2;
unsigned char volatile *BITMAP_DISPLAY256 = (unsigned char volatile *) 0xd6f4;

unsigned short volatile *LOWER_SPRITE_ACTIVE = (unsigned short volatile *) 0xd300;
unsigned short volatile *LOWER_SPRITE_ACTIONS = (unsigned short volatile *) 0xd320;
short volatile *LOWER_SPRITE_X = (short volatile *) 0xd360;
short volatile *LOWER_SPRITE_Y = (short volatile *) 0xd380;
unsigned short volatile *LOWER_SPRITE_TILE = (unsigned short volatile *) 0xd3a0;
unsigned short volatile *LOWER_SPRITE_UPDATE = (unsigned short volatile *) 0xd3c0;
unsigned short volatile *LOWER_SPRITE_COLLISION_BASE = (unsigned short volatile *) 0xd3c0;
unsigned short volatile *LOWER_SPRITE_LAYER_COLLISION_BASE = (unsigned short volatile *) 0xd3e0;
unsigned char volatile *LOWER_SPRITE_WRITER_NUMBER = (unsigned char volatile *) 0xd800;
unsigned char volatile *LOWER_SPRITE_WRITER_COLOUR = (unsigned char volatile *) 0xd802;

unsigned short volatile *UPPER_SPRITE_ACTIVE = (unsigned short volatile *) 0xd400;
unsigned short volatile *UPPER_SPRITE_ACTIONS = (unsigned short volatile *) 0xd420;
short volatile *UPPER_SPRITE_X = (short volatile *) 0xd460;
short volatile *UPPER_SPRITE_Y = (short volatile *) 0xd480;
unsigned short volatile *UPPER_SPRITE_TILE = (unsigned short volatile *) 0xd4a0;
unsigned short volatile *UPPER_SPRITE_UPDATE = (unsigned short volatile *) 0xd4c0;
unsigned short volatile *UPPER_SPRITE_COLLISION_BASE = (unsigned short volatile *) 0xd4c0;
unsigned short volatile *UPPER_SPRITE_LAYER_COLLISION_BASE = (unsigned short volatile *) 0xd4e0;
unsigned char volatile *UPPER_SPRITE_WRITER_NUMBER = (unsigned char volatile *) 0xd900;
unsigned char volatile *UPPER_SPRITE_WRITER_COLOUR = (unsigned char volatile *) 0xd902;

unsigned char volatile *TPU_X = (unsigned char volatile *) 0xd500;
unsigned char volatile *TPU_Y = (unsigned char volatile *) 0xd502;
unsigned short volatile *TPU_CHARACTER = (unsigned short volatile *) 0xd504;
unsigned char volatile *TPU_BACKGROUND = (unsigned char volatile *) 0xd506;
unsigned char volatile *TPU_FOREGROUND = (unsigned char volatile *) 0xd508;
unsigned char volatile *TPU_COMMIT = (unsigned char volatile *) 0xd50a;
unsigned char volatile *TPU_CURSOR = (unsigned char volatile *) 0xd50c;
unsigned char volatile *CURSES_BACKGROUND = (unsigned char volatile *) 0xd50e;
unsigned char volatile *CURSES_FOREGROUND = (unsigned char volatile *) 0xd50f;

unsigned char volatile *TERMINAL_COMMIT = (unsigned char volatile *) 0xd700;
unsigned char volatile *TERMINAL_STATUS = (unsigned char volatile *) 0xd700;
unsigned char volatile *TERMINAL_SHOW = (unsigned char volatile *) 0xd702;
unsigned char volatile *TERMINAL_RESET = (unsigned char volatile *) 0xd704;

unsigned char volatile *AUDIO_WAVEFORM = (unsigned char volatile *) 0xe000;
unsigned char volatile *AUDIO_FREQUENCY = (unsigned char volatile *) 0xe002;
unsigned short volatile *AUDIO_DURATION = (unsigned short volatile *) 0xe004;
unsigned char volatile *AUDIO_START = (unsigned char volatile *) 0xe006;
unsigned char volatile *AUDIO_NEW_SAMPLE = (unsigned char volatile *) 0xe008;
unsigned char volatile *AUDIO_LEFT_SAMPLE = (unsigned char volatile *) 0xe00a;
unsigned char volatile *AUDIO_RIGHT_SAMPLE = (unsigned char volatile *) 0xe00c;
unsigned char volatile *AUDIO_L_ACTIVE = (unsigned char volatile *) 0xe010;
unsigned char volatile *AUDIO_R_ACTIVE = (unsigned char volatile *) 0xe012;
unsigned char volatile *AUDIO_L_VOLUME = (unsigned char volatile *) 0xe010;
unsigned char volatile *AUDIO_R_VOLUME = (unsigned char volatile *) 0xe012;

unsigned short volatile *RNG = (unsigned short volatile *) 0xc000;
unsigned short volatile *ALT_RNG = (unsigned short volatile *) 0xc002;
float volatile *FRNG = (float volatile *) 0xc004;
unsigned short volatile *TIMER1HZ0 = (unsigned short volatile *) 0xc010;
unsigned short volatile *TIMER1HZ1 = (unsigned short volatile *) 0xc012;
unsigned short volatile *TIMER1KHZ0 = (unsigned short volatile *) 0xc014;
unsigned short volatile *TIMER1KHZ1 = (unsigned short volatile *) 0xc016;
unsigned short volatile *SLEEPTIMER0 = (unsigned short volatile *) 0xc018;
unsigned short volatile *SLEEPTIMER1 = (unsigned short volatile *) 0xc01a;
unsigned long volatile *SYSTEMSECONDS = (unsigned long volatile *) 0xc020;
unsigned int volatile *SYSTEMMILLISECONDS = (unsigned int volatile *)0xc028;
unsigned long volatile *RTC = (unsigned long volatile *) 0xf600;

// HANDLE SMT - RUNNING STATUS AND POINTER TO CODE TO RUN
unsigned char volatile *SMTSTATUS = (unsigned char volatile *) 0xff04;
unsigned int volatile *SMTPC = (unsigned int volatile *) 0xff00;

// HANDLE MINI DMA CONTROLLER
int volatile *DMASOURCEADD = (int volatile *) 0xfd00;
int volatile *DMADESTADD = (int volatile *) 0xfd04;
unsigned char volatile *DMACYCLES = (unsigned char volatile *) 0xfd08;
unsigned int volatile *DMASOURCE = (unsigned int volatile *) 0xfe00;
unsigned int volatile *DMADEST = (unsigned int volatile *) 0xfe04;
unsigned int volatile *DMACOUNT = (unsigned int volatile *) 0xfe08;
unsigned char volatile *DMAMODE = (unsigned char volatile *) 0xfe0c;
unsigned char volatile *DMASET = (unsigned char volatile *) 0xfe0e;

int volatile *AUDIO_REGS = (int volatile *) 0xe000;
char volatile *TPU_REGS_B = (char volatile *) 0xd500;

int volatile *PAWSMAGIC = (int volatile *) 0xf700;
unsigned int volatile *RAMBASE = (unsigned int volatile *) 0xf704;
unsigned int volatile *RAMTOP = (unsigned int volatile *) 0xf708;

// TYPES AND STRUCTURES
typedef unsigned int size_t;
typedef unsigned short bool;

// FOR EASE OF PORTING
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef signed char     int8;
typedef signed short    int16;
typedef signed int      int32;

// STRUCTURE OF THE SPRITE UPDATE FLAG
struct sprite_update_flag {
    unsigned int padding:3;
    unsigned int y_act:1;
    unsigned int x_act:1;
    unsigned int tile_act:1;
    int dy:5;
    int dx:5;
};

// FAT32 FILE SYSTEM
typedef struct {
    unsigned char first_byte;
    unsigned char start_chs[3];
    unsigned char partition_type;
    unsigned char end_chs[3];
    unsigned int start_sector;
    unsigned int length_sectors;
} __attribute((packed)) PartitionTable;

typedef struct {
    unsigned char jmp[3];
    char oem[8];
    unsigned short sector_size;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char number_of_fats;
    unsigned short root_dir_entries;
    unsigned short total_sectors_short; // if zero, later field is used
    unsigned char media_descriptor;
    unsigned short fat16_size_sectors;
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned int hidden_sectors;
    unsigned int total_sectors_long;
    unsigned int fat32_size_sectors;
    unsigned short flags;
    unsigned short version;
    unsigned int startof_root;
    unsigned short filesystem_information;
    unsigned short backupboot_sector;
    unsigned char reserved[12];
    unsigned char logical_drive_number;
    unsigned char unused;
    unsigned char extended_signature;
    unsigned int volume_id;
    char volume_label[11];
    char fs_type[8];
    char boot_code[420];
    unsigned short boot_sector_signature;
} __attribute((packed)) Fat32VolumeID;

union fname {
    unsigned char string[8];
    long sortvalue;
};

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
    union fname     filename;
    unsigned char   ext[3];
    unsigned char   type;
    unsigned int    starting_cluster;
    unsigned int    file_size;
} __attribute((packed)) DirectoryEntry;

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
#define GREY1 0x5b
#define GREY2 0xad
#define ORANGE 0xe9
#define PURPLE 0x86
#define LTBLUE 0x1f

#define UK_GOLD 241
#define UK_BLUE 30

// STANDARD CONSTANTS
#define NULL 0
#define true 1
#define false 0
#define TRUE 1
#define FALSE 0

// LOWER AND UPPER SPRITES/TILEMAPS
#define LOWER 0
#define UPPER 1

// SOUNDS
#define CHANNEL_LEFT 1
#define CHANNEL_RIGHT 2
#define CHANNEL_BOTH 3
#define WAVE_SQUARE 0
#define WAVE_SAW 1
#define WAVE_TRIANGLE 2
#define WAVE_SINE 3
#define WAVE_NOISE 4
#define WAVE_SAMPLE 8
#define SAMPLE_REPEAT 16

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

// NEW CU BACKGROUND CO-PROCESSOR
// OPCODES
#define CU_JMP 0
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

// REGISTER NAMES { VBLANK, X, Y, CPU, R0, R1, R2, R3 }
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
