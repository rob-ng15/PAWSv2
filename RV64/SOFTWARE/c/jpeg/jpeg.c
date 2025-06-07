#include <stdlib.h>
#include <nanojpeg.h>
#include <PAWSlibrary.h>

int main( int argc, char **argv ) {
    int width, height; unsigned int filesize;
    unsigned char *imagebuffer, colour, *filebuffer;

    filebuffer = sdcard_selectfile( "Please select a JPEG", "JPG", &filesize, "Decoding" );

    // JPEG LIBRARY
    if( filebuffer ) {
        njInit();
        njDecode( filebuffer, filesize );
        width = njGetWidth();
        height = njGetHeight();
        imagebuffer=njGetImage();

        // DISPLAY IN COLOUR - SWITCH OFF THE PIXEL BLOCK REMAPPER
        screen_mode( MODE_RGBM ); bitmap_256( true );
        gpu_pixelblock_mode( FALSE );
        gpu_pixelblock24( 0, 0, width, height, imagebuffer );
        sleep1khz( 4000 );

        // DISPLAY IN GREY - SWITCH ON THE PIXEL BLOCK REMAPPER
        screen_mode( MODE_GREY ); bitmap_256( true );
        gpu_pixelblock_mode( TRUE );
        gpu_pixelblock24( 0, 0, width, height, imagebuffer );

        free( filebuffer );
    } else {
        gpu_print_centre( WHITE, 160, 120, BOLD, 0, 0, "NO FILE FOUND!" );
    }

    sleep1khz( 4000 );
}
