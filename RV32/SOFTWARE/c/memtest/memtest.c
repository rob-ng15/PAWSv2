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

    unsigned char *membyte, byte, count; unsigned short *memhalf, half; unsigned int *memword, word, error;
    void *start, *last, *lastprint;

    for( int i = 0; i < 3; i++ ) {
        switch( i ) {
            case 0:  start = (void *)0x2000; last = (void *)0x4000; lastprint = (void *)0x2100; break;
            case 1:  start = (void *)0x6000000; last = (void *)0x6000100; lastprint = (void *)0x6000100; break;
            case 2:  start = (void *)0x6000000; last = (void *)0x8000000; lastprint = (void *)0x6000100; break;
        }

        switch( i ) {
            case 0:  printw("\n\nBRAM Test 0x2000 - 0x4000\n\n");
                break;
            case 1:  printw("\n\nSDRAM CACHE Test 0x6000000 - 0x6000100\n\n");
                break;
            case 2:  printw("\n\nSDRAM Test 0x6000000 - 0x8000000\n\n");
                break;
        }

        printw( "Start (byte)\n" );
        membyte = (unsigned char *)start; byte = 0x11;; error = 0; count = 0;
        while( membyte < (unsigned char *)last ) { *membyte++ = byte; count++; byte = byte ^ count; }
        memword = (unsigned int *)start;
        while( memword < (unsigned int *)lastprint ) {
            if( ( (long)memword & 0xc ) == 0 ) printw( "0x%7x : ",(long)memword );
            printw("%08x ",*memword);
            if( ( (long)memword & 0xc ) == 0xc ) printw( "\n" );
            memword++;
        }
        membyte = (unsigned char *)start; byte = 0x11; count = 0;
        while( membyte < (unsigned char *)last ) {
            if( *membyte != byte ) {
                if( !error ) printw( "  ERROR (byte) at 0x%07x, %02x not %02x via CACHE\n", membyte, *membyte, byte );
                error++;
            }
            membyte++; count++; byte = byte ^ count;
        }
        printw( "  Complete (byte) with %d errors\n\n",error); sleep1khz( 2000, 0 );

        printw( "Start (half)\n" );
        memhalf = (unsigned short *)start; half = 0x2211; error = 0; count = 0;
        while( memhalf < (unsigned short *)last ) { *memhalf++ = half; count++; half = half ^ count ^ ( count << 8 ); }
        memword = (unsigned int *)start;
        while( memword < (unsigned int *)lastprint ) {
            if( ( (long)memword & 0xc ) == 0 ) printw( "0x%7x : ",(long)memword );
            printw("%08x ",*memword);
            if( ( (long)memword & 0xc ) == 0xc ) printw( "\n" );
            memword++;
        }
        memhalf = (unsigned short *)start; half = 0x2211; count = 0;
        while( memhalf < (unsigned short *)last ) {
            if( *memhalf != half ) {
                if( !error ) printw( "  ERROR (half) at 0x%07x, %04x not %04x\n", memhalf, *memhalf, half );
                error++;
            }
            memhalf++; count++; half = half ^ count ^ ( count << 8 );
        }
        printw( "  Complete (half) with %d errors\n\n",error); sleep1khz( 2000, 0 );

        printw( "Start (word)\n" );
        memword = (unsigned int *)start; word = 0x44332211; error = 0; count = 0;
        while( memword < (unsigned int *)last ) { *memword++ = word; count++; word = word ^ count ^ ( count << 8 ) ^ ( count << 16 ) ^ ( count << 24 ); }
        membyte = (unsigned char *)start;
        while( membyte < (unsigned char *)lastprint ) {
            if( ( (long)membyte & 0xf ) == 0 ) printw( "0x%7x : ",(long)membyte );
            printw("%02x ",*membyte);
            if( ( (long)membyte & 0xf ) == 0xf ) printw( "\n" );
            membyte++;
        }
        memword = (unsigned int *)start;
        while( memword < (unsigned int *)lastprint ) {
            if( ( (long)memword & 0xc ) == 0 ) printw( "0x%7x : ",(long)memword );
            printw("%08x ",*memword);
            if( ( (long)memword & 0xc ) == 0xc ) printw( "\n" );
            memword++;
        }
        memword = (unsigned int *)start; word = 0x44332211; count = 0;
        while( memword < (unsigned int *)last ) {
            if( *memword != word ) {
                if( !error ) printw( "  ERROR (word) at 0x%07x, %08x not %08x\n", memword, *memword, word );
                error++;
            }
            memword++; count++; word = word ^ count ^ ( count << 8 ) ^ ( count << 16 ) ^ ( count << 24 );
        }
        printw( "  Complete (word) with %d errors\n\n",error); sleep1khz( 2000, 0 );
    }

    sleep1khz( 2000, 0 );
}
