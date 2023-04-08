#include <stdio.h>

void clmul( unsigned long rs1, unsigned long rs2 ) {
    unsigned long result = 0;

    for( int i = 0; i < 64; i++ ) {
        if( ( rs2 >> i ) & 1 )
            result = result ^ ( rs1 << i );
    }
    printf("CLMUL 0x%x, 0x%x -> 0x%x\n",rs1,rs2,result);
}

int main( void ) {
    clmul( 0xa, 0x6 );
    clmul( 0x355, 0x487 );
}
