#include <string.h>
#include <PAWSlibrary.h>

int main( void ) {
    // CODE GOES HERE
    unsigned char volatile *memory, *memory2;

    bitmap_display( 0 );
    for( memory = (unsigned char *)0x2000000; memory < (unsigned char *)0x2012C00; memory++ ) {
        *memory = rng( 256 );
    }
    sleep1khz( 4000, 0 );

    bitmap_display( 1 );
    for( memory = (unsigned char *)0x2020000; memory < (unsigned char *)0x2032C00; memory++ ) {
        *memory = rng( 256 );
    }
    sleep1khz( 4000, 0 );

    bitmap_display( 0 );
    for( memory = (unsigned char *)0x2000000; memory < (unsigned char *)0x2012C00; memory++ ) {
        *memory = (int)memory & 255;
    }
    sleep1khz( 4000, 0 );

    bitmap_display( 1 ); memory2 = (unsigned char *)0x2020000;
    for( memory = (unsigned char *)0x2000000; memory < (unsigned char *)0x2012C00; memory++ ) {
        *memory2++ = *memory;
    }
    sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
