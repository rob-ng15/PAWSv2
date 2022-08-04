#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>
#include <curses.h>
#include <PAWSlibrary.h>

int main( void ) {
    // set up curses library
    initscr(); start_color(); autorefresh( TRUE );

    move( 0, 0 );
    printw( "PAWS Memory Test\n\n" );

    char *membyte, byte; short *memhalf, half; int *memword, word, error;

    printw("BRAM Test 0x2000 - 0xcfff\n\n");

    printw( "Start (byte)\n" );
    membyte = (char *)0x2000; byte = 0; error = 0;
    while( membyte < (char *)0x4000 ) { *membyte++ = byte++; }
    membyte = (char *)0x2000; byte = 0;
    while( membyte < (char *)0x4000 ) {
        if( *membyte != byte ) {
            if( !error ) printw( "  ERROR (byte) at 0x%04x, %02x not %02x\n", membyte, *membyte, byte );
            error++;
        }
        membyte++; byte++;
    }
    printw( "  Complete (byte) with %d errors\n\n",error);

    printw( "Start (half)\n" );
    memhalf = (short *)0x2000; half = 0; error = 0;
    while( memhalf < (short *)0x4000 ) { *memhalf++ = half++; }
    memhalf = (short *)0x2000; half = 0;
    while( memhalf < (short *)0x4000 ) {
        if( *memhalf != half ) {
            if( !error ) printw( "  ERROR (half) at 0x%04x, %02x not %02x\n", memhalf, *memhalf, half );
            error++;
        }
        memhalf++; half++;
    }
    printw( "  Complete (half) with %d errors\n\n",error);

    printw( "Start (word)\n" );
    memword = (int *)0x2000; word = 0; error = 0;
    while( memword < (int *)0x4000 ) { *memword++ = word--; }
    memword = (int *)0x2000; word = 0;
    while( memword < (int *)0x4000 ) {
        if( *memword != word ) {
            if( !error ) printw( "  ERROR (word) at 0x%04x, %02x not %02x\n", memword, *memword, word );
            error++;
        }
        memword++; word--;
    }
    printw( "  Complete (word) with %d errors\n\n",error);

    printw("\n\nSDRAM Test 0x6000000 - 0x7ffffff\n\n");

    printw( "Start (byte)\n" );
    membyte = (char *)0x6000000; byte = 0; error = 0;
    while( membyte < (char *)0x6000100 ) { *membyte++ = byte++; }
    memword = (int *)0x6000000;
    while( memword < (int *)0x6000100 ) {
        if( ( (int)memword & 0xc ) == 0 ) printw( "  CACHE: 0x%7x ",(int)memword );
        printw("%08x ",*memword);
        if( ( (int)memword & 0xc ) == 0xc ) printw( "\n" );
        memword++;
    }
    membyte = (char *)0x6000000; byte = 0;
    while( membyte < (char *)0x6000100 ) {
        if( *membyte != byte ) {
            if( !error ) printw( "  ERROR (byte) at 0x%07x, %02x not %02x via CACHE\n", membyte, *membyte, byte );
            error++;
        }
        membyte++; byte++;
    }
    printw( "  Complete (byte) with %d errors via CACHE\n",error);
    membyte = (char *)0x6000000; byte = 0;  error = 0;
    while( membyte < (char *)0x8000000 ) { *membyte++ = byte++; }
    memword = (int *)0x6000000;
    while( memword < (int *)0x6000100 ) {
        if( ( (int)memword & 0xc ) == 0 ) printw( "  SDRAM: 0x%7x ",(int)memword );
        printw("%08x ",*memword);
        if( ( (int)memword & 0xc ) == 0xc ) printw( "\n" );
        memword++;
    }
    membyte = (char *)0x6000000; byte = 0;
    while( membyte < (char *)0x8000000 ) {
        if( *membyte != byte ) {
            if( !error ) printw( "  ERROR (byte) at 0x%07x, %02x not %02x\n", membyte, *membyte, byte );
            error++;
        }
        membyte++; byte++;
    }
    printw( "  Complete (byte) with %d errors\n\n",error);

    printw( "Start (half)\n" );
    memhalf = (short *)0x6000000; half = 0; error = 0;
    while( memhalf < (short *)0x6000100 ) { *memhalf++ = half++; }
    memword = (int *)0x6000000;
    while( memword < (int *)0x6000100 ) {
        if( ( (int)memword & 0xc ) == 0 ) printw( "  CACHE: 0x%7x ",(int)memword );
        printw("%08x ",*memword);
        if( ( (int)memword & 0xc ) == 0xc ) printw( "\n" );
        memword++;
    }
    memhalf = (short *)0x6000000; half = 0;
    while( memhalf < (short *)0x6000100 ) {
        if( *memhalf != half ) {
            if( !error ) printw( "  ERROR (half) at 0x%07x, %04x not %04x via CACHE\n", memhalf, *memhalf, half );
            error++;
        }
        memhalf++; half++;
    }
    printw( "  Complete (half) with %d errors via CACHE\n",error);
    memhalf = (short *)0x6000000; half = 0; error = 0;
    while( memhalf < (short *)0x8000000 ) { *memhalf++ = half++; }
    memword = (int *)0x6000000;
    while( memword < (int *)0x6000100 ) {
        if( ( (int)memword & 0xc ) == 0 ) printw( "  SDRAM: 0x%7x ",(int)memword );
        printw("%08x ",*memword);
        if( ( (int)memword & 0xc ) == 0xc ) printw( "\n" );
        memword++;
    }
    memhalf = (short *)0x6000000; half = 0;
    while( memhalf < (short *)0x8000000 ) {
        if( *memhalf != half ) {
            if( !error ) printw( "  ERROR (half) at 0x%07x, %04x not %04x\n", memhalf, *memhalf, half );
            error++;
        }
        memhalf++; half++;
    }
    printw( "  Complete (half) with %d errors\n\n",error);

    printw( "Start (word)\n" );
    memword = (int *)0x6000000; word = 0; error = 0;
    while( memword < (int *)0x6000100 ) { *memword++ = word--; }
    membyte = (char *)0x6000000;
    while( membyte < (char *)0x6000100 ) {
        if( ( (int)membyte & 0xf ) == 0 ) printw( "  CACHE: 0x%7x ",(int)membyte );
        printw("%02x ",*membyte);
        if( ( (int)membyte & 0xf ) == 0xf ) printw( "\n" );
        membyte++;
    }
    memword = (int *)0x6000000;
    while( memword < (int *)0x6000100 ) {
        if( ( (int)memword & 0xc ) == 0 ) printw( "  CACHE: 0x%7x ",(int)memword );
        printw("%08x ",*memword);
        if( ( (int)memword & 0xc ) == 0xc ) printw( "\n" );
        memword++;
    }
    memword = (int *)0x6000000; word = 0;
    while( memword < (int *)0x6000100 ) {
        if( *memword != word ) {
            if( !error ) printw( "  ERROR (word) at 0x%07x, %08x not %08x via CACHE\n", memword, *memword, word );
            error++;
        }
        memword++; word--;
    }
    printw( "  Complete (word) with %d errors via CACHE\n",error);
    memword = (int *)0x6000000; word = 0; error = 0;
    while( memword < (int *)0x8000000 ) { *memword++ = word--; }
    membyte = (char *)0x6000000;
    while( membyte < (char *)0x6000100 ) {
        if( ( (int)membyte & 0xf ) == 0 ) printw( "  SDRAM: 0x%7x ",(int)membyte );
        printw("%02x ",*membyte);
        if( ( (int)membyte & 0xf ) == 0xf ) printw( "\n" );
        membyte++;
    }
    memword = (int *)0x6000000;
    while( memword < (int *)0x6000100 ) {
        if( ( (int)memword & 0xc ) == 0 ) printw( "  SDRAM: 0x%7x ",(int)memword );
        printw("%08x ",*memword);
        if( ( (int)memword & 0xc ) == 0xc ) printw( "\n" );
        memword++;
    }
    memword = (int *)0x6000000; word = 0;
    while( memword < (int *)0x8000000 ) {
        if( *memword != word ) {
            if( !error ) printw( "  ERROR (word) at 0x%07x, %08x not %08x\n", memword, *memword, word );
            error++;
        }
        memword++; word--;
    }
    printw( "  Complete (word) with %d errors\n\n",error);

    sleep1khz( 4000, 0 );
}
