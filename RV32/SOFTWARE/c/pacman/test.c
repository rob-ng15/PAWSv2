#include <PAWSlibrary.h>

char pacmaze[60][28] = {
    "ABBBBBBBBBBBBCCBBBBBBBBBBBBA",
    "              r            r",
    "B            DD            B",
    "3            31            1",
    "B EDDE EDDDE DD EDDDE EDDE B",
    "3    r     r 31     r    r 1",
    "B D  D D   D DD D   D D  D B",
    "3 3  1 3   1 31 3   1 3  1 1",
    "B EDDE EDDDE EE EDDDE EDDE B",
    "3 ssst sssst st sssst ssst 1",
    "B                          B",
    "3                          1",
    "B EDDE EE EDDDDDDE EE EDDE B",
    "3    r  r        r  r    r 1",
    "B EDDE DD EDDFFDDE DD EDDE B",
    "3 ssst 31 sssr sst 31 ssst 1",
    "B      DD    DD    DD      B",
    "3      31    31    31      1",
    "ABBBBE DEDDE DD EDDED EBBBBA",
    "sssssr 3sssr 31    t1  sssst",
    "     B DEDDE EE EDDED B     ",
    "     3 3 sst st sssr1 1     ",
    "     B DD          DD B     ",
    "     3 31          31 1     ",
    "     B DD GBHIJHBG DD B     ",
    "     3 31  sr   sr 31 1     ",
    "BBBBBE EE B      B EE EBBBBB",
    "     t st 1      3 st s     ",
    "          B      B          ", // TUNNEL
    "          1      3          ",
    "BBBBBE EE B      B EE EBBBBB",
    "sssssr  r 1      3  r  sssss",
    "     B DD GBBBBBBG DD B     ",
    "     3 31 s      t 31 1     ",
    "     B DD          DD B     ",
    "     3 31          31 1     ",
    "     B DD EDDDDDDE DD B     ",
    "     3 31        r 31 1     ",
    "ABBBBE EE EDDFFDDE EE EBBBBA",
    "     t st sssr sst st s    r",
    "B EDDE EDDDE DD EDDDE EDDE B",
    "3    r     r 31     r    r 1",
    "B EDED EDDDE EE EDDDE DEDE B",
    "3 ssr1 sssst st sssst 3 st 1",
    "B   DD                DD   B",
    "3   31                31   1",
    "KDE DD EE EDDDDDDE EE DD EDC",
    "3 r 31  r        r  r 31   1",
    "CDE EE DD EDDFFDDE DD EE EDK",
    "3st st 31 sssr sst 31 st ss1",
    "B      DD    DD    DD      B",
    "3      31    31    31      1",
    "B EDDDDEEDDE DD EDDEEDDDDE B",
    "3      ts  r 31    ts    r 1",
    "B EDDDDDDDDE EE EDDDDDDDDE B",
    "3 ssssssssst st ssssssssst 1",
    "B                          B",
    "3                          1",
    "ABBBBBBBBBBBBBBBBBBBBBBBBBBA",
    "ssssssssssssssssssssssssssst"

};

unsigned char tilemap_lower[] = {
    #include "graphics/tilemap_lower.h"
};

void drawmaze() {
    set_tilemap_bitamps_from_spritesheet( LOWER_LAYER, &tilemap_lower[0] );

    unsigned char action, tile;
    for( int y = 0; y < 30; y++ ) {
        for( int x = 0; x < 28; x++ ) {
            switch( pacmaze[ y * 2 + 1 ][ x ] ) {
                case 'r':   action = REFLECT_X;
                            break;
                case 's':   action = REFLECT_Y;
                            break;
                case 't':   action = REFLECT_X | REFLECT_Y;
                            break;
                case '1':   action = ROTATE90;
                            break;
                case '2':   action = ROTATE180;
                            break;
                case '3':   action = ROTATE270;
                            break;
                default:    action = 0;
                            break;
            }
            tile = ( pacmaze[ y * 2 ][ x ] == ' ' ) ? 0 : pacmaze[ y * 2 ][ x ] - '@';
            set_tilemap_tile( LOWER_LAYER, x + 7, y + 1, tile, action );
        }
    }
}

int main( void ) {
    // CODE GOES HERE
    drawmaze();
    sleep1khz( 8000, 0 );
}

// EXIT WILL RETURN TO BIOS
