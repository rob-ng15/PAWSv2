#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>
#include <curses.h>
#include <PAWSlibrary.h>

unsigned char tune_treble[] = {  51, 51, 75, 75, 65, 65, 59, 59, 75, 63, 51, 51, 57, 57, 0, 0,
                                 75, 75, 77, 77, 65, 65, 61, 61, 77, 65, 53, 53, 61, 61, 0, 0,
                                 51, 51, 75, 75, 65, 65, 59, 59, 75, 63, 51, 51, 57, 57, 0, 0,
                                 59, 61, 63,  0, 63, 65, 65,  0, 65, 69, 71,  0, 75, 75, 75, 0 };
unsigned char tune_bass[] = {   27,  0,  0, 41, 27,  0,  0, 43,
                                29,  0,  0, 43, 29,  0,  0, 41,
                                27,  0,  0, 41, 27,  0,  0, 43,
                                41,  0, 43,  0, 47,  0,  51, 0 };

int main( void ) {
    ps2_keyboardmode(PS2_KEYBOARD);

    // set up curses library
    initscr();
    start_color();

    move( 0, 0 );
    for( int i = 1; i < 8 ; i++ ) {
        attron( COLOR_PAIR(i) );
        printw( "Terminal Test: Colour <%d>\n", i );
    }
    printw( "\nPS/2 Keyboard Test PS/2 in WHITE, UART in YELLOW\n\n");

    printw( "TIMER REG[0] = %8x, REG[1] = %8x, REG[2] = %8x\n",TIMER_REGS[8], TIMER_REGS[9], TIMER_REGS[10] );
    printw( "RTC   REG[0] = %8x, REG[1] = %8x\n", IO_REGS[384], IO_REGS[385] );

    struct timeval tv; struct timezone tz;
    gettimeofday( &tv, &tz );
    printw( "GTD = %ld =  %s\n",(long)tv.tv_sec,ctime( &tv.tv_sec ) );

    int cursor_x, cursor_y;
    unsigned short thecharacter;

    sample_upload( CHANNEL_LEFT, 64, &tune_treble[0] ); sample_upload( CHANNEL_RIGHT, 32, &tune_bass[0] );
    beep( CHANNEL_LEFT, WAVE_SAMPLE | WAVE_SINE, 0, 8 << 3 );
    beep( CHANNEL_RIGHT, WAVE_SAMPLE | WAVE_SINE, 0, 16 << 3 );

    while(1) {
        if( ps2_character_available() ) {
            thecharacter = ps2_inputcharacter();
            if( thecharacter & 0x100 ) {
                attron( COLOR_PAIR(6) ); getyx( &cursor_y, &cursor_x );
                // ESCAPE CHARACTER
                getyx( &cursor_y, &cursor_x );
                switch( thecharacter ) {
                    case 0x141: // UP
                        move( cursor_y != 0 ? cursor_y - 1 : LINES-1, cursor_x );
                        break;
                    case 0x142: // DOWN
                        move( cursor_y != LINES-1 ? cursor_y + 1 : 0, cursor_x );
                        break;
                    case 0x143: // RIGHT
                        if( ( cursor_y == LINES-1 ) && ( cursor_x == COLS-1 ) ) {
                            move( 0, 0 );
                        } else {
                            move( cursor_x == COLS-1 ? cursor_y + 1 : cursor_y, cursor_x == COLS-1 ? 0 : cursor_x + 1 );
                        }
                        break;
                    case 0x144: // LEFT
                        if( ( cursor_y == 0 ) && ( cursor_x == 0 ) ) {
                            move( LINES-1, COLS-1 );
                        } else {
                            move( cursor_x == 0 ? cursor_y - 1 : cursor_y, cursor_x == 0 ? COLS-1 : cursor_x - 1 );
                        }
                        break;
                    case 0x131: // HOME
                        move( cursor_y, 0 ); printw("HOME");
                        break;
                    case 0x134: // END
                        move( cursor_y, COLS-4 ); printw("END");
                        break;
                    case 0x135: // PGUP
                        move( 0, cursor_x ); printw("PGUP");
                        break;
                    case 0x136: // PGDN
                        move( LINES-1, cursor_x ); printw("PGDN");
                        break;
                    case 0x133: // DELETE
                        deleteln(); printw("DELETE");
                        break;
                    case 0x132: // INSERT
                        clrtoeol(); printw("INSERT");
                        break;
                    default: // PROBABLY A FUNCTION KEY
                        attron( A_BOLD );
                        printw("F%d",( thecharacter & 0xf ) + 12 * ( ( thecharacter & 0x10 ) >> 4 ) );
                        attron( A_NORMAL );
                }
            } else {
                // PRINTABLE CHARACTER
                attron( COLOR_PAIR(7) ); addch( (unsigned char )thecharacter & 0xff );
            }
            fprintf(stderr,"PS/2 = %x\n",thecharacter);
        }
        if( uart_character_available() ) {
            attron( COLOR_PAIR(3) ); thecharacter = uart_inputcharacter(); addch( thecharacter );
            fprintf(stderr,"UART = %x\n",thecharacter);
        }

        refresh();
    }
}
