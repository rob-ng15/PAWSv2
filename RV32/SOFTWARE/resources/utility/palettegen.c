#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] ) {

    int red, green, blue, magenta, colour,count = 0;
    int BIT3to8[] = { 0, 36, 73, 109, 146, 182, 219, 255 };

    for( red = 0; red < 4; red++ ) {
        for( green = 0; green <8; green++ ) {
            for( blue = 0; blue < 4; blue++ ) {
                for( magenta = 0; magenta < 2; magenta++ ) {
                    colour = BIT3to8[ red * 2 + magenta ] << 16;
                    colour += ( BIT3to8[ green ] ) << 8;
                    colour += BIT3to8[ blue * 2 + magenta ];
                    printf("0x%06x,",colour);
                    count++; if( !(count&0xf) ) printf("\n");
                }
            }
        }
    }

    return 0;
}
