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
unsigned char volatile *COLOUR = (unsigned char volatile *) 0xdf01;
unsigned char volatile *DIMMER = (unsigned char volatile *) 0xdf03;
unsigned char volatile *STATUS_DISPLAY = (unsigned char volatile *) 0xdf04;
unsigned char volatile *STATUS_BACKGROUND = (unsigned char volatile *) 0xdf05;
unsigned int volatile *PALETTERGB = (unsigned int volatile *) 0xdf10;
unsigned char volatile *PALETTEENTRY = (unsigned char volatile *) 0xdf14;
unsigned char volatile *PALETTEACTIVE = (unsigned char volatile *) 0xdf15;
unsigned long volatile *SCREENORDER = (unsigned long volatile *) 0xdf20;

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

unsigned short volatile *TM_X = (unsigned short volatile *) 0xd100;
unsigned short volatile *TM_Y = (unsigned short volatile *) 0xd108;
short volatile *TM_OFFSET_X = (short volatile *) 0xd110;
short volatile *TM_OFFSET_Y = (short volatile *) 0xd118;
unsigned short volatile *TM_SCROLLWRAP = (unsigned short volatile *) 0xd120;
unsigned short volatile *TM_SCROLLAMOUNT = (unsigned short volatile *) 0xd128;
unsigned short volatile *TM_LASTACTION = (unsigned short volatile *) 0xd130;
unsigned char volatile *TM_WRITER_TILE_NUMBER = (unsigned char volatile *) 0xd140;
unsigned char volatile *TM_WRITER_COLOUR = (unsigned char volatile *) 0xd142;
unsigned char volatile *TM_LOREZ = (unsigned char volatile *) 0xd180;
unsigned short volatile *TMAPBUFFER[4] = { (unsigned short volatile *) 0x800000, (unsigned short volatile *) 0x808000, (unsigned short volatile *) 0x810000, (unsigned short volatile *) 0x818000 };

short volatile *GPU_X = (short volatile *) 0xd300;
short volatile *GPU_Y = (short volatile *) 0xd302;
short volatile *GPU_PARAM0 = (short volatile *) 0xd304;
short volatile *GPU_PARAM1 = (short volatile *) 0xd306;
short volatile *GPU_PARAM2 = (short volatile *) 0xd308;
short volatile *GPU_PARAM3 = (short volatile *) 0xd30a;
short volatile *GPU_PARAM4 = (short volatile *) 0xd30c;
short volatile *GPU_PARAM5 = (short volatile *) 0xd30e;
unsigned char volatile *GPU_COLOUR = (unsigned char volatile *) 0xd310;
unsigned int volatile *GPU_COLOURRGB = (unsigned int volatile *) 0xd310;
unsigned char volatile *GPU_COLOUR_ALT = (unsigned char volatile *) 0xd312;
unsigned char volatile *GPU_DITHERMODE = (unsigned char volatile *) 0xd314;
unsigned char volatile *GPU_WRITE = (unsigned char volatile *) 0xd316;
unsigned char volatile *GPU_STATUS = (unsigned char volatile *) 0xd316;
unsigned char volatile *GPU_FINISHED = (unsigned char volatile *) 0xd318;

unsigned char volatile *BLIT_WRITER_TILE = (unsigned char volatile *) 0xd340;
unsigned short volatile *BLIT_WRITER_BITMAP = (unsigned short volatile *) 0xd342;
unsigned char volatile *BLIT_CHWRITER_TILE = (unsigned char volatile *) 0xd350;
unsigned char volatile *BLIT_CHWRITER_BITMAP = (unsigned char volatile *) 0xd352;
unsigned char volatile *COLOURBLIT_WRITER_TILE = (unsigned char volatile *) 0xd360;
unsigned char volatile *COLOURBLIT_WRITER_COLOUR = (unsigned char volatile *) 0xd362;

unsigned char volatile *PB_COLOUR = (unsigned char volatile *) 0xd370;
unsigned char volatile *PB_COLOUR_RGB = (unsigned char volatile *) 0xd372;
unsigned char volatile *PB_STOP = (unsigned char volatile *) 0xd378;
unsigned char volatile *PB_MODE = (unsigned char volatile *) 0xd37a;
unsigned char volatile *PB_CMNUMBER = (unsigned char volatile *) 0xd37c;
unsigned char volatile *PB_CMENTRY = (unsigned char volatile *) 0xd37e;
unsigned int volatile *PB_ARGB = (unsigned int volatile *) 0xd380;
unsigned int volatile *PB_RGBA = (unsigned int volatile *) 0xd384;
unsigned int volatile *PB_ABGR = (unsigned int volatile *) 0xd388;
unsigned int volatile *PB_BGRA = (unsigned int volatile *) 0xd38c;

unsigned short volatile *CROP_LEFT = (unsigned short volatile *) 0xd3e0;
unsigned short volatile *CROP_RIGHT = (unsigned short volatile *) 0xd3e2;
unsigned short volatile *CROP_TOP = (unsigned short volatile *) 0xd3e4;
unsigned short volatile *CROP_BOTTOM = (unsigned short volatile *) 0xd3e6;

unsigned char volatile *FRAMEBUFFER_DRAW = (unsigned char volatile *) 0xd3f2;
unsigned char volatile *BITMAP_DISPLAY256 = (unsigned char volatile *) 0xd3f4;

unsigned char volatile *TPU_X = (unsigned char volatile *) 0xd300;
unsigned char volatile *TPU_Y = (unsigned char volatile *) 0xd301;
unsigned char volatile *TPU_BACKGROUND = (unsigned char volatile *) 0xd302;
unsigned char volatile *TPU_FOREGROUND = (unsigned char volatile *) 0xd303;
unsigned char volatile *TPU_CURSOR = (unsigned char volatile *) 0xd304;
unsigned char volatile *TPU_LOREZ = (unsigned char volatile *) 0xd305;
unsigned short volatile *TPU_CHARGEN_CHAR = (unsigned short volatile *) 0xd308;
unsigned char volatile *TPU_CHARGEN_BITMAP = (unsigned char volatile *) 0xd30a;

unsigned short volatile *SPRITE_ACTIVE = (unsigned short volatile *) 0xd400;
unsigned short volatile *SPRITE_ACTIONS = (unsigned short volatile *) 0xd480;
unsigned short volatile *SPRITE_TILE = (unsigned short volatile *) 0xd500;
unsigned short volatile *SPRITE_SIZE = (unsigned short volatile *) 0xd580;
short volatile *SPRITE_X = (short volatile *) 0xd600;
short volatile *SPRITE_Y = (short volatile *) 0xd680;
unsigned char volatile *SPRITE_WRITER_NUMBER = (unsigned char volatile *) 0xd700;
unsigned char volatile *SPRITE_WRITER_COLOUR = (unsigned char volatile *) 0xd702;
unsigned short volatile *SPRITE_LAYER_COLLISION_BASE = (unsigned short volatile *) 0xd700;
unsigned long volatile *SPRITE_COLLISION_BASE = (unsigned long volatile *) 0xd800;

unsigned char volatile *AUDIO_WAVEFORM = (unsigned char volatile *) 0xe000;
unsigned char volatile *AUDIO_FREQUENCY = (unsigned char volatile *) 0xe010;
unsigned short volatile *AUDIO_DURATION = (unsigned short volatile *) 0xe020;
unsigned char volatile *AUDIO_VOLUME = (unsigned char volatile *) 0xe030;
unsigned char volatile *AUDIO_NEW_SAMPLE = (unsigned char volatile *) 0xe040;
unsigned char volatile *AUDIO_SAMPLE = (unsigned char volatile *) 0xe048;
unsigned char volatile *AUDIO_NEW_BITSAMPLE = (unsigned char volatile *) 0xe050;
unsigned char volatile *AUDIO_BITSAMPLE = (unsigned char volatile *) 0xe058;

unsigned int volatile *AUDIO_DMA_L_BASE = (unsigned int volatile *) 0xe080;
unsigned int volatile *AUDIO_DMA_L_LENGTH = (unsigned int volatile *) 0xe084;
unsigned char volatile *AUDIO_DMA_L_STATUS = (unsigned char volatile *) 0xe088;

unsigned int volatile *AUDIO_DMA_R_BASE = (unsigned int volatile *) 0xe090;
unsigned int volatile *AUDIO_DMA_R_LENGTH = (unsigned int volatile *) 0xe094;
unsigned char volatile *AUDIO_DMA_R_STATUS = (unsigned char volatile *) 0xe098;

unsigned char volatile *AUDIO_ACTIVE = (unsigned char volatile *) 0xe800;

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
unsigned int volatile *IRQ_TIMER_ADD = (unsigned int volatile *) 0xfffc;

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


