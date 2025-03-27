#include <stdint.h>
#include <stdint.h>
#include "PAWS.h"
#include "PAWS_BIOS_LIBRARY.h"

void draw_paws_logo( void ) {
    set_blitter_bitmap( 3, &PAWSLOGO[0] );
    gpu_blit( UK_GOLD, 2, 2, 3, 2 );
}

void reset_display( void ) {
    // WAIT FOR THE GPU TO FINISH
    gpu_pixelblock_stop();
    while( !*GPU_FINISHED );

    set_background( BLACK, BLACK, BKG_SOLID );
    *GPU_DITHERMODE = 0; *CROP_LEFT = 0; *CROP_RIGHT = 319; *CROP_TOP = 0; *CROP_BOTTOM = 239;
    *FRAMEBUFFER_DRAW = 3; gpu_cs(); while( !*GPU_FINISHED );
    *FRAMEBUFFER_DRAW = 1; *FRAMEBUFFER_DISPLAY = 1; *BITMAP_DISPLAY256 = 0; *PALETTEACTIVE = 0;
    *SCREENMODE = 0; *COLOUR = 0; *REZ = 0; *DIMMER = 0; *STATUS_DISPLAY = 1; *STATUS_BACKGROUND = 0x40;
    *TPU_CURSOR = 0; tpu_cs();
    tm_cs( 0 ); tm_cs( 1 );
    *UPPER_TM_SCROLLWRAPAMOUNT = *LOWER_TM_SCROLLAMOUNT = 1;
    for( unsigned short i = 0; i < 32; i++ ) {
        LOWER_SPRITE_ACTIVE[i] = UPPER_SPRITE_ACTIVE[i] = 0;
    }
}

// DISPLAY FILENAME, ADD AN ARROW IN FRONT OF DIRECTORIES
void displayfilename( unsigned char *filename, unsigned char type ) {
    char displayname[10], i, j;
    gpu_outputstringcentre( UK_BLUE, 144, 1, "P64 File:", 0 );
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

// FAT32 FILE BROWSER FOR DIRECTORIES AND .P64 FILES
unsigned char *BOOTRECORD;;
PartitionTable *PARTITIONS;

Fat32VolumeID *VOLUMEID;
unsigned int *FAT32table;
DirectoryEntry *directorynames;

FAT32DirectoryEntry *directorycluster;
unsigned int FAT32startsector, FAT32clustersize, FAT32clusters;

// SDCARD FUNCTIONS
// INTERNAL FUNCTION - WAIT FOR THE SDCARD TO BE READY
void sdcard_wait( void ) {
    while( !*SDCARD_READY );
}

void sdcard_readsector( unsigned int sectorAddress, unsigned char *copyAddress ) {                                              // READ A SINGLE SECTOR FROM THE SDCARD AND COPY TO MEMORY
    sdcard_wait();
    *SDCARD_SECTOR = sectorAddress;
    *SDCARD_RESET_BUFFERADDRESS = 0;                // WRITE ANY VALUE TO RESET THE BUFFER ADDRESS
    *SDCARD_READSTART = 1;
    sdcard_wait();

    // USE DMA CONTROLLER TO COPY THE DATA, MODE 4 COPIES FROM A SINGLE ADDRESS TO MULTIPLE
    // EACH READ OF THE SDCARD BUFFER INCREMENTS THE BUFFER ADDRESS
    DMASTART( (const void *restrict)SDCARD_IN_DATA, copyAddress, 512, DMA_FROM_IO );
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

    do {
        sdcard_readcluster( nextCluster, CLUSTERBUFFER );
        memcpy( copyAddress, CLUSTERBUFFER, FAT32clustersize * 512 ); copyAddress += FAT32clustersize * 512;
        nextCluster = getnextcluster( nextCluster);
    } while( nextCluster < 0xffffff8 );
}

// SORT DIRECTORY ENTRIES BY TYPE AND FIRST CHARACTER
void swapentries( unsigned int i, unsigned int j ) {
    // SIMPLE BUBBLE SORT, PUT DIRECTORIES FIRST, THEN FILES, IN ALPHABETICAL ORDER
    DirectoryEntry temporary;

    memcpy( &temporary, &directorynames[i], sizeof( DirectoryEntry ) );
    memcpy( &directorynames[i], &directorynames[j], sizeof( DirectoryEntry ) );
    memcpy( &directorynames[j], &temporary, sizeof( DirectoryEntry ) );
}

void sortdirectoryentries( unsigned int entries ) {
    if( !entries )
        return;

    int changes;
    do {
        changes = 0;

        for( int i = 0; i < entries; i++ ) {
            if( directorynames[i].type < directorynames[i+1].type ) {
                swapentries(i,i+1);
                changes++;
            }
            if( ( directorynames[i].type == directorynames[i+1].type ) && ( _rv64_rev8( directorynames[i].filename.sortvalue ) > _rv64_rev8( directorynames[i+1].filename.sortvalue ) ) ) {
                swapentries(i,i+1);
                changes++;
            }
        }
    } while( changes );
}

// WAIT FOR USER TO SELECT A VALID PAW FILE, BROWSING SUBDIRECTORIES
unsigned int filebrowser( int startdirectorycluster, int rootdirectorycluster ) {
    unsigned int thisdirectorycluster = startdirectorycluster, entries, present_entry, temp;
    unsigned char rereaddirectory = 1;
    FAT32DirectoryEntry *fileentry;

    while( 1 ) {
        if( rereaddirectory ) {
            entries = -1; present_entry = 0;
            fileentry = (FAT32DirectoryEntry *) directorycluster;
            memset( &directorynames[0], 0, sizeof( DirectoryEntry ) * 256 );
        }

        while( rereaddirectory ) {
            sdcard_readcluster( thisdirectorycluster, (unsigned char *)directorycluster );

            for( unsigned int i = 0; i < 16 * FAT32clustersize; i++ ) {
                if( ( fileentry[i].filename[0] != 0x00 ) && ( fileentry[i].filename[0] != 0xe5 ) ) {
                    // LOG ITEM INTO directorynames
                    if( fileentry[i].attributes & 0x10 ) {
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
                                    ( ( fileentry[i].ext[1] == '6' ) ) &&
                                    ( ( fileentry[i].ext[2] == '4' ) ) ) {
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

        if( entries == -1 ) {
            // NO ENTRIES FOUND
            gpu_outputstringcentre( RED, 176, 1, "NO FILES", 1 );
            gpu_outputstringcentre( RED, 192, 1, "IN DIRECTORY", 1 );
            beep( CHANNEL_BOTH, WAVE_SAW, 27, 1000 );
            sleep( 1000 );
            return(0);
        }

        sortdirectoryentries( entries );

        while( !rereaddirectory ) {
            displayfilename( directorynames[present_entry].filename.string, directorynames[present_entry].type );

            // WAIT FOR BUTTON, AND WAIT FOR RELEASE TO STOP ACCIDENTAL DOUBLE PRESSES
            unsigned short buttons = get_buttons();
            while( buttons == 1 ) { buttons = get_buttons(); }
            while( get_buttons() != 1 ) {} sleep( 100 );
            if( ( buttons & 64 ) >> 6 ) {
                // MOVE RIGHT
                if( present_entry == entries ) { present_entry = 0; } else { present_entry++; }
            }
            if( ( buttons & 32 ) >> 5 ) {
                // MOVE LEFT
                if( present_entry == 0 ) { present_entry = entries; } else { present_entry--; }
            }
            if( ( buttons & 8 ) >> 3 ) {
                // MOVE UP
                if( startdirectorycluster != rootdirectorycluster ) { return(0); }
            }
            if( ( buttons & 2 ) >> 1 ) {
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

// SMT THREAD TO MOVE COLOUR BARS AND FLASH LEDS
void printclock( void ) {
    long rtc;

    tpu_set( 0, 17, TRANSPARENT, WHITE, 1 );
    rtc = *RTC + 0x2000000000000000;
    for( int i = 0; i < 16; i++ ) {
        rtc = _rv64_rol( rtc, 4 );
        switch(i) {
            case 8: case 9: break;
            default: tpu_output_character( 48 + ( rtc & 0xf ) );
        }
        switch(i) {
            case 3: case 5: tpu_output_character('-'); break;
            case 7: tpu_output_character(' '); break;
            case 11: case 13: tpu_output_character(':'); break;
        }
    }
}

__attribute__((used)) void scrollbars( void ) {
    unsigned char leds = 1;
    int count = 0, direction = 0, ledcount = 0;

    while(1) {
        await_vblank(); count++;
        if( count == 64 ) {
            *LOWER_TM_SCROLLWRAPCLEAR = 3; *UPPER_TM_SCROLLWRAPCLEAR = 1;
            count = 0;
            ledcount++;
            if( ledcount == 32 ) {
                ledcount = 0;
                if( direction ) {
                    if( leds == 1 ) { direction = 0; } else { leds = leds >> 1; }
                } else {
                    if( ( leds & 128 ) != 0 ) { direction = 1; } else { leds = ( *SDCARD_READY ) ? ( leds << 1 ) : ( leds << 1 ) + 1; }
                }
                *LEDS = leds;
                printclock();
            }
        }
    }
}

void smtthread( void ) {
    // SETUP STACKPOINTER FOR THE SMT THREAD
    asm volatile ("li   sp ,0xff08");               // ADDRESS OF SMT STACKTOP
    asm volatile ("lwu  sp, (sp)");                 // LOAD FROM SMT STACKTOP
    asm volatile ("j    scrollbars");
}

extern int _bss_start, _bss_end;
unsigned char chime[] = { 75, 83, 89, 0 };

int main( void ) {
    unsigned int i, j, x, y, selectedfile = 0;

    // STOP SMT, PIXELBLOCK AND AUDIO DMA
    SMTSTOP(); *PB_STOP = *PB_MODE = 0;
    *AUDIO_DMA_L_STATUS = 1; *AUDIO_DMA_R_STATUS = 1;

    // CLEAR BSS MEMORY AND DEFINE HEAPEND AND ALLOCATE FAT32 MEMORY
    memset( &_bss_start, 0, &_bss_end - &_bss_start );
    HEAPEND = (void *)((long)*RAMTOP);
    BOOTRECORD = bios_malloc( 512 );
    PARTITIONS = (PartitionTable *)&BOOTRECORD[446];
    VOLUMEID = bios_malloc( 512 );
    FAT32table = bios_malloc( 512 );
    directorynames = (DirectoryEntry *)bios_malloc( sizeof( DirectoryEntry ) * 256 );

    // RESET THE DISPLAY, AUDIO AND VOLUME
    reset_display(); set_background( UK_BLUE, UK_GOLD, 1 );
    beep( 3, 0, 0, 0 ); volume( 7, 7 );

    // KEYBOARD INTO JOYSTICK MODE, RESET MOUSE
    *PS2_MODE = 0; *PS2_CAPSLOCK = 0; *PS2_NUMLOCK = 0; *MOUSE_RESET = 0;

    // DRAW LOGO AND SDCARD
    draw_paws_logo();

    // COLOUR BARS ON THE TILEMAP - SCROLL WITH SMT THREAD - SET VIA DMA 5 SINGLE SOURCE TO SINGLE DESTINATION
    for( i = 0; i < 63; i++ ) {
        *LOWER_TM_WRITER_TILE_NUMBER = i + 1; *DMASET = 65+i; DMASTART( (const void *restrict)DMASET, (void *restrict)LOWER_TM_WRITER_COLOUR, 256, DMA_SET_TO_S );
        *UPPER_TM_WRITER_TILE_NUMBER = i + 1; *DMASET = 255-i; DMASTART( (const void *restrict)DMASET, (void *restrict)UPPER_TM_WRITER_COLOUR, 256, DMA_SET_TO_S );
        set_tilemap_tile( 0, i, 18, i+1, 0 );
        set_tilemap_tile( 1, i, 30, i+1, 0 );
    }
    SMTSTART( smtthread );

    gpu_outputstring( WHITE, 66, 2, 1, "PAWSv2", 2 );
    gpu_outputstring( WHITE, 66, 34, 1, "Risc-V RV64GC+", 0 );
    gpu_outputstringcentre( UK_BLUE, 224, 0, "For ULX3S by Rob S in Silice", 0);

    // CLEAR UART AND PS/2 BUFFERS
    while( ( *UART_STATUS & 1 ) | *PS2_AVAILABLE ) { (void)*UART_DATA;( void)*PS2_DATA; }

    gpu_outputstringcentre( RED, 72, 1, "SDCARD?", 0 );
    gpu_outputstringcentre( RED, 88, 0, "RESET", 0 );
    sdcard_readsector( 0, BOOTRECORD );
    PARTITIONS = (PartitionTable *) &BOOTRECORD[ 0x1BE ];

    // NO FAT16 PARTITION FOUND
    if( ( PARTITIONS[0].partition_type != 0x0b ) && ( PARTITIONS[0].partition_type != 0x0c ) ) {
        gpu_outputstringcentre( RED, 72, 1, "ERROR", 2 );
        gpu_outputstringcentre( RED, 120, 1, "Insert SDCARD", 0 );
        gpu_outputstringcentre( RED, 128, 1, "WITH FAT32 PARTITION", 0 );
        gpu_outputstringcentre( RED, 136, 1, "RESET", 0 );
        while(1) {}
    }

    // READ VOLUMEID FOR PARTITION 0
    sdcard_readsector( PARTITIONS[0].start_sector, (unsigned char *)VOLUMEID );
    FAT32startsector = PARTITIONS[0].start_sector + VOLUMEID -> reserved_sectors;
    FAT32clusters = PARTITIONS[0].start_sector + VOLUMEID -> reserved_sectors + ( VOLUMEID -> number_of_fats * VOLUMEID -> fat32_size_sectors );
    FAT32clustersize = VOLUMEID -> sectors_per_cluster;
    directorycluster = bios_malloc( FAT32clustersize * 512 );

    // FILE SELECTOR
    gpu_outputstringcentre( WHITE, 72, 1, "Select File", 0 );
    gpu_outputstringcentre( WHITE, 88, 0, "SELECT \x0f", 0 );
    gpu_outputstringcentre( WHITE, 96, 0, "SCROLL \x1b & \x1a", 0 );
    gpu_outputstringcentre( WHITE, 104, 0, "UP A DIRECTORY \x18", 0 );
    gpu_outputstringcentre( RED, 144, 1, "No Files", 0 );

    // CALL FILEBROWSER
    int starting_cluster = filebrowser( VOLUMEID -> startof_root, VOLUMEID -> startof_root );
    if( !starting_cluster ) {
        while(1) {}
    }

    // ACKNOWLEDGE SELECTION AND STOP SMT TO ALLOW FASTER LOADING
    sample_upload( CHANNEL_BOTH, 4, &chime[0] ); beep( CHANNEL_BOTH, WAVE_SINE | WAVE_SAMPLE, 0, 63 ); SMTSTOP();

    *LEDS = 255;
    gpu_outputstringcentre( WHITE, 72, 1, "P64 File", 0 );
    gpu_outputstringcentre( WHITE, 80, 1, "SELECTED", 0 );
    gpu_rectangle( TRANSPARENT, 0, 80, 319, 111 );
    sleep( 500 );
    gpu_outputstringcentre( WHITE, 80, 1, "LOADING", 0 );
    asm volatile ("fence iorw,iorw"); sdcard_readfile( starting_cluster, (unsigned char *)((long)*RAMBASE) ); asm volatile ("fence.i");
    gpu_outputstringcentre( WHITE, 72, 1, "LOADED", 0 );
    gpu_outputstringcentre( WHITE, 80, 1, "LAUNCHING", 0 );
    sleep(500);

    // RESET THE DISPLAY, TURN OFF LEDS
    reset_display();
    *LEDS = 0;

    // CALL SDRAM LOADED PROGRAM
    ((void(*)(void))((long)*RAMBASE))();
    // RETURN TO BIOS IF PROGRAM EXITS
    ((void(*)(void))0x0)();
}
