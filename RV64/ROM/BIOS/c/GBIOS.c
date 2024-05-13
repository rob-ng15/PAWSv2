#include "PAWS.h"
#include "GPUTEST.h"

// DMA CONTROLLER
void DMASTART( const void *restrict source, void *restrict destination, unsigned int count, unsigned short mode ) {
    *DMASOURCE = (unsigned long)source;
    *DMADEST = (unsigned long)destination;
    *DMACOUNT = count;
    *DMAMODE = mode;
}

// STANDARD C FUNCTIONS ( from @sylefeb mylibc )
void *memset(void *destination, int val, size_t len) {
    *DMASET = val;
    DMASTART( (const void *restrict)DMASET, destination, len, DMA_SET_TO_M );
    return destination;
}
void *memset32( void *restrict destination, int value, size_t count ) {
    *DMASET32 = value; DMASTART( (const void *restrict)DMASET, destination, count, DMA_SET_TO_M );
    return( destination );
}

void *memcpy( void *restrict destination, const void *restrict source, size_t count ) {
    DMASTART( source, destination, count, DMA_CPY_M_TO_M );
    return( destination );
}

// INTERNAL FUNCTION - WAIT FOR THE GPU TO FINISH THE LAST COMMAND
void wait_gpu( void ) {
    while( *GPU_STATUS );
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

// CLEAR THE CHARACTER MAP
void tpu_cs( void ) {
    memset32( ( void *)0x1000000, ( 64 << 16 ), 4800 * 4 );
}

void reset_display( void ) {
    *GPU_DITHERMODE = 0;
    *FRAMEBUFFER_DRAW = 3; gpu_cs(); while( !*GPU_FINISHED );
    *FRAMEBUFFER_DRAW = 1; *FRAMEBUFFER_DISPLAY = 1;
    *SCREENMODE = 0; *COLOUR = 0;
    tpu_cs();
    *LOWER_TM_SCROLLWRAPCLEAR = 5;
    *UPPER_TM_SCROLLWRAPCLEAR = 5;
    for( unsigned short i = 0; i < 16; i++ ) {
        LOWER_SPRITE_ACTIVE[i] = 0;
        UPPER_SPRITE_ACTIVE[i] = 0;
    }
}

extern int _bss_start, _bss_end;

void main( void ) {
    // STOP SMT
    *SMTSTATUS = 0;

    // CLEAR MEMORY
    memset( &_bss_start, 0, &_bss_end - &_bss_start );
    memcpy( ( void *)(long)*RAMBASE, &array[0], 7408 );

    // RESET THE DISPLAY
    reset_display();

    // CALL SDRAM LOADED PROGRAM
    ((void(*)(void))((long)*RAMBASE))();
    // RETURN TO BIOS IF PROGRAM EXITS
    ((void(*)(void))0x0)();
}
