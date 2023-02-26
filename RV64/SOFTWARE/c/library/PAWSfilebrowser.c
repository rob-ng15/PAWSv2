// READ A FILE USING THE SIMPLE FILE BROWSER
// WILL ALLOW SELECTION OF A FILE FROM THE SDCARD, INCLUDING SUB-DIRECTORIES
// ALLOCATES MEMORY FOR THE FILE, AND LOADS INTO MEMORY
// FAT32 File System for the file selector

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <PAWSdefinitions.h>
#include "PAWSintrinsics.h"

// PAWSlibrary functions
extern void beep( unsigned char, unsigned char, unsigned char, unsigned short );
extern unsigned short get_buttons( void );
extern void gpu_character_blit( unsigned char, short, short, unsigned short, unsigned char, unsigned char );
extern void gpu_rectangle( unsigned char, short, short, short, short );
extern void sdcard_readsector( unsigned int sectorAddress, unsigned char *copyAddress );
extern void sleep1khz( unsigned short, unsigned char );

union fname {                                                                                                                   // FILENAME AS CHAR OR LONG TO ALLOW FOR QUICK SORTING
    unsigned char string[8];
    long sortvalue;
};


typedef struct {                                                                                                                // SDCARD PARTITION STRUCTURE
    unsigned char first_byte;
    unsigned char start_chs[3];
    unsigned char partition_type;
    unsigned char end_chs[3];
    unsigned int start_sector;
    unsigned int length_sectors;
} __attribute((packed)) PartitionTable;

typedef struct {                                                                                                                // FAT32 VOLUME ID
    unsigned char   jmp[3];
    unsigned char   oem[8];
    unsigned short  sector_size;
    unsigned char   sectors_per_cluster;
    unsigned short  reserved_sectors;
    unsigned char   number_of_fats;
    unsigned short  root_dir_entries;
    unsigned short  total_sectors_short;                                                                                        // if zero, later field is used
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

typedef struct {                                                                                                                // ENTRY IN A FAT32 DIRECTORY
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

typedef struct {                                                                                                                // STORAGE FOR ENTRIES EXTRACTED FROM A FAT32 DIRECTORY
    union fname     filename;
    unsigned char   ext[3];
    unsigned char   type;
    unsigned int    starting_cluster;
    unsigned int    file_size;
} __attribute((packed)) DirectoryEntry;

unsigned char *BOOTRECORD = NULL;
PartitionTable *PARTITIONS = NULL;
Fat32VolumeID *VolumeID = NULL;
FAT32DirectoryEntry *directorycluster = NULL;
unsigned int FAT32startsector, FAT32clustersize, FAT32clusters, *FAT32table = NULL;
DirectoryEntry *directorynames;

// DISPLAY A FILENAME CLEARING THE AREA BEHIND IT
void gpu_outputstring( unsigned char colour, short x, short y, unsigned char bold, char *s, unsigned char size ) {
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256 : 0 ) + *s++, size, 0 );
        x = x + ( 8 << size );
    }
}
void gpu_outputstringcentre( unsigned char colour, short y, unsigned char bold, char *s, unsigned char size ) {
    gpu_rectangle( TRANSPARENT, 0, y, 319, y + ( 8 << size ) - 1 );
    gpu_outputstring( colour, 160 - ( ( ( 8 << size ) * strlen(s) ) >> 1) , y, bold, s, size );
}
void displayfilename( unsigned char *filename, unsigned char type ) {
    char displayname[10], i, j;
    gpu_outputstringcentre( WHITE, 144, 0, "Current File:", 0 );
    for( i = 0; i < 10; i++ ) {
        displayname[i] = 0;
    }
    j = type - 1;
    if( j == 1 ) {
        displayname[0] = 16;
    }
    for( i = 0; i < 8; i++ ) {
        if( filename[i] != ' ' ) {
            displayname[j++] = filename[i];
        }
    }
    gpu_outputstringcentre( type == 1 ? WHITE : GREY5, 176, 0, displayname, 2 );
}

// HANDLE READING OF CLUSTERS
unsigned int __basecluster = 0xffffff8;
unsigned int getnextcluster( unsigned int thiscluster ) {
    unsigned int readsector = thiscluster/128;
    if( ( __basecluster == 0xffffff8 ) || ( thiscluster < __basecluster ) || ( thiscluster > __basecluster + 127 ) ) {
        sdcard_readsector( FAT32startsector + readsector, (unsigned char *)FAT32table );
        __basecluster = readsector * 128;
    }
    return( FAT32table[ thiscluster - __basecluster ] );
}
void readcluster( unsigned int cluster, unsigned char *buffer ) {
     for( unsigned char i = 0; i < FAT32clustersize; i++ ) {
        sdcard_readsector( FAT32clusters + ( cluster - 2 ) * FAT32clustersize + i, buffer + i * 512 );
    }
}

// HANDLE READING OF A FILE INTO MEMORY
void readfile( unsigned int starting_cluster, unsigned char *copyAddress ) {
    unsigned int nextCluster = starting_cluster;
    unsigned char *CLUSTERBUFFER = (unsigned char *)directorycluster;
    int i;

    do {
        readcluster( nextCluster, CLUSTERBUFFER );
        memcpy( copyAddress, CLUSTERBUFFER, FAT32clustersize * 512 ); copyAddress += FAT32clustersize * 512;
        nextCluster = getnextcluster( nextCluster);
    } while( nextCluster < 0xffffff8 );
}

// SORT DIRECTORY ENTRIES INTO ASCII ORDER
void swapentries( short i, short j ) {
    DirectoryEntry temporary;

    paws_memcpy( &temporary, &directorynames[i], sizeof( DirectoryEntry ) );
    paws_memcpy( &directorynames[i], &directorynames[j], sizeof( DirectoryEntry ) );
    paws_memcpy( &directorynames[j], &temporary, sizeof( DirectoryEntry ) );
}
void sortdirectoryentries( unsigned short entries ) {
    // SIMPLE BUBBLE SORT, PUT DIRECTORIES FIRST, THEN FILES, IN ALPHABETICAL ORDER
    // FILENAMES ARE STORED AS 8 ASCII CHARACTERS WITH SPACES AS PADDING
    // BY TREATING FILENAME AS AN UNSIGNED LONG AND DOING A BYTE REVRSE
    // SIMPLE COMPARISONS CAN BE DONE TO QUICKLY SORT THE FILENAMES
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

            if( ( directorynames[i].type == directorynames[i+1].type ) && ( _rv64_rev8( directorynames[i].filename.sortvalue ) > _rv64_rev8( directorynames[i+1].filename.sortvalue ) ) ) {
                swapentries(i,i+1);
                changes++;
            }
        }
    } while( changes );
}

// READ THE CONTENTS OF A DIRECTORY FROM THE SDCARD
// PROCESS THE ENTRIES AND PUT INTO THE directorynames ARRAY
int read_directory( int startdirectorycluster, char *extension ) {
    unsigned int thisdirectorycluster = startdirectorycluster, rereaddirectory = 1, entries = -1;
    FAT32DirectoryEntry *fileentry;

    fileentry = (FAT32DirectoryEntry *) directorycluster;                                                                       // CLEAR PRESENT DIRECTORY ENTRIES
    paws_memset( &directorynames[0], 0, sizeof( DirectoryEntry ) * 256 );

    while( rereaddirectory ) {
        readcluster( thisdirectorycluster, (unsigned char *)directorycluster );

        for( int i = 0; i < 16 * FAT32clustersize; i++ ) {
            switch( fileentry[i].filename[0] ) {
                case 0x00:
                case 0xe5:
                    break;
                default:
                    if( fileentry[i].attributes & 0x10 ) {                                                                      // DIRECTORY FOUND, IGNORING "." and ".."
                        if( fileentry[i].filename[0] != '.' ) {
                            entries++;
                            paws_memcpy( &directorynames[entries], &fileentry[i].filename[0], 11 );
                            directorynames[entries].type = 2;
                            directorynames[entries].starting_cluster = ( fileentry[i].starting_cluster_high << 16 )+ fileentry[i].starting_cluster_low;
                        }
                    } else {
                        if( fileentry[i].attributes & 0x08 ) {                                                                  // VOLUME ID FOUND, IGNORING
                        } else {
                            if( fileentry[i].attributes != 0x0f ) {                                                             // SHORT FILE NAME ENTRY FOUND
                                if( ( fileentry[i].ext[0] == extension[0] ) &&                                                  // CHECK ENTENSION MATCHES
                                    ( fileentry[i].ext[1] == extension[1] ) &&
                                    ( fileentry[i].ext[2] == extension[2] ) ) {
                                    entries++;
                                    paws_memcpy( &directorynames[entries], &fileentry[i].filename[0], 11 );
                                    directorynames[entries].type = 1;
                                    directorynames[entries].starting_cluster = ( fileentry[i].starting_cluster_high << 16 )+ fileentry[i].starting_cluster_low;
                                    directorynames[entries].file_size = fileentry[i].file_size;
                                }
                            }
                        }
                    }
                    break;
            }
        }

        // MOVE TO THE NEXT CLUSTER OF THE DIRECTORY
        thisdirectorycluster = getnextcluster( thisdirectorycluster );
        if( thisdirectorycluster >= 0xffffff8 ) {
            rereaddirectory = 0;
        }
    }

    return( entries );
}

// FILE BROWSER
unsigned int filebrowser( char *message, char *extension, int startdirectorycluster, int rootdirectorycluster, unsigned int *filesize ) {
    unsigned int thisdirectorycluster = startdirectorycluster, rereaddirectory = 1, entries, present_entry, temp;

    while( 1 ) {
        if( rereaddirectory ) {
            present_entry = 0; rereaddirectory = 0;
            entries = read_directory( thisdirectorycluster, extension );
        }

        if( entries == -1 ) {
            // NO ENTRIES FOUND
            gpu_outputstringcentre( RED, 176, 1, "NO FILES", 1 );
            gpu_outputstringcentre( RED, 192, 1, "IN THIS DIRECTORY", 1 );
            beep( CHANNEL_BOTH, WAVE_SAW, 27, 1000 );
            sleep1khz( 1000, 0 );
            return(0);
        } else {
            sortdirectoryentries( entries );
            gpu_outputstringcentre( WHITE, 88, 0, "Select directory/file using \x0f", 0 );
            gpu_outputstringcentre( WHITE, 96, 0, "Scroll left/right using \x1b & \x1a", 0 );
            gpu_outputstringcentre( WHITE, 104, 0, "Move UP a directory using \x18", 0 );
            gpu_outputstringcentre( WHITE, 128, 1, message, 0 );
        }

        while( !rereaddirectory ) {
            displayfilename( directorynames[present_entry].filename.string, directorynames[present_entry].type );

            unsigned short buttons = get_buttons();
            while( buttons == 1 ) { buttons = get_buttons(); }
            while( get_buttons() != 1 ) {} sleep1khz( 100, 0 );
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
                        if( !(*filesize) ) *filesize = directorynames[present_entry].file_size;
                        return( directorynames[present_entry].starting_cluster );
                        break;
                    case 2:
                        temp = filebrowser( message, extension, directorynames[present_entry].starting_cluster, rootdirectorycluster, filesize );
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

unsigned int move_todirectory( char *path, char *extension, unsigned int root_directory ) {
    unsigned int base_directory = root_directory;                                                                               // SET BASE DIRECTORY TO ROOT IN CASE REQUESTED DIRECTORY NOT FOUND
    int entries = read_directory( base_directory, extension );                                                                  // READ THE ROOT DIRECTORY
    int path_length = strlen( path ), path_position = 0, i = 0;

    if( ( entries == -1 ) || ( path_length & 0x7 ) ) { return( base_directory ); }                                              // PATH MUST BE MULTIPLE OF 8 CHARACTERS, AND MUST BE ENTRIES

    // SCAN THROUGH FOR EACH OF THE REQUESTED DIRECTORYS IN THE PATH
    union fname *presentpathname = (union fname *)path;                                                                         // MOVE TO THE FIRST ENTRY IN THE PATH

    do {
        if( presentpathname->sortvalue == directorynames[i].filename.sortvalue ) {                                              // FOUND PATH, MOVE TO NEW DIRECTORY
            base_directory = directorynames[i].starting_cluster;
            entries = read_directory( base_directory, extension );
            path_position++; presentpathname = (union fname *)&path[ path_position * 8];
            i = 0;
        } else {
            i++;
        }
    } while( ( i <= entries ) || ( !path[ path_position * 8] ) );                                                               // FINISH WHEN NO MORE ENTRIES, OR SCANNED WHOLE PATH

    return( base_directory );
}

//unsigned char *sdcard_selectfile( char *message, char *extension, unsigned int *filesize, char *afterloading ) {
unsigned char *sdcard_selectfile( char *path, char *message, char *extension, unsigned int *filesize, char *afterloading ) {
    unsigned int starting_cluster;

    *filesize = 0;

    if( VolumeID == NULL ) {
        // MEMORY SPACE NOT ALLOCATED FOR FAT32 STRUCTURES
        BOOTRECORD = malloc( 512 );
        PARTITIONS = (PartitionTable *)&BOOTRECORD[446];
        VolumeID = malloc( 512 );
        FAT32table = malloc( 512 );
        directorynames = (DirectoryEntry *)malloc( sizeof( DirectoryEntry ) * 256 );
        sdcard_readsector( 0, BOOTRECORD );
        sdcard_readsector( PARTITIONS[0].start_sector, (unsigned char *)VolumeID );

        FAT32startsector = PARTITIONS[0].start_sector + VolumeID -> reserved_sectors;
        FAT32clusters = PARTITIONS[0].start_sector + VolumeID -> reserved_sectors + ( VolumeID -> number_of_fats * VolumeID -> fat32_size_sectors );
        FAT32clustersize = VolumeID -> sectors_per_cluster;

        directorycluster = malloc( FAT32clustersize * 512 );
    }

    if( !path[0] ) {
        // NO DIRECTORY SPECIFIED, START AT ROOT DIRECTORY
        starting_cluster = filebrowser( message, extension, VolumeID -> startof_root, VolumeID -> startof_root, filesize );
    } else {
        // STARTING DIRECTORY SPECIFIED, TRY TO ENTER
        starting_cluster = move_todirectory( path, extension, VolumeID -> startof_root );
        starting_cluster = filebrowser( message, extension, starting_cluster, starting_cluster, filesize );
    }

    if( starting_cluster ) {
        // ALLOCATE ENOUGH MEMORY TO READ CLUSTERS
        unsigned char *copyaddress = malloc( ( ( *filesize / ( FAT32clustersize * 512 )  ) + 1 ) * ( FAT32clustersize * 512 ) );
        if( copyaddress ) {
            gpu_outputstringcentre( WHITE, 224, 0, "Loading File", 0 );
            readfile( starting_cluster, copyaddress );
            gpu_outputstringcentre( WHITE, 224, 0, afterloading, 0 );
            return( copyaddress );
        } else {
            gpu_outputstringcentre( WHITE, 224, 1, "Insufficient Memory", 0 );
            return(0);
        }
    } else {
        return(0);
    }
}
