// I/O MEMORY MAPPED REGISTER DEFINITIONS
unsigned char volatile *UART_DATA = (unsigned char volatile *) 0xf000;
unsigned char volatile *UART_STATUS = (unsigned char volatile *) 0xf002;

// PS/2 KEYBOARD
unsigned char volatile *PS2_AVAILABLE = (unsigned char volatile *) 0xf100;
unsigned char volatile *PS2_MODE = (unsigned char volatile *) 0xf100;
unsigned char volatile *PS2_CAPSLOCK = (unsigned char volatile *) 0xf102;
unsigned char volatile *PS2_NUMLOCK = (unsigned char volatile *) 0xf104;
unsigned short volatile *PS2_DATA = (unsigned short volatile *) 0xf102;

unsigned short volatile *BUTTONS = (unsigned short volatile *) 0xf200;
unsigned short volatile *MOUSE_X = (unsigned short volatile *) 0xf202;
unsigned short volatile *MOUSE_Y = (unsigned short volatile *) 0xf204;
unsigned short volatile *MOUSE_BUTTONS = (unsigned short volatile *) 0xf206;
unsigned char volatile *MOUSE_RESET = (unsigned char *) 0xf200;

unsigned char volatile *LEDS = (unsigned char volatile *) 0xf300;

// SDCARD
unsigned char volatile *SDCARD_READY = (unsigned char volatile *) 0xf400;
unsigned char volatile *SDCARD_ERROR = (unsigned char volatile *) 0xf401;
unsigned char volatile *SDCARD_READSTART = (unsigned char volatile *) 0xf400;
unsigned char volatile *SDCARD_WRITESTART = (unsigned char volatile *) 0xf402;
unsigned int volatile *SDCARD_SECTOR = (unsigned int *) 0xf404;
unsigned char volatile *SDCARD_RESET_BUFFERADDRESS = (unsigned char volatile *) 0xf500;
unsigned char volatile *SDCARD_IN_DATA = (unsigned char volatile *) 0xf504;
unsigned char volatile *SDCARD_OUT_DATA = (unsigned char volatile *) 0xf504;

// DISPLAY UNITS
unsigned char volatile *VBLANK = (unsigned char volatile *) 0xdf00;
unsigned int volatile *FRAMECOUNT = ( unsigned int volatile *)0xdf04;
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
unsigned short volatile *BACKGROUND_COPPER_ADDRESS = (unsigned short volatile *) 0xd00c;
unsigned int volatile *BACKGROUND_COPPER_OP = (unsigned int volatile *) 0xd010;
unsigned char volatile *BACKGROUND_COPPER_MEMRESET = (unsigned char volatile *) 0xd016;
unsigned short volatile *BACKGROUND_COPPER_MEMVINIT = (unsigned short volatile *) 0xd018;

unsigned char volatile *LOWER_TM_X = (unsigned char volatile *) 0xd100;
unsigned char volatile *LOWER_TM_Y = (unsigned char volatile *) 0xd102;
signed short volatile *LOWER_TM_OFFSET_X = (signed short  volatile *) 0xd104;
signed short volatile *LOWER_TM_OFFSET_Y = (signed short volatile *) 0xd106;
unsigned char volatile *LOWER_TM_WRITER_TILE_NUMBER = (unsigned char volatile *) 0xd10a;
unsigned char volatile *LOWER_TM_WRITER_COLOUR = (unsigned char volatile *) 0xd10c;
unsigned char volatile *LOWER_TM_SCROLLWRAPCLEAR = (unsigned char volatile *) 0xd10e;
unsigned char volatile *LOWER_TM_LASTACTION = (unsigned char volatile *) 0xd10e;
unsigned char volatile *LOWER_TM_SCROLLAMOUNT = (unsigned char volatile *) 0xd10f;

unsigned char volatile *UPPER_TM_X = (unsigned char volatile *) 0xd200;
unsigned char volatile *UPPER_TM_Y = (unsigned char volatile *) 0xd202;
signed short volatile *UPPER_TM_OFFSET_X = (signed short  volatile *) 0xd204;
signed short volatile *UPPER_TM_OFFSET_Y = (signed short volatile *) 0xd206;
unsigned char volatile *UPPER_TM_WRITER_TILE_NUMBER = (unsigned char volatile *) 0xd20a;
unsigned char volatile *UPPER_TM_WRITER_COLOUR = (unsigned char volatile *) 0xd20c;
unsigned char volatile *UPPER_TM_SCROLLWRAPCLEAR = (unsigned char volatile *) 0xd20e;
unsigned char volatile *UPPER_TM_LASTACTION = (unsigned char volatile *) 0xd20e;
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
unsigned int volatile *GPU_COLOURRGB = (unsigned int volatile *) 0xd610;
unsigned char volatile *GPU_COLOUR_ALT = (unsigned char volatile *) 0xd612;
unsigned char volatile *GPU_DITHERMODE = (unsigned char volatile *) 0xd614;
unsigned char volatile *GPU_WRITE = (unsigned char volatile *) 0xd616;
unsigned char volatile *GPU_STATUS = (unsigned char volatile *) 0xd616;
unsigned char volatile *GPU_FINISHED = (unsigned char volatile *) 0xd618;

unsigned char volatile *BLIT_WRITER_TILE = (unsigned char volatile *) 0xd640;
unsigned short volatile *BLIT_WRITER_BITMAP = (unsigned short volatile *) 0xd642;
unsigned char volatile *BLIT_CHWRITER_TILE = (unsigned char volatile *) 0xd650;
unsigned char volatile *BLIT_CHWRITER_BITMAP = (unsigned char volatile *) 0xd652;
unsigned char volatile *COLOURBLIT_WRITER_TILE = (unsigned char volatile *) 0xd660;
unsigned char volatile *COLOURBLIT_WRITER_COLOUR = (unsigned char volatile *) 0xd662;

unsigned char volatile *PB_COLOUR = (unsigned char volatile *) 0xd670;
unsigned char volatile *PB_COLOUR_RGB = (unsigned char volatile *) 0xd672;
unsigned char volatile *PB_STOP = (unsigned char volatile *) 0xd678;
unsigned char volatile *PB_MODE = (unsigned char volatile *) 0xd67a;
unsigned char volatile *PB_CMNUMBER = (unsigned char volatile *) 0xd67c;
unsigned char volatile *PB_CMENTRY = (unsigned char volatile *) 0xd67e;
unsigned int volatile *PB_ARGB = (unsigned int volatile *) 0xd680;
unsigned int volatile *PB_RGBA = (unsigned int volatile *) 0xd684;
unsigned int volatile *PB_ABGR = (unsigned int volatile *) 0xd688;
unsigned int volatile *PB_BGRA = (unsigned int volatile *) 0xd68c;

unsigned short volatile *CROP_LEFT = (unsigned short volatile *) 0xd6e0;
unsigned short volatile *CROP_RIGHT = (unsigned short volatile *) 0xd6e2;
unsigned short volatile *CROP_TOP = (unsigned short volatile *) 0xd6e4;
unsigned short volatile *CROP_BOTTOM = (unsigned short volatile *) 0xd6e6;

unsigned char volatile *FRAMEBUFFER_DISPLAY = (unsigned char volatile *) 0xd6f0;
unsigned char volatile *FRAMEBUFFER_DRAW = (unsigned char volatile *) 0xd6f2;
unsigned char volatile *BITMAP_DISPLAY256 = (unsigned char volatile *) 0xd6f4;

unsigned char volatile *LOWER_SPRITE_ACTIVE = (unsigned char volatile *) 0xd300;
unsigned char volatile *LOWER_SPRITE_ACTIONS = (unsigned char volatile *) 0xd320;
unsigned char volatile *LOWER_SPRITE_TILE = (unsigned char volatile *) 0xd340;
unsigned char volatile *LOWER_SPRITE_LAYER_COLLISION_BASE = (unsigned char volatile *) 0xd360;
short volatile *LOWER_SPRITE_X = (short volatile *) 0xd380;
short volatile *LOWER_SPRITE_Y = (short volatile *) 0xd3c0;
unsigned short volatile *LOWER_SPRITE_UPDATE = (unsigned short volatile *) 0xda00;
unsigned int volatile *LOWER_SPRITE_COLLISION_BASE = (unsigned int volatile *) 0xd800;
unsigned char volatile *LOWER_SPRITE_WRITER_NUMBER = (unsigned char volatile *) 0xd800;
unsigned char volatile *LOWER_SPRITE_WRITER_COLOUR = (unsigned char volatile *) 0xd802;

unsigned char volatile *UPPER_SPRITE_ACTIVE = (unsigned char volatile *) 0xd400;
unsigned char volatile *UPPER_SPRITE_ACTIONS = (unsigned char volatile *) 0xd420;
unsigned char volatile *UPPER_SPRITE_TILE = (unsigned char volatile *) 0xd440;
unsigned char volatile *UPPER_SPRITE_LAYER_COLLISION_BASE = (unsigned char volatile *) 0xd460;
short volatile *UPPER_SPRITE_X = (short volatile *) 0xd480;
short volatile *UPPER_SPRITE_Y = (short volatile *) 0xd4c0;
unsigned short volatile *UPPER_SPRITE_UPDATE = (unsigned short volatile *) 0xdb00;
unsigned int volatile *UPPER_SPRITE_COLLISION_BASE = (unsigned int volatile *) 0xd900;
unsigned char volatile *UPPER_SPRITE_WRITER_NUMBER = (unsigned char volatile *) 0xd900;
unsigned char volatile *UPPER_SPRITE_WRITER_COLOUR = (unsigned char volatile *) 0xd902;

unsigned char volatile *TPU_X = (unsigned char volatile *) 0xd500;
unsigned char volatile *TPU_Y = (unsigned char volatile *) 0xd502;
unsigned char volatile *TPU_BACKGROUND = (unsigned char volatile *) 0xd504;
unsigned char volatile *TPU_FOREGROUND = (unsigned char volatile *) 0xd506;
unsigned char volatile *TPU_CURSOR = (unsigned char volatile *) 0xd508;

unsigned char volatile *AUDIO_WAVEFORM = (unsigned char volatile *) 0xe000;
unsigned char volatile *AUDIO_FREQUENCY = (unsigned char volatile *) 0xe002;
unsigned short volatile *AUDIO_DURATION = (unsigned short volatile *) 0xe004;
unsigned char volatile *AUDIO_START = (unsigned char volatile *) 0xe006;
unsigned char volatile *AUDIO_NEW_SAMPLE = (unsigned char volatile *) 0xe008;
unsigned char volatile *AUDIO_LEFT_SAMPLE = (unsigned char volatile *) 0xe00a;
unsigned char volatile *AUDIO_RIGHT_SAMPLE = (unsigned char volatile *) 0xe00c;
unsigned char volatile *AUDIO_L_VOLUME = (unsigned char volatile *) 0xe010;
unsigned char volatile *AUDIO_R_VOLUME = (unsigned char volatile *) 0xe012;
unsigned char volatile *AUDIO_NEW_BITSAMPLE = (unsigned char volatile *) 0xe014;
unsigned char volatile *AUDIO_LEFT_BITSAMPLE = (unsigned char volatile *) 0xe016;
unsigned char volatile *AUDIO_RIGHT_BITSAMPLE = (unsigned char volatile *) 0xe018;
unsigned char volatile *AUDIO_NEW_WAVEFORM = (unsigned char volatile *) 0xe01a;
unsigned char volatile *AUDIO_LEFT_WAVESAMPLE = (unsigned char volatile *) 0xe01c;
unsigned char volatile *AUDIO_RIGHT_WAVESAMPLE = (unsigned char volatile *) 0xe01e;

unsigned int volatile *AUDIO_DMA_L_BASE = (unsigned int volatile *) 0xe020;
unsigned int volatile *AUDIO_DMA_L_LENGTH = (unsigned int volatile *) 0xe024;
unsigned char volatile *AUDIO_DMA_L_STATUS = (unsigned char volatile *) 0xe028;

unsigned int volatile *AUDIO_DMA_R_BASE = (unsigned int volatile *) 0xe030;
unsigned int volatile *AUDIO_DMA_R_LENGTH = (unsigned int volatile *) 0xe034;
unsigned char volatile *AUDIO_DMA_R_STATUS = (unsigned char volatile *) 0xe038;

unsigned char volatile *AUDIO_ACTIVE = (unsigned char volatile *) 0xe000;

unsigned short volatile *RNG = (unsigned short volatile *) 0xc000;
unsigned short volatile *ALT_RNG = (unsigned short volatile *) 0xc002;
float volatile *FRNG = (float volatile *) 0xc004;
unsigned long volatile *SET_RTC_TIME = (unsigned long volatile *) 0xc020;
unsigned long volatile *SYSTEMSECONDS = (unsigned long volatile *) 0xc020;
unsigned int volatile *SYSTEMMILLISECONDS = (unsigned int volatile *)0xc028;
unsigned char volatile *SET_RTC = (unsigned char volatile *)0xc02a;
unsigned long volatile *RTC = (unsigned long volatile *) 0xf600;

// MAGIC FOR SECTOR 0 WRITE, RAM POSITIONS
int volatile *PAWSMAGIC = (int volatile *) 0xf700;
unsigned int volatile *RAMBASE = (unsigned int volatile *) 0xf704;
unsigned int volatile *RAMTOP = (unsigned int volatile *) 0xf708;
unsigned int volatile *STACKTOP = (unsigned int volatile *) 0xf70c;

// IRQ_TIMER COMPARATOR
unsigned long volatile *IRQ_TIMER_COMPARATOR = (unsigned long volatile *) 0xfff0;
unsigned int volatile *IRQ_TIMER_NEXT = (unsigned int volatile *) 0xfff8;

// HANDLE MINI DMA CONTROLLER
int volatile *DMASOURCEADD = (int volatile *) 0xfd00;
int volatile *DMADESTADD = (int volatile *) 0xfd04;
unsigned char volatile *DMACYCLES = (unsigned char volatile *) 0xfd08;
unsigned int volatile *DMASET32 = (unsigned int volatile *) 0xfd0c;
unsigned int volatile *DMASOURCE = (unsigned int volatile *) 0xfe00;
unsigned int volatile *DMADEST = (unsigned int volatile *) 0xfe04;
unsigned int volatile *DMACOUNT = (unsigned int volatile *) 0xfe08;
unsigned short volatile *DMAMODE = (unsigned short volatile *) 0xfe0c;
unsigned char volatile *DMASET = (unsigned char volatile *) 0xfe0e;
unsigned int volatile *DMASETRGB = (unsigned int volatile *) 0xfe0c;

unsigned char volatile *FRAMEBUFFER0 = (unsigned char volatile *)   0x2000000;
unsigned char volatile *FRAMEBUFFER1 = (unsigned char volatile *)   0x2020000;
unsigned int volatile *TPUBUFFER = (unsigned int volatile *)        0x1000000;
unsigned short volatile *LTMAPBUFFER = (unsigned short volatile *)   0x800000;
unsigned short volatile *UTMAPBUFFER = (unsigned short volatile *)   0x808000;

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

// FONT ATTRIBUTES
#define TPU_NORMAL 0
#define TPU_BOLD 1
#define TPU_X2 2
#define TPU_Y2 4
#define TPU_BLINK 8
#define TPU_UNDER 16

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

// DMA(NEW) TRANSFER PROTOCOLS
#define DMA_SET_TO_S    0x10
#define DMA_SET_TO_M    0x12
#define DMA_CPY_S_TO_S  0x20
#define DMA_CPY_M_TO_S  0x21
#define DMA_CPY_S_TO_M  0x22
#define DMA_CPY_M_TO_M  0x23
#define DMA_CPY_STEP_SD 0x27
#define DMA_SET_RECT    0x40
#define DMA_CPY_RECT    0x80
#define DMA_TO_IO       0x100
#define DMA_FROM_IO     0x101

// IRQ FLAGS
#define IRQ_SOFTWARE    ( 1 << 3 )
#define IRQ_TIMER       ( 1 << 7 )
#define IRQ_VBLANK      ( 1 << 11 )
#define IRQ_CAUSE_SOFTWARE 3
#define IRQ_CAUSE_TIMER 7
#define IRQ_CAUSE_VBLANK 11
#define IRQ_TIMER_CYCLES 50000000
#define IRQ_1_HZ 0
#define IRQ_10_HZ 1
#define IRQ_100_HZ 2
#define IRQ_1000_HZ 3


