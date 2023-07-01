#include <PAWSlibrary.h>

int main( void ) {
    for( int i = 0; i < 32; i++ ) {
        gpu_cs(); tpu_cs();

        short x0 = rng(384)-32, x1 = rng(384)-32, x2 = rng(384)-32;
        short y0 = rng(304)-32, y1 = rng(304)-32, y2 = rng(304)-32;

        gpu_triangle( BLUE, x0, y0, x1, y1, x2, y2 );
        gpu_line( WHITE, x0, y0, x1, y1 ); gpu_line( WHITE, x1, y1, x2, y2 ); gpu_line( WHITE, x2, y2, x0, y0 );

        tpu_set( 0, 0, TRANSPARENT, GREEN );
        tpu_printf( 0, " ( %d, %d ) -> ( %d, %d ) -> ( %d, %d )", x0, y0, x1, y1, x2, y2 );
        sleep1khz( 1000, 0 );
    }
}

// EXIT WILL RETURN TO BIOS
