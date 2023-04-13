#include <PAWSlibrary.h>

void plot4( unsigned char colour, short xc, short yc, short xd, short yd ) {
    gpu_pixel( colour, xc + xd, yc + yd );
    gpu_pixel( colour, xc - xd, yc + yd );
    gpu_pixel( colour, xc - xd, yc - yd );
    gpu_pixel( colour, xc + xd, yc -yd );
}

void fill4( unsigned char colour, short xc, short yc, short xd, short yd ) {
    gpu_line( colour, xc + xd, yc + yd, xc + xd, yc -yd );
    gpu_line( colour, xc - xd, yc + yd, xc - xd, yc -yd );
}

void ellipse( unsigned char colour, short xc, short yc, short radius_x, short radius_y, int filled ) {
    short active_x, active_y;
    int Xchange, Ychange, ellipseERROR, Asquare2, Bsquare2, Xstop, Ystop;

    Asquare2 = 2* radius_x * radius_x;
    Bsquare2 = 2 * radius_y * radius_y;

    active_x = radius_x;
    active_y = 0;
    Xchange = radius_y * radius_y * ( 1 - 2 * radius_x );
    Ychange = radius_x * radius_x;
    ellipseERROR= 0;
    Xstop = Bsquare2 * radius_x;
    Ystop = 0;


    while( Xstop >= Ystop ) {
        if( filled ) fill4( colour, xc, yc, active_x, active_y ); else  plot4( colour, xc, yc, active_x, active_y );
        active_y++;
        Ystop += Asquare2;
        ellipseERROR += Ychange;
        Ychange += Asquare2;
        if( ( 2 * ellipseERROR + Xchange ) > 0 ) {
            active_x--;
            Xstop -= Bsquare2;
            ellipseERROR += Xchange;
            Xchange += Bsquare2;
        }
    }

    active_x = 0;
    active_y = radius_y;
    Xchange = radius_y * radius_y;
    Ychange = radius_x * radius_x * ( 1 - 2 * radius_y );
    ellipseERROR= 0;
    Xstop = 0;
    Ystop = Asquare2 * radius_y;

    while( Xstop <= Ystop ) {
        if( filled ) fill4( colour, xc, yc, active_x, active_y ); else  plot4( colour, xc, yc, active_x, active_y );
        active_x++;
        Xstop += Bsquare2;
        ellipseERROR += Xchange;
        Xchange += Bsquare2;
        if( ( 2 * ellipseERROR + Ychange ) > 0 ) {
            active_y--;
            Ystop -= Asquare2;
            ellipseERROR += Ychange;
            Ychange += Asquare2;
        }
    }
}

int main( void ) {
    gpu_cs(); tpu_cs();

    for(int i = 0; i < 32; i++ ) {
        ellipse( rng(256), rng(320), rng(240), rng(64), rng(64), rng(1) );
        sleep1khz( 500, 0 );
    }

    sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
