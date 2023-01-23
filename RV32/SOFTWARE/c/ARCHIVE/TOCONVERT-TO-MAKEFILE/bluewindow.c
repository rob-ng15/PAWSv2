#include "library/PAWSlibrary.h"

int main( void ) {
    // QUICK TEST OF THE TERMINAL WINDOW
    terminal_showhide( 1 );
    terminal_cs();

    terminal_print("Hello, I'm the little blue terminal window!\n");
    sleep1khz( 4000, 0 );
    for( int i = 0; i < 16; i++ ) {
        terminal_printf("Counting %d, Floating Point Random Number %f\n",i,frng());
        sleep1khz( 500, 0 );
    }
    terminal_printf("\nBye!");
    sleep1khz( 4000, 0 );
    terminal_showhide( 0 );
}

// EXIT WILL RETURN TO BIOS
