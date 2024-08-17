#include "PAWS.h"
#include <stdint.h>
#include "PAWS_BIOS_LIBRARY.h"

extern int _bss_start, _bss_end;

void draw_paws_logo( void ) {
    set_blitter_bitmap( 3, &PAWSLOGO[0] );
    gpu_blit( UK_GOLD, 2, 2, 3, 2 );
}

void reset_system( void ) {
    *AUDIO_DMA_L_STATUS = 1; *AUDIO_DMA_R_STATUS = 1;

    *GPU_DITHERMODE = 0;
    *FRAMEBUFFER_DRAW = 3; gpu_cs(); while( !*GPU_FINISHED );
    *FRAMEBUFFER_DRAW = 1; *FRAMEBUFFER_DISPLAY = 1;
    *SCREENMODE = 0; *COLOUR = 0;
    tpu_cs();
    tm_cs( 0 ); tm_cs( 1 );
    for( unsigned short i = 0; i < 32; i++ ) {
        LOWER_SPRITE_ACTIVE[i] = 0;
        UPPER_SPRITE_ACTIVE[i] = 0;
    }
}

void main( void ) {
    // STOP SMT, RESET THE SYSTEM, ZERO THE VARIABLE SPACE
    *SMTSTATUS = 0;
    memset( &_bss_start, 0, &_bss_end - &_bss_start );
    reset_system();

    // SET THE DISPLAY
    set_background( UK_BLUE, UK_GOLD, 1 );
    draw_paws_logo();

    unsigned long *memoryaddress = ( void *)*RAMBASE;
    unsigned int memorycells = ( *STACKTOP - *RAMBASE ) / 8;

    tpu_set( 16, 0, TRANSPARENT, WHITE, TPU_BOLD | TPU_X2 | TPU_Y2 ); tpu_outputstring( "PPAAWWSSvv22  MMEEMMOORRYYTTEESSTT" );
    tpu_set( 16, 1, TRANSPARENT, WHITE, TPU_BOLD | TPU_X2 | TPU_Y2 ); tpu_outputstring( "PPAAWWSSvv22  MMEEMMOORRYYTTEESSTT" );

    tpu_set( 0, 55, TRANSPARENT, BLACK, TPU_BOLD );
    tpu_outputstring( "SDRAM FROM: " ); tpu_outputhex( *RAMBASE ); tpu_outputstring( " TO: "); tpu_outputhex( *STACKTOP );
    tpu_outputstring( " IN: " ); tpu_outputhex( memorycells ); tpu_outputstring(" cells");

    tpu_set( 0, 57, TRANSPARENT, BLACK, TPU_BOLD | TPU_BLINK );
    tpu_outputstring( "RESET SDRAM        " );

    // SET THE SDRAM TO THE ADDRESS
    for( int i = 0; i < memorycells; i++ ) {
        memoryaddress[ i ] = ( unsigned long )&memoryaddress[ i ];
    }

    tpu_set( 0, 57, TRANSPARENT, BLACK, TPU_BOLD | TPU_BLINK );
    tpu_outputstring( "READING SDRAM      " );

    unsigned long errors = 0, old_memory_writevalue = 0, new_memory_writevalue;
    for( int i = 0; i < memorycells; i++ ) {
        if( !( i & 0xff ) ) {
            tpu_set( 0, 58, TRANSPARENT, BLUE, TPU_BOLD );
            tpu_outputstring( "READING: " ); tpu_outputhex( ( unsigned long ) &memoryaddress[i] );
        }
        if( memoryaddress[ i ] != ( unsigned long )&memoryaddress[ i ] ) {
            errors++;
            tpu_set( 0, 59, RED, WHITE, TPU_BOLD | TPU_BLINK );
            tpu_outputstring( "ERRORS: " ); tpu_outputhex( errors ); tpu_outputstring( " ADDRESS: " ); tpu_outputhex( (unsigned long)&memoryaddress[i] );
            tpu_outputstring( " GOT: " ); tpu_outputhex( memoryaddress[ i ] );
            while(1) {}
        }
    }

    tpu_set( 0, 57, TRANSPARENT, BLACK, TPU_BOLD | TPU_BLINK );
    tpu_outputstring( "RESET SDRAM        " );

    // BLANK THE SDRAM
    for( int i = 0; i < memorycells; i++ ) {
        if( !( i & 0xff ) ) {
            tpu_set( 0, 58, TRANSPARENT, BLUE, TPU_BOLD );
            tpu_outputstring( "WRITING: " ); tpu_outputhex( 0 ); tpu_outputstring( " TO: " ); tpu_outputhex( ( unsigned long ) &memoryaddress[i] );
        }
        memoryaddress[ i ] = 0;
    }

    tpu_set( 0, 57, TRANSPARENT, BLACK, TPU_BOLD | TPU_BLINK );
    tpu_outputstring( "SLOW TESTING SDRAM " );

    errors = 0; old_memory_writevalue = 0; new_memory_writevalue =0;
    for( unsigned long w = 0; w < 256; w++ ) {
        new_memory_writevalue = 0;
        for( int i = 0; i < 8; i++ )
            new_memory_writevalue = _rv64_rol( new_memory_writevalue, 8 ) | w;

        for( int j = 0; j < 1024; j++ ) {
            for( int i = j; i < memorycells; i += 1024 ) {
                if( !( i & 0xff ) ) {
                    tpu_set( 0, 58, TRANSPARENT, BLUE, TPU_BOLD );
                    tpu_outputstring( "WRITING: " ); tpu_outputhex( new_memory_writevalue ); tpu_outputstring( " TO: " ); tpu_outputhex( ( unsigned long ) &memoryaddress[i] );
                }
                if( memoryaddress[ i ] != old_memory_writevalue ) {
                    errors++;
                    tpu_set( 0, 59, RED, WHITE, TPU_BOLD | TPU_BLINK );
                    tpu_outputstring( "ERRORS: " ); tpu_outputhex( errors ); tpu_outputstring( " ADDRESS: " ); tpu_outputhex( (unsigned long)&memoryaddress[i] );
                    tpu_outputstring( " GOT: " ); tpu_outputhex( memoryaddress[ i ] );
                    while(1) {}
                }
                memoryaddress[ i ] = new_memory_writevalue;
            }
        }

        old_memory_writevalue = new_memory_writevalue;
    }

    tpu_set( 0, 57, TRANSPARENT, BLACK, TPU_BOLD | TPU_BLINK );
    tpu_outputstring( "FINISHED TESTING SDRAM" );

    while(1) {}
}
