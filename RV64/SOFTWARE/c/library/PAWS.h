// I/O MEMORY MAPPED REGISTER DEFINITIONS
unsigned char volatile *UART_DATA = (unsigned char volatile *) 0xf000;
unsigned char volatile *UART_STATUS = (unsigned char volatile *) 0xf002;
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
unsigned char volatile *SD_BUFFER_SECTOR = (unsigned char volatile *) 0xf408;                                                 //                          SECTOR WITHIN BUFFER ( 0 - 7 )
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
unsigned char volatile *FRAMEBUFFERS[2] = { (unsigned char volatile *)0x2000000, (unsigned char volatile *)0x2020000 };

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

// HANDLE SMT - RUNNING STATUS AND POINTER TO CODE TO RUN
unsigned int volatile *SMTPC = (unsigned int volatile *) 0xff00;
unsigned char volatile *SMTSTATUS = (unsigned char volatile *) 0xff04;
unsigned int volatile *SMTSTACK = (unsigned int volatile *) 0xff08;

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

unsigned char volatile *FRAMEBUFFER0 = (unsigned char volatile *)0x2000000;
unsigned char volatile *FRAMEBUFFER1 = (unsigned char volatile *)0x2020000;
unsigned int volatile *TPUBUFFER = (unsigned int volatile *)0x1000000;
