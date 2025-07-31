#include "PAWS.h"
#include "RAYTRACE.h"
#include "PAWS_BIOS_LIBRARY.h"

void reset_display( void ) {
    *AUDIO_DMA_L_STATUS = 1; *AUDIO_DMA_R_STATUS = 1;
    for( int i = 0; i < 6; i++ ) AUDIO_DURATION[ i ] = 0;

    *GPU_DITHERMODE = 0;
    *CROP_LEFT = 0; *CROP_RIGHT = 319; *CROP_TOP = 0; *CROP_BOTTOM = 239;
    *FRAMEBUFFER_DRAW = 3; gpu_cs(); while( !*GPU_FINISHED );
    *FRAMEBUFFER_DRAW = 1; *BITMAP_DISPLAY256 = 0;
    *SCREENORDER = ( 3 << 0 ) | ( 1 << 4 ) | ( 4 << 8 ) | ( 8 << 12 ) | ( 9 << 16 ) | ( 10 << 20 ) | ( 11 << 24 );
    *COLOUR = 0;
    tpu_cs(); *TPU_LOREZ = 0;
    *TM_LOREZ = 0;
    for( int i = 0; i < 4; i++ ) {
        tm_cs( i );
        TM_SCROLLAMOUNT[i] = 1;
    }
    for( int i = 0; i < 63; i++ ) SPRITE_ACTIVE[i] = 0;
}

extern int _bss_start, _bss_end;

void main( void ) {
    // CLEAR MEMORY
    memset( &_bss_start, 0, &_bss_end - &_bss_start );
    memcpy( ( void *)(long)*RAMBASE, (void *)array, sizeof(array) );

    // RESET THE DISPLAY
    reset_display();

    // CALL SDRAM LOADED PROGRAM
    ((void(*)(void))((long)*RAMBASE))();
    // RETURN TO BIOS IF PROGRAM EXITS
    ((void(*)(void))0x0)();
}
