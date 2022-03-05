#include <stdio.h>
#include <stdlib.h>
#include "gifdec.h"
#include "gifdec.c"

// PAWSv2 Palette for colour matching
uint32_t PAWSpalette[128] = {
    0x000000,0x000055,0x0000aa,0x0000ff,0x005500,0x005555,0x0055aa,0x0055ff,
    0x00aa00,0x00aa55,0x00aaaa,0x00aaff,0x00ff00,0x00ff55,0x00ffaa,0x00ffff,
    0x550000,0x550055,0x5500aa,0x5500ff,0x555500,0x555555,0x5555aa,0x5555ff,
    0x55aa00,0x55aa55,0x55aaaa,0x55aaff,0x55ff00,0x55ff55,0x55ffaa,0x55ffff,
    0xaa0000,0xaa0055,0xaa00aa,0xaa00ff,0xaa5500,0xaa5555,0xaa55aa,0xaa55ff,
    0xaaaa00,0xaaaa55,0xaaaaaa,0xaaaaff,0xaaff00,0xaaff55,0xaaffaa,0xaaffff,
    0xff0000,0xff0055,0xff00aa,0xff00ff,0xff5500,0xff5555,0xff55aa,0xff55ff,
    0xffaa00,0xffaa55,0xffaaaa,0xffaaff,0xffff00,0xffff55,0xffffaa,0xffffff,
    0x000000,0x333333,0x4d4d4d,0x666666,0x808080,0x999999,0xb3b3b3,0xcccccc,
    0x1a0000,0x330000,0x4d0000,0x660000,0x800000,0x990000,0xb30000,0xcc0000,
    0x001a00,0x003300,0x004d00,0x006600,0x008000,0x009900,0x00b300,0x00cc00,
    0x00001a,0x000033,0x00004d,0x000066,0x000080,0x000099,0x0000b3,0x0000cc,
    0x1a1a00,0x333300,0x4d4d00,0x666600,0x808000,0x999900,0xb3b300,0xcccc00,
    0x1a001a,0x330033,0x4d004d,0x660066,0x800080,0x990099,0xb300b3,0xcc00cc,
    0x001a1a,0x003333,0x004d4d,0x006666,0x008080,0x009999,0x00b3b3,0x00cccc,
    0x994c00,0xffd700,0x228b22,0x4682b4,0x8a2be2,0xffc0cb,0x87ceeb,0xe5ffcc
};

// Match RGB colour to palette index
uint8_t matchcolour( uint8_t *colour ) {
    uint32_t colour32;

    colour32 = ( colour[0] << 16 ) + ( colour[1] << 8 ) + colour[2];
    for( uint8_t i = 0; i < 128; i++ ) {
        if( colour32 == PAWSpalette[i] ) {
            return(i);
        }
    }
    printf("FAIL: %0x6x ",colour32);
    return( 0x99 );
}

int main(int argc, char *argv[]) {
    gd_GIF *gif;
    char title[32] = {0};
    uint8_t *colour, *frame;

    if (argc < 2) {
        fprintf(stderr, "usage:\n  %s gif-file [t]\n", argv[0]);
        return 1;
    }
    gif = gd_open_gif(argv[1]);
    if (!gif) {
        fprintf(stderr, "Could not open %s\n", argv[1]);
        return 1;
    }
    frame = malloc(gif->width * gif->height * 3);
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        return 1;
    }

    while (gd_get_frame(gif)) {
        gd_render_frame(gif, frame);
        colour = frame;
        printf("        ");
        for( int y = 0; y < gif->height; y++ ) {
            for( int x = 0; x < gif->width; x++ ) {
                if(gd_is_bgcolor(gif,colour) && ( argc == 3 )) {
                    printf("64, ");
                } else {
                    printf("%d, ", matchcolour( colour ));
                }
                colour += 3;
                if( ( x & 0xf ) == 0xf ) printf("\n        ");
            }
        }
    }

    free(frame);
    gd_close_gif(gif);
    return 0;
}
