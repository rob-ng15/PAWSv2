#include <PAWSlibrary.h>

int main( void ) {
    gpu_cs(); tpu_cs();

    for(int i = 0; i < 32; i++ ) {
        gpu_dither( rng(16), rng( 256 ) );
        gpu_ellipse( rng(256), rng(320), rng(240), rng(64), rng(64), rng(1) );
        sleep1khz( 500, 0 );
    }

    sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
