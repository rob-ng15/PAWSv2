#include "PAWS.h"

typedef unsigned int size_t;

// BACKGROUND PATTERN GENERATOR
#define BKG_SOLID 0

// PAWS LOGO BLITTER TILE
unsigned short PAWSLOGO[] = {
    0b0000000001000000,
    0b0000100011100000,
    0b0001110011100000,
    0b0001110011100000,
    0b0001111011100100,
    0b0000111001001110,
    0b0010010000001110,
    0b0111000000001110,
    0b0111000111001100,
    0b0111001111110000,
    0b0011011111111000,
    0b0000011111111000,
    0b0000011111111100,
    0b0000111111111100,
    0b0000111100001000,
    0b0000010000000000
};

// SDCARD BLITTER TILES
unsigned short sdcardtiles[] = {
    // CARD
    0x0000, 0x0000, 0x0ec0, 0x08a0, 0xea0, 0x02a0, 0x0ec0, 0x0000,
    0x0a60, 0x0a80, 0x0e80, 0xa80, 0x0a60, 0x0000, 0x0000, 0x0000,
    // SDHC
    0x3ff0, 0x3ff8, 0x3ffc, 0x3ffc, 0x3ffc, 0x3ff8, 0x1ffc, 0x1ffc,
    0x3ffc, 0x3ffc, 0x3ffc, 0x3ffc, 0x3ffc, 0x3ffc, 0x3ffc, 0x3ffc,
    // LED INDICATOR
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 0x0018, 0x0000
};

// PACMAN GHOST GRAPHICS 2 EACH FOR RIGHT
unsigned char ghost_bitmap[] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0x40, 0x40,
    0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0x40,
    0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xc1, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xc1, 0x40, 0x40,
    0x40, 0x40, 0xc1, 0xc1, 0xff, 0xff, 0x00, 0x00, 0xc1, 0xc1, 0xff, 0xff, 0x00, 0x00, 0x40, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0x00, 0x00, 0xc1, 0xc1, 0xff, 0xff, 0x00, 0x00, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xff, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xc1, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0x40, 0xc1, 0xc1, 0xc1, 0x40, 0x40, 0xc1, 0xc1, 0xc1, 0x40, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0x40, 0x40, 0x40, 0xc1, 0xc1, 0x40, 0x40, 0xc1, 0xc1, 0x40, 0x40, 0x40, 0xc1, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,

    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0x40, 0x40,
    0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0x40,
    0x40, 0x40, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xc1, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xc1, 0x40, 0x40,
    0x40, 0x40, 0xc1, 0xc1, 0xff, 0xff, 0x00, 0x00, 0xc1, 0xc1, 0xff, 0xff, 0x00, 0x00, 0x40, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0x00, 0x00, 0xc1, 0xc1, 0xff, 0xff, 0x00, 0x00, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xff, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xc1, 0xc1, 0xc1, 0xc1, 0xff, 0xff, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x40,
    0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0x40, 0xc1, 0xc1, 0xc1, 0xc1, 0x40,
    0x40, 0x40, 0xc1, 0xc1, 0x40, 0x40, 0x40, 0xc1, 0xc1, 0x40, 0x40, 0x40, 0xc1, 0xc1, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40
};


// PACMAN - 4 FOR UP/DOWN 4 FOR RIGHT/LEFT, USE REFLECTION OF UP FOR DOWN AND RIGHT FOR LEFT
unsigned char pacman_bitmap[] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0x40, 0x40, 0x40, 0x40, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,

    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0x40, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,

    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,

    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0x40, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40
};

// RISC-V CSR FUNCTIONS
unsigned int CSRisa() {
   unsigned int isa;
   asm volatile ("csrr %0, 0x301" : "=r"(isa));
   return isa;
}

// DMA CONTROLLER
void DMASTART( const void *restrict source, void *restrict destination, unsigned int count, unsigned char mode ) {
    *DMASOURCE = (unsigned int)source;
    *DMADEST = (unsigned int)destination;
    *DMACOUNT = count;
    *DMAMODE = mode;
}

// STANDARD C FUNCTIONS ( from @sylefeb mylibc )
void * memset(void *dest, int val, size_t len) {
    *DMASET = val;
    DMASTART( (const void *restrict)DMASET, dest, len, 4 );
    return dest;
}

short strlen( char *s ) {
    short i = 0;
    while( *s ) {
        s++;
        i++;
    }
    return(i);
}

// TIMER AND PSEUDO RANDOM NUMBER GENERATOR
// SLEEP FOR counter milliseconds
void sleep( unsigned short counter ) {
    *SLEEPTIMER0 = counter;
    while( *SLEEPTIMER0 );
}

// AUDIO OUTPUT
// START A note (1 == DEEP C, 25 == MIDDLE C )
// OF duration MILLISECONDS TO THE LEFT ( channel_number == 1 ) RIGHT ( channel_number == 2 ) or BOTH ( channel_number == 3 ) AUDIO CHANNEL
// IN waveform 0 == SQUARE, 1 == SAWTOOTH, 2 == TRIANGLE, 3 == SINE, 4 == WHITE NOISE, 7 == SAMPLE MODE
// 1 = C 2 or Deep C
// 25 = C 3
// 49 = C 4 or Middle C
// 73 = C 5 or Tenor C
// 97 = C 6 or Soprano C
// 121 = C 7 or Double High C
void beep( unsigned char channel_number, unsigned char waveform, unsigned char note, unsigned short duration ) {
    AUDIO_REGS[ 0x00 ] = _rv32_pack( waveform, note );
    AUDIO_REGS[ 0x01 ] = _rv32_pack( duration, channel_number );
}
void volume( unsigned char left, unsigned char right ) {
    *AUDIO_L_VOLUME = left; *AUDIO_R_VOLUME = right;
}
void await_beep( unsigned char channel_number ) {
    unsigned char volatile *AUDIO_REGS_B = (unsigned char volatile *)AUDIO_REGS;
    while( ( ( channel_number & 1) & AUDIO_REGS_B[ 0x10 ] ) | ( ( channel_number & 2) & AUDIO_REGS_B[ 0x12 ] ) ) {}
}

unsigned short get_beep_active( unsigned char channel_number ) {
    unsigned char volatile *AUDIO_REGS_B = (unsigned char volatile *)AUDIO_REGS;
    return( ( ( channel_number & 1) & AUDIO_REGS_B[ 0x10 ] ) | ( ( channel_number & 2) & AUDIO_REGS_B[ 0x12 ] ) );
}

// USES DOOM PC SPEAKER FORMAT SAMPLES - USE DMA MODE 1 multi-source to single-dest
void sample_upload( unsigned char channel_number, unsigned short length, unsigned char *samples ) {
    beep( channel_number, 0, 0, 0 );
    *AUDIO_NEW_SAMPLE = channel_number;
    if( channel_number & 1 ) { DMASTART( samples, (void *restrict)AUDIO_LEFT_SAMPLE, length, 1 ); }
    if( channel_number & 2 ) { DMASTART( samples, (void *restrict)AUDIO_RIGHT_SAMPLE, length, 1 ); }
}

// WAIT FOR VBLANK TO START
void await_vblank( void ) {
    while( !*VBLANK );
}

// BACKGROUND GENERATOR
void set_background( unsigned char colour, unsigned char altcolour, unsigned char backgroundmode ) {
    *BACKGROUND_COPPER_STARTSTOP = 0;
    *BACKGROUND_COLOUR = colour;
    *BACKGROUND_ALTCOLOUR = altcolour;
    *BACKGROUND_MODE = backgroundmode;
}

// GPU AND BITMAP
// The bitmap is 320 x 240 pixels (0,0) is top left
// The GPU can draw pixels, filled rectangles, lines, (filled) circles, filled triangles and has a 16 x 16 pixel blitter from user definable tiles

// INTERNAL FUNCTION - WAIT FOR THE GPU TO FINISH THE LAST COMMAND
inline void wait_gpu( void )  __attribute__((always_inline));
void wait_gpu( void ) {
    while( *GPU_STATUS );
}

// SET GPU TO RECEIVE A PIXEL BLOCK, SEND INDIVIDUAL PIXELS, STOP
void gpu_pixelblock_start( short x, short y, unsigned short w ) {
    wait_gpu();
    *GPU_X = x;
    *GPU_Y = y;
    *GPU_PARAM0 = w;
    *GPU_PARAM1 = TRANSPARENT;
    *GPU_WRITE = 10;
}

// DRAW A FILLED RECTANGLE from (x1,y1) to (x2,y2) in colour
void gpu_rectangle( unsigned char colour, short x1, short y1, short x2, short y2 ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = x2;
    *GPU_PARAM1 = y2;

    wait_gpu();
    *GPU_WRITE = 3;
}

// CLEAR THE BITMAP by drawing a transparent rectangle from (0,0) to (639,479) and resetting the bitamp scroll position
void gpu_cs( void ) {
    wait_gpu();
    gpu_rectangle( 64, 0, 0, 319, 239 );
}

// BLIT A 16 x 16 ( blit_size == 1 doubled to 32 x 32 ) TILE ( from tile 0 to 31 ) to (x1,y1) in colour
void gpu_blit( unsigned char colour, short x1, short y1, short tile, unsigned char blit_size ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = tile;
    *GPU_PARAM1 = blit_size;
    *GPU_PARAM2 = 0; // NO REFLECTION

    wait_gpu();
    *GPU_WRITE = 7;
}

// BLIT AN 8 x8  ( blit_size == 1 doubled to 16 x 16, blit_size == 1 doubled to 32 x 32 ) CHARACTER ( from tile 0 to 255 ) to (x1,y1) in colour
void gpu_character_blit( unsigned char colour, short x1, short y1, unsigned char tile, unsigned char blit_size ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = tile;
    *GPU_PARAM1 = blit_size;
    *GPU_PARAM2 = 0; // NO REFLECTION

    wait_gpu();
    *GPU_WRITE = 8;
}

// OUTPUT A STRING TO THE GPU
void gpu_outputstring( unsigned char colour, short x, short y, char *s, unsigned char size ) {
    while( *s ) {
        gpu_character_blit( colour, x, y, *s++, size );
        x = x + ( 8 << size );
    }
}
void gpu_outputstringcentre( unsigned char colour, short y, char *s, unsigned char size ) {
    gpu_rectangle( TRANSPARENT, 0, y, 319, y + ( 8 << size ) - 1 );
    gpu_outputstring( colour, 160 - ( ( ( 8 << size ) * strlen(s) ) >> 1) , y, s, 0 );
}
// SET THE BLITTER TILE to the 16 x 16 pixel bitmap
void set_blitter_bitmap( unsigned char tile, unsigned short *bitmap ) {
    *BLIT_WRITER_TILE = tile;

    for( short i = 0; i < 16; i ++ ) {
        *BLIT_WRITER_BITMAP = bitmap[i];
    }
}

// CHARACTER MAP FUNCTIONS
// The character map is an 80 x 30 character window with a 256 character 8 x 16 pixel character generator ROM )
// NO SCROLLING, CURSOR WRAPS TO THE TOP OF THE SCREEN

// CLEAR THE CHARACTER MAP
void tpu_cs( void ) {
    while( *TPU_COMMIT );
    *TPU_COMMIT = 3;
}

// SET THE TILEMAP TILE at (x,y) to tile
void set_tilemap_tile( unsigned char tm_layer, unsigned char x, unsigned char y, unsigned char tile, unsigned char action ) {
    switch( tm_layer ) {
        case 0:
            while( *LOWER_TM_STATUS );
            *LOWER_TM_X = x;
            *LOWER_TM_Y = y;
            *LOWER_TM_TILE = tile;
            *LOWER_TM_ACTION = action;
            *LOWER_TM_COMMIT = 1;
            break;
        case 1:
            while( *UPPER_TM_STATUS );
            *UPPER_TM_X = x;
            *UPPER_TM_Y = y;
            *UPPER_TM_TILE = tile;
            *UPPER_TM_ACTION = action;
            *UPPER_TM_COMMIT = 1;
            break;
    }
}
// SCROLL WRAP or CLEAR the TILEMAP by amount ( 0 - 15 ) pixels
//  action == 1 to 4 move the tilemap amount pixels LEFT, UP, RIGHT, DOWN
//  action == 5 clear the tilemap
//  RETURNS 0 if no action taken other than pixel shift, action if SCROLL WRAP or CLEAR was actioned
unsigned char tilemap_scrollwrapclear( unsigned char tm_layer, unsigned char action, unsigned char amount ) {
    while( *( tm_layer ? UPPER_TM_STATUS : LOWER_TM_STATUS ) );
    *( tm_layer ? UPPER_TM_SCROLLWRAPAMOUNT : LOWER_TM_SCROLLAMOUNT ) = amount;
    *( tm_layer ? UPPER_TM_SCROLLWRAPCLEAR : LOWER_TM_SCROLLWRAPCLEAR ) = action;
    return( tm_layer ? *UPPER_TM_SCROLLWRAPCLEAR : *LOWER_TM_SCROLLWRAPCLEAR );
}

// SET THE BITMAPS FOR sprite_number in sprite_layer to the 8 x 16 x 16 pixel bitmaps ( 2048 ARRGGBB pixels )
void set_sprite_bitmaps( unsigned char sprite_layer, unsigned char sprite_number, unsigned char *sprite_bitmaps ) {
    *( sprite_layer ? UPPER_SPRITE_WRITER_NUMBER : LOWER_SPRITE_WRITER_NUMBER ) = sprite_number;
    DMASTART( sprite_bitmaps, (void *restrict)(sprite_layer ? UPPER_SPRITE_WRITER_COLOUR : LOWER_SPRITE_WRITER_COLOUR), 2048, 1 );
}

// SET SPRITE sprite_number in sprite_layer to active status, in colour to (x,y) with bitmap number tile ( 0 - 7 ) in sprite_attributes bit 0 size == 0 16 x 16 == 1 32 x 32 pixel size, bit 1 x-mirror bit 2 y-mirror
void set_sprite( unsigned char sprite_layer, unsigned char sprite_number, unsigned char active, short x, short y, unsigned char tile, unsigned char sprite_attributes ) {
    switch( sprite_layer ) {
        case 0:
            LOWER_SPRITE_ACTIVE[sprite_number] = active;
            LOWER_SPRITE_TILE[sprite_number] = tile;
            LOWER_SPRITE_X[sprite_number] = x;
            LOWER_SPRITE_Y[sprite_number] = y;
            LOWER_SPRITE_ACTIONS[sprite_number] = sprite_attributes;
            break;

        case 1:
            UPPER_SPRITE_ACTIVE[sprite_number] = active;
            UPPER_SPRITE_TILE[sprite_number] = tile;
            UPPER_SPRITE_X[sprite_number] = x;
            UPPER_SPRITE_Y[sprite_number] = y;
            UPPER_SPRITE_ACTIONS[sprite_number] = sprite_attributes;
            break;
    }
}

// SET or GET ATTRIBUTES for sprite_number in sprite_layer
//  attribute == 0 active status ( 0 == inactive, 1 == active )
//  attribute == 1 tile number ( 0 to 7 )
//  attribute == 2 colour
//  attribute == 3 x coordinate
//  attribute == 4 y coordinate
//  attribute == 5 attributes bit 0 = size == 0 16x16 == 1 32x32. bit 1 = x-mirror bit 2 = y-mirror
void set_sprite_attribute( unsigned char sprite_layer, unsigned char sprite_number, unsigned char attribute, short value ) {
    if( sprite_layer == 0 ) {
        switch( attribute ) {
            case 0:
                LOWER_SPRITE_ACTIVE[sprite_number] = ( unsigned char) value;
                break;
            case 1:
                LOWER_SPRITE_TILE[sprite_number] = ( unsigned char) value;
                break;
            case 2:
                break;
            case 3:
                LOWER_SPRITE_X[sprite_number] = value;
                break;
            case 4:
                LOWER_SPRITE_Y[sprite_number] = value;
                break;
            case 5:
                LOWER_SPRITE_ACTIONS[sprite_number] = ( unsigned char) value;
                break;
        }
    } else {
        switch( attribute ) {
            case 0:
                UPPER_SPRITE_ACTIVE[sprite_number] = ( unsigned char) value;
                break;
            case 1:
                UPPER_SPRITE_TILE[sprite_number] = ( unsigned char) value;
                break;
            case 2:
                break;
            case 3:
                UPPER_SPRITE_X[sprite_number] = value;
                break;
            case 4:
                UPPER_SPRITE_Y[sprite_number] = value;
                break;
            case 5:
                UPPER_SPRITE_ACTIONS[sprite_number] = ( unsigned char) value;
                break;
        }
    }
}

// UPDATE A SPITE moving by x and y deltas, with optional wrap/kill and optional changing of the tile
//  update_flag = { y action, x action, tile action, 5 bit y delta, 5 bit x delta }
//  x and y action ( 0 == wrap, 1 == kill when moves offscreen )
//  x and y deltas a 2s complement -15 to 15 range
//  tile action, increase the tile number ( provides limited animation effects )
void update_sprite( unsigned char sprite_layer, unsigned char sprite_number, unsigned short update_flag ) {
    switch( sprite_layer ) {
        case 0:
            LOWER_SPRITE_UPDATE[sprite_number] = update_flag;
            break;
        case 1:
            UPPER_SPRITE_UPDATE[sprite_number] = update_flag;
            break;
    }
}

void draw_paws_logo( void ) {
    set_blitter_bitmap( 3, &PAWSLOGO[0] );
    gpu_blit( 60, 2, 2, 3, 2 );
}

void set_sdcard_bitmap( void ) {
    set_blitter_bitmap( 0, &sdcardtiles[0] );
    set_blitter_bitmap( 1, &sdcardtiles[16] );
    set_blitter_bitmap( 2, &sdcardtiles[32] );
}

void draw_sdcard( void  ) {
    set_sdcard_bitmap();
    gpu_blit( 60, 256, 2, 1, 2 );
    gpu_blit( 6, 256, 2, 0, 2 );
}

void reset_display( void ) {
    *GPU_DITHERMODE = 0;
    *FRAMEBUFFER_DRAW = 1; gpu_cs(); while( !*GPU_FINISHED );
    *FRAMEBUFFER_DRAW = 0; gpu_cs(); while( !*GPU_FINISHED );
    *FRAMEBUFFER_DISPLAY = 0;
    *SCREENMODE = 0; *COLOUR = 0;
    tpu_cs();
    *LOWER_TM_SCROLLWRAPCLEAR = 5;
    *UPPER_TM_SCROLLWRAPCLEAR = 5;
    for( unsigned short i = 0; i < 16; i++ ) {
        LOWER_SPRITE_ACTIVE[i] = 0;
        UPPER_SPRITE_ACTIVE[i] = 0;
    }
}

// SMT START STOP
void SMTSTOP( void ) {
    *SMTSTATUS = 0;
}
void SMTSTART( unsigned int code ) {
    *SMTPC = code;
    *SMTSTATUS = 1;
}

void smtmandel( void ) {
    const int graphwidth = 320, graphheight = 100;
    float kt = 63, m = 4.0;
    float xmin = -2.1, xmax = 0.6, ymin = -1.35, ymax = 1.35;
    float dx = (xmax - xmin) / graphwidth, dy = (ymax - ymin) / graphheight;
    float jx, jy, tx, ty, wx, wy, r;
    int k;

    gpu_pixelblock_start( 0, 122, 320 );
    for( int y = 0; y < graphheight; y++ ) {
        jy = ymin + y * dy;
        for( int x = 0; x < graphwidth; x++ ) {
            jx = xmin + x * dx;
            k = 0; wx = 0.0; wy = 0.0;
            do {
                tx = wx * wx - wy * wy + jx;
                ty = 2.0 * wx * wy + jy;
                wx = tx;
                wy = ty;
                r = wx * wx + wy * wy;
                k = k + 1;
            } while( ( r < m ) && ( k < kt ) );

            *PB_COLOUR = ( k > kt ) ? BLACK : k + 1;
        }
    }
    *PB_STOP = 3;
}

void smtthread( void ) {
    // SETUP STACKPOINTER FOR THE SMT THREAD
    asm volatile ("li sp ,0x4000");
    smtmandel();
    SMTSTOP();
}

unsigned char ufo_sample[] = { 75, 83, 89, 0 };

extern int _bss_start, _bss_end;
void main( void ) {
    unsigned int isa;
    unsigned short i,j = 0, x, y;

    // STOP SMT
    *SMTSTATUS = 0;

    // CLEAR MEMORY
    memset( &_bss_start, 0, &_bss_end - &_bss_end );

    // RESET THE DISPLAY
    reset_display(); set_background( UK_BLUE, UK_GOLD, 1 );

    // SETUP INITIAL WELCOME MESSAGE
    draw_paws_logo();
    draw_sdcard();
    gpu_outputstring( WHITE, 66, 2, "PAWSv2", 2 );
    gpu_outputstring( WHITE, 66, 34, "Risc-V RV32IMAFCB CPU", 0 );

    // COLOUR BARS ON THE TILEMAP - SCROLL WITH SMT THREAD - SET VIA DMA 5 SINGLE SOURCE TO SINGLE DESTINATION
    for( i = 0; i < 42; i++ ) {
        *LOWER_TM_WRITER_TILE_NUMBER = i + 1; *DMASET = i; DMASTART( (const void *restrict)DMASET, (void *restrict)LOWER_TM_WRITER_COLOUR, 256, 5 );
        *UPPER_TM_WRITER_TILE_NUMBER = i + 1; *DMASET = 63 - i; DMASTART( (const void *restrict)DMASET, (void *restrict)UPPER_TM_WRITER_COLOUR, 256, 5 );
        set_tilemap_tile( 0, i, 15, i+1, 0 );
        set_tilemap_tile( 1, i, 29, i+1, 0 );
    }

    gpu_outputstringcentre( 60, 72, "VERILATOR - SMT + FPU TEST", 0 );
    gpu_outputstringcentre( 60, 80, "THREAD 0 - PACMAN SPRITES", 0 );
    gpu_outputstringcentre( 60, 88, "THREAD 1 - FPU MANDELBROT", 0 );

    SMTSTART( (unsigned int )smtthread );

    set_sprite_bitmaps( 1, 0, &pacman_bitmap[0] );
    set_sprite_bitmaps( 1, 1, &ghost_bitmap[0] );

    set_sprite( 1, 0, 1, 0, 440, 4, 13 );
    set_sprite( 1, 1, 1, 64, 440, 0, 8);

    sample_upload( CHANNEL_LEFT, 4, &ufo_sample[0] );
    beep( CHANNEL_LEFT, SAMPLE_REPEAT | WAVE_SAMPLE | WAVE_SINE, 0, 250 );

    while(1) {
        await_vblank();
        if( (j&0xff)==0xff ) *UART_DATA = *SYSTEMSECONDS & 0xff;
        tilemap_scrollwrapclear( 0, 3, 1 );
        tilemap_scrollwrapclear( 1, 1, 1 );
        for( i = 0; i < 2; i++ ) update_sprite( 1, i, 1 );
        set_sprite_attribute( 1, 1, 1, ( j & 128 ) >> 7 );
        set_sprite_attribute( 1, 0, 1, ( ( j & 192 ) >> 6 ) );
        j++;
    }
}
