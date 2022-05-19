#include <stdio.h>
#include <PAWSlibrary.h>

extern void sdcard_readsector( unsigned int sectorAddress, unsigned char *copyAddress );
extern void sdcard_writesector( unsigned int sectorAddress, unsigned char *copyAddress );

unsigned char buffer[512];

int main( void ) {
    printf("Reading Sector 1\n");
    sdcard_readsector( 1, buffer );

    printf("Dumping Sector 1\n");
    for( int i = 0; i < 512; i++ ) {
        printf("%02x ",buffer[i]);
    }
    printf("\n\n");
    sleep1khz( 8000, 0 );

    printf("Filled Buffer\n");
    for( int i = 0; i < 512; i++ ) {
        buffer[i] = i & 255;
        printf("%02x ",buffer[i]);
    }
    printf("\n\n");

    printf("Writing Sector 1\n");
    sdcard_writesector( 1, buffer );

    printf("Reading Sector 1\n");
    sdcard_readsector( 1, buffer );

    printf("Dumping Sector 1\n");
    for( int i = 0; i < 512; i++ ) {
        printf("%02x ",buffer[i]);
    }
    printf("\n\n");
    sleep1khz( 8000, 0 );
}

// EXIT WILL RETURN TO BIOS
