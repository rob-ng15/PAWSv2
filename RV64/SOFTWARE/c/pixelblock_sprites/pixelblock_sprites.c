#include <PAWSlibrary.h>

unsigned char tree_1[] = {
#include "graphics/tree-1.h"
};
unsigned char tree_2[] = {
#include "graphics/tree-2.h"
};
unsigned char tree_3[] = {
#include "graphics/tree-3.h"
};
unsigned char tree_4[] = {
#include "graphics/tree-4.h"
};
unsigned char moon_bitmap[] = {
    #include "graphics/moon.h"
};
bitmap_sprite tree_sprites[]= {
    {128,107,tree_1},
    {128,143,tree_2},
    {128,205,tree_3},
    {128,151,tree_4},
    {64,66,moon_bitmap}
};

int main( void ) {
    DrawBitmapSprite( 0, 0, 2, tree_sprites[ 0 ] ); sleep1khz( 4000, 0 );
    DrawBitmapSprite( 160, 0, 0.5, tree_sprites[ 1 ] ); sleep1khz( 4000, 0 );
    DrawBitmapSprite( 0, 120, 2, tree_sprites[ 2 ] ); sleep1khz( 4000, 0 );
    DrawBitmapSprite( 160, 120, 0.25, tree_sprites[ 3 ] ); sleep1khz( 4000, 0 );
    DrawBitmapSprite( 160, 60, 0.25, tree_sprites[ 4 ] ); sleep1khz( 4000, 0 );

    sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
// BITMAP GRAPHICS FOR SCALABLE TREES
