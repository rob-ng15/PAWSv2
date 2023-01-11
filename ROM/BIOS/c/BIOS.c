#include "PAWS.h"

typedef unsigned int size_t;

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

// PAWS LOGO BLITTER TILE
unsigned short PAWSLOGO[] = {
    0b0000000001000000,
    0b0000100011100000,
    0b0001110011100000,
    0b0001110011100000,
    0b0001111011100100,
    0b0000111001001110,
    0b0010010000001110,
    0b0111000000001110,
    0b0111000111001100,
    0b0111001111110000,
    0b0011011111111000,
    0b0000011111111000,
    0b0000011111111100,
    0b0000111111111100,
    0b0000111100001000,
    0b0000010000000000
};

// RISC-V CSR FUNCTIONS
unsigned int CSRisa() {
   unsigned int isa;
   asm volatile ("csrr %0, 0x301" : "=r"(isa));
   return isa;
}

// DMA CONTROLLER
void DMASTART( const void *restrict source, void *restrict destination, unsigned int count, unsigned char mode ) {
    *DMASOURCE = (unsigned int)source;
    *DMADEST = (unsigned int)destination;
    *DMACOUNT = count;
    *DMAMODE = mode;
}

// STANDARD C FUNCTIONS ( from @sylefeb mylibc )
void *memset(void *dest, int val, size_t len) {
    *DMASET = val;
    DMASTART( (const void *restrict)DMASET, dest, len, 4 );
    return dest;
}

void *memcpy( void *dest, void *src, size_t len ) {
    DMASTART( src, dest, len, 3 );
    return dest;
}

short strlen( char *s ) {
    short i = 0;
    while( *s++ ) {
        i++;
    }
    return(i);
}

// TIMER AND PSEUDO RANDOM NUMBER GENERATOR
// SLEEP FOR counter milliseconds
void sleep( unsigned short counter ) {
    *SLEEPTIMER0 = counter;
    while( *SLEEPTIMER0 );
}

// I/O FUNCTIONS
// READ THE ULX3S JOYSTICK BUTTONS
unsigned short get_buttons( void ) {
    return( *BUTTONS );
}

// WAIT FOR VBLANK TO START
void await_vblank( void ) {
    while( !*VBLANK );
}

// AUDIO CONTROLS
void beep( unsigned char channel_number, unsigned char waveform, unsigned char note, unsigned short duration ) {
    *AUDIO_WAVEFORM = waveform; *AUDIO_FREQUENCY = note; *AUDIO_DURATION = duration; *AUDIO_START = channel_number;
}
void volume( unsigned char left, unsigned char right ) {
    *AUDIO_L_VOLUME = left; *AUDIO_R_VOLUME = right;
}
void sample_upload( unsigned char channel_number, unsigned short length, unsigned char *samples ) {
    beep( channel_number, 0, 0, 0 );
    *AUDIO_NEW_SAMPLE = channel_number;
    if( channel_number & 1 ) { DMASTART( samples, (void *restrict)AUDIO_LEFT_SAMPLE, length, 1 ); }
    if( channel_number & 2 ) { DMASTART( samples, (void *restrict)AUDIO_RIGHT_SAMPLE, length, 1 ); }
}

// BACKGROUND GENERATOR
void set_background( unsigned char colour, unsigned char altcolour, unsigned char backgroundmode ) {
    *BACKGROUND_COPPER_STARTSTOP = 0;
    *BACKGROUND_COLOUR = colour;
    *BACKGROUND_ALTCOLOUR = altcolour;
    *BACKGROUND_MODE = backgroundmode;
}

// GPU AND BITMAP
// The bitmap is 320 x 240 pixels (0,0) is top left
// The GPU can draw pixels, filled rectangles, lines, (filled) circles, filled triangles and has a 16 x 16 pixel blitter from user definable tiles

// INTERNAL FUNCTION - WAIT FOR THE GPU TO FINISH THE LAST COMMAND
void wait_gpu( void ) {
    while( *GPU_STATUS );
}

// DRAW A FILLED RECTANGLE from (x1,y1) to (x2,y2) in colour
void gpu_rectangle( unsigned char colour, short x1, short y1, short x2, short y2 ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = x2;
    *GPU_PARAM1 = y2;

    wait_gpu();
    *GPU_WRITE = 3;
}

// CLEAR THE BITMAP by drawing a transparent rectangle from (0,0) to (639,479) and resetting the bitamp scroll position
void gpu_cs( void ) {
    wait_gpu();
    gpu_rectangle( 64, 0, 0, 319, 239 );
}

// BLIT A 16 x 16 ( blit_size == 1 doubled to 32 x 32 ) TILE ( from tile 0 to 31 ) to (x1,y1) in colour
void gpu_blit( unsigned char colour, short x1, short y1, short tile, unsigned char blit_size ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = tile;
    *GPU_PARAM1 = blit_size;
    *GPU_PARAM2 = 0; // NO REFLECTION

    wait_gpu();
    *GPU_WRITE = 7;
}

// BLIT AN 8 x8  ( blit_size == 1 doubled to 16 x 16, blit_size == 1 doubled to 32 x 32 ) CHARACTER ( from tile 0 to 255 ) to (x1,y1) in colour
void gpu_character_blit( unsigned char colour, short x1, short y1, unsigned short tile, unsigned char blit_size ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = tile;
    *GPU_PARAM1 = blit_size;
    *GPU_PARAM2 = 0; // NO REFLECTION

    wait_gpu();
    *GPU_WRITE = 8;
}

// OUTPUT A STRING TO THE GPU
void gpu_outputstring( unsigned char colour, short x, short y, char bold, char *s, unsigned char size ) {
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256 : 0 ) + *s++, size );
        x = x + ( 8 << size );
    }
}
void gpu_outputstringcentre( unsigned char colour, short y, char bold, char *s, unsigned char size ) {
    gpu_rectangle( TRANSPARENT, 0, y, 319, y + ( 8 << size ) - 1 );
    gpu_outputstring( colour, 160 - ( ( ( 8 << size ) * strlen(s) ) >> 1) , y, bold, s, size );
}

// SET THE BLITTER TILE to the 16 x 16 pixel bitmap
void set_blitter_bitmap( unsigned char tile, unsigned short *bitmap ) {
    *BLIT_WRITER_TILE = tile;

    for( short i = 0; i < 16; i ++ ) {
        *BLIT_WRITER_BITMAP = *bitmap++;
    }
}

// STOP PIXEL BLOCK - SENT DURING RESET TO ENSURE GPU RESETS
void gpu_pixelblock_stop( void ) {
    *PB_STOP = 3;
}

// CLEAR THE CHARACTER MAP
void tpu_cs( void ) {
    *TPU_COMMIT = 3;
}

// SET THE TILEMAP TILE at (x,y) to tile
void set_tilemap_tile( unsigned char tm_layer, unsigned char x, unsigned char y, unsigned char tile, unsigned char action ) {
    switch( tm_layer ) {
        case 0:
            while( *LOWER_TM_STATUS );
            *LOWER_TM_X = x;
            *LOWER_TM_Y = y;
            *LOWER_TM_TILE = tile;
            *LOWER_TM_ACTION = action;
            *LOWER_TM_COMMIT = 1;
            break;
        case 1:
            while( *UPPER_TM_STATUS );
            *UPPER_TM_X = x;
            *UPPER_TM_Y = y;
            *UPPER_TM_TILE = tile;
            *UPPER_TM_ACTION = action;
            *UPPER_TM_COMMIT = 1;
            break;
    }
}

// SCROLL WRAP or CLEAR the TILEMAP by amount ( 0 - 15 ) pixels
//  action == 1 to 4 move the tilemap amount pixels LEFT, UP, RIGHT, DOWN
//  action == 5 clear the tilemap
//  RETURNS 0 if no action taken other than pixel shift, action if SCROLL WRAP or CLEAR was actioned
unsigned char tilemap_scrollwrapclear( unsigned char tm_layer, unsigned char action, unsigned char amount ) {
    while( *( tm_layer ? UPPER_TM_STATUS : LOWER_TM_STATUS ) );
    *( tm_layer ? UPPER_TM_SCROLLWRAPAMOUNT : LOWER_TM_SCROLLAMOUNT ) = amount;
    *( tm_layer ? UPPER_TM_SCROLLWRAPCLEAR : LOWER_TM_SCROLLWRAPCLEAR ) = action;
    return( tm_layer ? *UPPER_TM_SCROLLWRAPCLEAR : *LOWER_TM_SCROLLWRAPCLEAR );
}

// SMT START STOP
void SMTSTOP( void ) {
    *SMTSTATUS = 0;
}
void SMTSTART( unsigned int code ) {
    *SMTPC = code;
    *SMTSTATUS = 1;
}

void draw_paws_logo( void ) {
    set_blitter_bitmap( 3, &PAWSLOGO[0] );
    gpu_blit( UK_GOLD, 2, 2, 3, 2 );
}

void reset_display( void ) {
    // WAIT FOR THE GPU TO FINISH
    gpu_pixelblock_stop();
    while( !*GPU_FINISHED );

    *GPU_DITHERMODE = 0; *CROP_LEFT = 0; *CROP_RIGHT = 319; *CROP_TOP = 0; *CROP_BOTTOM = 239;
    *FRAMEBUFFER_DRAW = 3; gpu_cs(); while( !*GPU_FINISHED );
    *FRAMEBUFFER_DRAW = 1; *FRAMEBUFFER_DISPLAY = 1; *BITMAP_DISPLAY256 = 0; *PALETTEACTIVE = 0;
    *SCREENMODE = 0; *COLOUR = 0; *REZ = 0; *DIMMER = 0; *STATUS_DISPLAY = 1; *STATUS_BACKGROUND = 0x40;
    *TPU_CURSOR = 0; tpu_cs();
    *TERMINAL_SHOW = 0; *TERMINAL_RESET = 1;
    *LOWER_TM_SCROLLWRAPCLEAR = 5; *UPPER_TM_SCROLLWRAPCLEAR = 5;
    for( unsigned short i = 0; i < 16; i++ ) {
        LOWER_SPRITE_ACTIVE[i] = UPPER_SPRITE_ACTIVE[i] = 0;
    }
}

// POSITION THE CURSOR to (x,y) and set background and foreground colours
void tpu_set( unsigned char x, unsigned char y, unsigned char background, unsigned char foreground ) {
    while( *TPU_COMMIT );
    *TPU_X = x; *TPU_Y = y; *TPU_BACKGROUND = background; *TPU_FOREGROUND = foreground; *TPU_COMMIT = 1;
}
// OUTPUT CHARACTER, STRING EQUIVALENT FOR THE TPU
void tpu_output_character( short c ) {
    while( *TPU_COMMIT );
    *TPU_CHARACTER = c; *TPU_COMMIT = 2;
}
void tpu_outputstring( char *s ) {
    while( *s ) {
        tpu_output_character( *s );
        s++;
    }
}
void tpu_outputbinary( int number, int length ) {
    for( int i = length - 1; i != -1; i-- )
        tpu_output_character( '0' +  _rv32_bext( number, i ) );
}

// SMT THREAD TO MOVE COLOUR BARS AND FLASH LEDS
void scrollbars( void ) {
    unsigned char leds = 1, direction = 0, ledcount = 0;
    short count = 0;
    int rtc_low, rtc_high;

    while(1) {
        await_vblank();count++;
        if( count == 64 ) {
            tilemap_scrollwrapclear( 0, 3, 1 );
            tilemap_scrollwrapclear( 1, 1, 1 );
            count = 0;
            ledcount++;
            if( ledcount == 32 ) {
                if( direction ) {
                    if( leds == 1 ) { direction = 0; } else { leds = leds >> 1; }
                } else {
                    if( leds == 128 ) { direction = 1; } else { leds = leds << 1; }
                }
                *LEDS = leds;
                ledcount = 0;
                tpu_set( 0, 17, TRANSPARENT, WHITE );
                rtc_high = RTC[0] << 8; rtc_low = RTC[1] + 0x20000000;
                for( int i = 0; i < 8; i++ ) {
                    tpu_output_character( 48 + ( ( rtc_low & 0xf0000000 ) >> 28 ) );
                    rtc_low = rtc_low << 4;
                    if( ( i == 3 ) || ( i == 5 ) ) tpu_output_character('-');
                }
                tpu_output_character(' ');
                for( int i = 0; i < 6; i++ ) {
                    tpu_output_character( 48 + ( ( rtc_high & 0xf0000000 ) >> 28 ) );
                    rtc_high = rtc_high << 4;
                    if( ( i == 1 ) || ( i == 3 ) ) tpu_output_character(':');
                }
            }
        }
    }
}

void smtthread( void ) {
    // SETUP STACKPOINTER FOR THE SMT THREAD
    asm volatile ("li sp ,0x4000");
    scrollbars();
    SMTSTOP();
}

// DISPLAY FILENAME, ADD AN ARROW IN FRONT OF DIRECTORIES
void displayfilename( unsigned char *filename, unsigned char type ) {
    char displayname[10], i, j;
    gpu_outputstringcentre( UK_BLUE, 144, 1, "Current PAW File:", 0 );
    memset( displayname, 0, 10 );

    j = type - 1;
    if( j == 1 ) {
        displayname[0] = 16;
    }
    for( i = 0; i < 8; i++ ) {
        if( filename[i] != ' ' ) {
            displayname[j++] = filename[i];
        }
    }
    gpu_outputstringcentre( type == 1 ? UK_BLUE : GREY2, 176, 1, displayname, 2 );
}

// FAT32 FILE BROWSER FOR DIRECTORIES AND .PAW FILES
unsigned char *BOOTRECORD = (unsigned char *) 0x8000000 - 0x200;
PartitionTable *PARTITIONS;

Fat32VolumeID *VOLUMEID = (Fat32VolumeID *)0x8000000 - 0x400;
unsigned int *FAT32table = (unsigned int *)0x8000000 - 0x600;
DirectoryEntry *directorynames = (DirectoryEntry *) ( 0x8000000 - 0x600 - ( sizeof( DirectoryEntry) * 256 ) );

FAT32DirectoryEntry *directorycluster;
unsigned int FAT32startsector, FAT32clustersize, FAT32clusters;

// SDCARD FUNCTIONS
// INTERNAL FUNCTION - WAIT FOR THE SDCARD TO BE READY
void sdcard_wait( void ) {
    while( !*SDCARD_READY );
}

// READ A SECTOR FROM THE SDCARD AND COPY TO MEMORY
void sdcard_readsector( unsigned int sectorAddress, unsigned char *copyAddress ) {
    sdcard_wait();
    *SDCARD_SECTOR = sectorAddress;
    *SDCARD_RESET_BUFFERADDRESS = 0;                // WRITE ANY VALUE TO RESET THE BUFFER ADDRESS
    *SDCARD_READSTART = 1;
    sdcard_wait();

    // USE DMA CONTROLLER TO COPY THE DATA, MODE 4 COPIES FROM A SINGLE ADDRESS TO MULTIPLE
    // EACH READ OF THE SDCARD BUFFER INCREMENTS THE BUFFER ADDRESS
    DMASTART( (const void *restrict)SDCARD_DATA, copyAddress, 512, 4 );
}

void sdcard_readcluster( unsigned int cluster, unsigned char *buffer ) {
     for( unsigned char i = 0; i < FAT32clustersize; i++ ) {
        sdcard_readsector( FAT32clusters + ( cluster - 2 ) * FAT32clustersize + i, buffer + i * 512 );
    }
}

// READ A SECTION OF THE FILE ALLOCATION TABLE INTO MEMORY
unsigned int __basecluster = 0xffffff8;
unsigned int getnextcluster( unsigned int thiscluster ) {
    unsigned int readsector = thiscluster/128;
    if( ( __basecluster == 0xffffff8 ) || ( thiscluster < __basecluster ) || ( thiscluster > __basecluster + 127 ) ) {
        sdcard_readsector( FAT32startsector + readsector, (unsigned char *)FAT32table );
        __basecluster = readsector * 128;
    }
    return( FAT32table[ thiscluster - __basecluster ] );
}

// READ A FILE CLUSTER BY CLUSTER INTO MEMORY
void sdcard_readfile( unsigned int starting_cluster, unsigned char * copyAddress ) {
    unsigned int nextCluster = starting_cluster;
    unsigned char *CLUSTERBUFFER = (unsigned char *)directorycluster;
    int i;

    do {
        sdcard_readcluster( nextCluster, CLUSTERBUFFER );
        for( i = 0; i < FAT32clustersize * 512; i++ ) {
            *copyAddress = CLUSTERBUFFER[i];
            copyAddress++;
        }
        nextCluster = getnextcluster( nextCluster);
    } while( nextCluster < 0xffffff8 );
}

// SORT DIRECTORY ENTRIES BY TYPE AND FIRST CHARACTER
void swapentries( short i, short j ) {
    // SIMPLE BUBBLE SORT, PUT DIRECTORIES FIRST, THEN FILES, IN ALPHABETICAL ORDER
    DirectoryEntry temporary;

    memcpy( &temporary, &directorynames[i], sizeof( DirectoryEntry ) );
    memcpy( &directorynames[i], &directorynames[j], sizeof( DirectoryEntry ) );
    memcpy( &directorynames[j], &temporary, sizeof( DirectoryEntry ) );
}

void sortdirectoryentries( unsigned short entries ) {
    if( !entries )
        return;

    short changes;
    do {
        changes = 0;

        for( int i = 0; i < entries; i++ ) {
            if( directorynames[i].type < directorynames[i+1].type ) {
                swapentries(i,i+1);
                changes++;
            }
            if( ( directorynames[i].type == directorynames[i+1].type ) &&
                ( ( directorynames[i].filename[0] > directorynames[i+1].filename[0] ) ||
                ( ( directorynames[i].filename[0] == directorynames[i+1].filename[0] ) && ( directorynames[i].filename[1] > directorynames[i+1].filename[1] ) ) )
            ) {
                swapentries(i,i+1);
                changes++;
            }
        }
    } while( changes );
}

// WAIT FOR USER TO SELECT A VALID PAW FILE, BROWSING SUBDIRECTORIES
unsigned int filebrowser( int startdirectorycluster, int rootdirectorycluster ) {
    unsigned int thisdirectorycluster = startdirectorycluster;
    FAT32DirectoryEntry *fileentry;

    unsigned char rereaddirectory = 1;
    unsigned short entries, present_entry;
    int temp;

    directorycluster = ( FAT32DirectoryEntry * )directorynames - FAT32clustersize * 512;

    while( 1 ) {
        if( rereaddirectory ) {
            entries = 0xffff; present_entry = 0;
            fileentry = (FAT32DirectoryEntry *) directorycluster;
            memset( &directorynames[0], 0, sizeof( DirectoryEntry ) * 256 );
        }

        while( rereaddirectory ) {
            sdcard_readcluster( thisdirectorycluster, (unsigned char *)directorycluster );

            for( int i = 0; i < 16 * FAT32clustersize; i++ ) {
                if( ( fileentry[i].filename[0] != 0x00 ) && ( fileentry[i].filename[0] != 0xe5 ) ) {
                    // LOG ITEM INTO directorynames
                    if( fileentry[i].attributes &  0x10 ) {
                        // DIRECTORY, IGNORING "." and ".."
                        if( fileentry[i].filename[0] != '.' ) {
                            entries++;
                            memcpy( &directorynames[entries], &fileentry[i].filename[0], 11 );
                            directorynames[entries].type = 2;
                            directorynames[entries].starting_cluster = ( fileentry[i].starting_cluster_high << 16 )+ fileentry[i].starting_cluster_low;
                        }
                    } else {
                        if( fileentry[i].attributes & 0x08 ) {
                            // VOLUMEID
                        } else {
                            if( fileentry[i].attributes != 0x0f ) {
                                // SHORT FILE NAME ENTRY
                                if( ( ( fileentry[i].ext[0] == 'P' ) ) &&
                                    ( ( fileentry[i].ext[1] == 'A' ) ) &&
                                    ( ( fileentry[i].ext[2] == 'W' ) ) ) {
                                        entries++;
                                        memcpy( &directorynames[entries], &fileentry[i].filename[0], 11 );
                                        directorynames[entries].type = 1;
                                        directorynames[entries].starting_cluster = ( fileentry[i].starting_cluster_high << 16 )+ fileentry[i].starting_cluster_low;
                                }
                            }
                        }
                    }
                }
            }

            // MOVE TO THE NEXT CLUSTER OF THE DIRECTORY
            if( getnextcluster( thisdirectorycluster ) >= 0xffffff8 ) {
                rereaddirectory = 0;
            } else {
                thisdirectorycluster = getnextcluster( thisdirectorycluster );
            }
        }

        if( entries == 0xffff ) {
            // NO ENTRIES FOUND
            gpu_outputstringcentre( RED, 176, 1, "NO FILES", 1 );
            gpu_outputstringcentre( RED, 192, 1, "IN THIS DIRECTORY", 1 );
            beep( CHANNEL_BOTH, WAVE_SAW, 27, 1000 );
            sleep( 1000 );
            return(0);
        }

        sortdirectoryentries( entries );

        while( !rereaddirectory ) {
            displayfilename( directorynames[present_entry].filename, directorynames[present_entry].type );

            // WAIT FOR BUTTON, AND WAIT FOR RELEASE TO STOP ACCIDENTAL DOUBLE PRESSES
            unsigned short buttons = get_buttons();
            while( buttons == 1 ) { buttons = get_buttons(); }
            while( get_buttons() != 1 ) {} sleep( 100 );
            if( buttons & 64 ) {
                // MOVE RIGHT
                if( present_entry == entries ) { present_entry = 0; } else { present_entry++; }
            }
            if( buttons & 32 ) {
                // MOVE LEFT
                if( present_entry == 0 ) { present_entry = entries; } else { present_entry--; }
           }
            if( buttons & 8 ) {
                // MOVE UP
                if( startdirectorycluster != rootdirectorycluster ) { return(0); }
           }
            if( buttons & 2 ) {
                // SELECTED
                switch( directorynames[present_entry].type ) {
                    case 1:
                        return( directorynames[present_entry].starting_cluster );
                        break;
                    case 2:
                        temp = filebrowser( directorynames[present_entry].starting_cluster, rootdirectorycluster );
                        if( temp ) {
                            return( temp );
                        } else {
                            rereaddirectory = 1;
                        }
                }
            }
        }
    }
}

extern int _bss_start, _bss_end;
unsigned char chime[] = { 75, 83, 89, 0 };

int main( void ) {
    unsigned int isa;
    unsigned short i, j, x, y;
    unsigned short selectedfile = 0;

    // STOP SMT AND PIXELBLOCK
    SMTSTOP(); *PB_STOP = *PB_MODE = 0;

    // CLEAR MEMORY
    memset( &_bss_start, 0, &_bss_end - &_bss_end );

    // RESET THE DISPLAY, AUDIO AND VOLUME
    reset_display(); set_background( UK_BLUE, UK_GOLD, 1 );
    beep( 3, 0, 0, 0 ); volume( 7, 7 );

    // KEYBOARD INTO JOYSTICK MODE
    *PS2_MODE = 0; *PS2_CAPSLOCK = 0; *PS2_NUMLOCK = 0;

    // DRAW LOGO AND SDCARD
     draw_paws_logo();

     // OUTPUT SOC & ISA CAPABILITIES
    tpu_set( 1, 59, TRANSPARENT, UK_BLUE ); tpu_outputbinary( *PAWSMAGIC, 32 );
    tpu_set( 47,59, TRANSPARENT, UK_BLUE ); tpu_outputbinary( CSRisa(), 32 );

    // COLOUR BARS ON THE TILEMAP - SCROLL WITH SMT THREAD - SET VIA DMA 5 SINGLE SOURCE TO SINGLE DESTINATION
    for( i = 0; i < 42; i++ ) {
        *LOWER_TM_WRITER_TILE_NUMBER = i + 1; *DMASET = 65+i; DMASTART( (const void *restrict)DMASET, (void *restrict)LOWER_TM_WRITER_COLOUR, 256, 5 );
        *UPPER_TM_WRITER_TILE_NUMBER = i + 1; *DMASET = 255-i; DMASTART( (const void *restrict)DMASET, (void *restrict)UPPER_TM_WRITER_COLOUR, 256, 5 );
        set_tilemap_tile( 0, i, 21, i+1, 0 );
        set_tilemap_tile( 1, i, 27, i+1, 0 );
    }
    SMTSTART( (unsigned int )smtthread );

    gpu_outputstring( WHITE, 66, 2, 1, "PAWSv2", 2 );
    gpu_outputstring( WHITE, 70, 34, 0, "Risc-V RV32GC+B CPU", 0 );
    gpu_outputstringcentre( UK_BLUE, 224, 0, "PAWSv2 for ULX3S by Rob S in Silice", 0);

    // CLEAR UART AND PS/2 BUFFERS
    while( *UART_STATUS & 1 ) { (void)*UART_DATA; }
    while( *PS2_AVAILABLE ) { (void)*PS2_DATA; }

    gpu_outputstringcentre( RED, 72, 0, "Waiting for SDCARD", 0 );
    gpu_outputstringcentre( RED, 80, 0, "Press RESET if not detected", 0 );
    sdcard_readsector( 0, BOOTRECORD );
    PARTITIONS = (PartitionTable *) &BOOTRECORD[ 0x1BE ];

    gpu_outputstringcentre( GREEN, 72, 0, "Ready", 0 );
    gpu_outputstringcentre( RED, 80, 0, "", 0 );

    // NO FAT16 PARTITION FOUND
    if( ( PARTITIONS[0].partition_type != 0x0b ) && ( PARTITIONS[0].partition_type != 0x0c ) ) {
        gpu_outputstringcentre( RED, 72, 1, "ERROR", 2 );
        gpu_outputstringcentre( RED, 120, 1, "Please Insert SDCARD", 0 );
        gpu_outputstringcentre( RED, 128, 1, "WITH FAT32 PARTITION", 0 );
        gpu_outputstringcentre( RED, 136, 1, "Press RESET", 0 );
        while(1) {}
    }

    // READ VOLUMEID FOR PARTITION 0
    sdcard_readsector( PARTITIONS[0].start_sector, (unsigned char *)VOLUMEID );
    FAT32startsector = PARTITIONS[0].start_sector + VOLUMEID -> reserved_sectors;
    FAT32clusters = PARTITIONS[0].start_sector + VOLUMEID -> reserved_sectors + ( VOLUMEID -> number_of_fats * VOLUMEID -> fat32_size_sectors );
    FAT32clustersize = VOLUMEID -> sectors_per_cluster;

    // FILE SELECTOR
    gpu_outputstringcentre( WHITE, 72, 1, "Select File", 0 );
    gpu_outputstringcentre( WHITE, 88, 0, "SELECT USING \x0f", 0 );
    gpu_outputstringcentre( WHITE, 96, 0, "SCROLL USING \x1b & \x1a", 0 );
    gpu_outputstringcentre( WHITE, 104, 0, "RETURN FROM DIRECTORY USING \x18", 0 );
    gpu_outputstringcentre( RED, 144, 1, "No Files Found", 0 );

    // CALL FILEBROWSER
    unsigned int starting_cluster = filebrowser( VOLUMEID -> startof_root, VOLUMEID -> startof_root );
    if( !starting_cluster ) {
        while(1) {}
    }

    // ACKNOWLEDGE SELECTION AND STOP SMT TO ALLOW FASTER LOADING
    sample_upload( CHANNEL_BOTH, 4, &chime[0] ); beep( CHANNEL_BOTH, WAVE_SINE | WAVE_SAMPLE, 0, 63 ); SMTSTOP();

    *LEDS = 255;
    gpu_outputstringcentre( WHITE, 72, 1, "PAW File", 0 );
    gpu_outputstringcentre( WHITE, 80, 1, "SELECTED", 0 );
    gpu_outputstringcentre( WHITE, 88, 0, "", 0 );
    gpu_outputstringcentre( WHITE, 96, 0, "", 0 );
    gpu_outputstringcentre( WHITE, 104, 0, "", 0 );
    sleep( 500 );
    gpu_outputstringcentre( WHITE, 80, 1, "LOADING", 0 );
    asm volatile ("fence iorw,iorw"); sdcard_readfile( starting_cluster, (unsigned char *)0x4000000 ); asm volatile ("fence.i");
    gpu_outputstringcentre( WHITE, 72, 1, "LOADED", 0 );
    gpu_outputstringcentre( WHITE, 80, 1, "LAUNCHING", 0 );
    sleep(500);

    // RESET THE DISPLAY AND TURN OFF LEDS
    reset_display();
    set_background( BLACK, BLACK, BKG_SOLID );
    *LEDS = 0;

    // CALL SDRAM LOADED PROGRAM
    ((void(*)(void))0x4000000)();
    // RETURN TO BIOS IF PROGRAM EXITS
    ((void(*)(void))0x0)();
}
