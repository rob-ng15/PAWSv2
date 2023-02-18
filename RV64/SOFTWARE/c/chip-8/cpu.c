#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "chip-8.h"

struct C8 machine;                                                                                                              // INSTANCE OF THE CHIP8 MACHINE

// SIMPLE ACCESS TO THE FLAG REGISTER
#define VF machine.V[15]

// DISPLAY FUNCTIONS
int set_pixel( int p, int x, int y ) {
    int max_x = ( 64 << machine.HIRES ) - 1;                                                                                    // MAX COORDINATE MASK FOR WRAPPING
    int max_y = ( 32 << machine.HIRES ) - 1;                                                                                    // MAX COORDINATE MASK FOR WRAPPING

    if( ( machine.MODE != XOCHIP ) && ( ( x > max_x ) || ( y > max_y ) ) ) {
        return( 0 );                                                                                                            // OUT OF RANGE AND NOT XO-CHIP, NO ACTION
    } else {
        x = ( x & max_x ) << ( 1 - machine.HIRES );                                                                             // WRAP AND ADJUST IF LORES
        y = ( y & max_y ) << ( 1 - machine.HIRES );
    }

    int temp = _rv64_bext( machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ], 64 - ( x & 63 ) );                                  // SAVE CURRENT PIXEL

    switch( machine.HIRES ) {
        case 0:                                                                                                                 // HIRES SET 2x2 PIXELS
            machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ], 64 - ( x ) );
            machine.DISPLAY[ p ][ y ][ _rv64_bext( x + 1, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y ][ _rv64_bext( x + 1, 6 ) ], 64 - ( x + 1 ) );
            machine.DISPLAY[ p ][ y + 1 ][ _rv64_bext( x, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y + 1 ][ _rv64_bext( x, 6 ) ], 64 - ( x ) );
            machine.DISPLAY[ p ][ y + 1 ][ _rv64_bext( x + 1, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y + 1 ][ _rv64_bext( x + 1, 6 ) ], 64 - ( x + 1 ) );
            break;
        case 1:                                                                                                                 // HIRES SET 1 PIXEL
            machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ] = _rv64_binv( machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ], 64 - ( x ) );
            break;
    }
    return( temp );
}

int drawsprite( uint8_t xc, uint8_t yc, uint16_t i, uint8_t n ) {
    int pixelflag, pixelerased = 0, rowpixelerased, hirescount = 0;
    int max_x = ( 64 << machine.HIRES ) - 1;
    int max_y = ( 32 << machine.HIRES ) - 1;

    xc = xc & max_x; yc = yc & max_y;                                                                                           // WRAP COORDINATES IF REQUIRED

    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    if( n ) {
        for( int p = min_p; p < max_p; p++ ) {
            for( int y = 0; ( y < n ); y++ ) {                                                                                     // N != 0, DRAW STANDARD 8 x n SPRITE
                rowpixelerased = 0;
                for( int x = 0; ( x < 8 ); x++ ) {
                    if( _rv64_bext( machine.MEMORY[ i ], 7 - x ) ) {
                        pixelflag = set_pixel( p, xc + x, yc + y );
                        pixelerased |= pixelflag;
                        rowpixelerased |= pixelflag;
                    }
                }
                hirescount += rowpixelerased;
                i = ADDRESS( i, 1 );
            }
        }
    } else {                                                                                                                    // N == 0, DRAW 16 HEIGHT SPRITE
        for( int p = min_p; p < max_p; p++ ) {
            for( int y = 0; ( y < 16 ); y++ ) {
                uint16_t spritedata = FETCH( i );
                rowpixelerased = 0;
                for( int x = 0; ( x < 16 ); x++ ) {                                                                                     // ONLY DRAW 8 WIDE IF IN LORES
                    if( _rv64_bext( spritedata, 15 - x ) ) {
                        pixelflag = set_pixel( p, xc + x, yc + y );
                        pixelerased |= pixelflag;
                        rowpixelerased |= pixelflag;
                    }
                }
                hirescount += rowpixelerased;
                i = ADDRESS( i, 2 );
            }
        }
    }

    if( machine.HIRES )
        if( yc + ( ( n == 0 ) ? 16 : n ) > max_y )
            hirescount += ( yc + ( ( n == 0 ) ? 16 : n ) ) - max_y;

    return( machine.HIRES ? hirescount : pixelerased );
}

void scroll_left( void ) {
    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    for( int p = min_p; p < max_p; p++ ) {
        for( int y = 0; y < 64; y++ ) {
            int temp = _rv64_rol( machine.DISPLAY[ p ][ y ][ 1 ], 4 ) & 0xf;                                                         // EXTRACT PIXELS 64, 65, 66, 67
            machine.DISPLAY[ p ][ y ][ 0 ] = ( machine.DISPLAY[ p ][ y ][ 0 ] << 4 ) | temp;                                              // SHIFT LEFT PIXELS 0 - 63 AND OR IN PIXELS 64, 65, 66, 67
            machine.DISPLAY[ p ][ y ][ 1 ] = machine.DISPLAY[ p ][ y ][ 1 ] << 4;                                                         // SHIFT LEFT PIXELS 64 - 127
        }
    }
}

void scroll_right( void ) {
    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    for( int p = min_p; p < max_p; p++ ) {
        for( int y = 0; y < 64; y++ ) {
            int temp = _rv64_ror( machine.DISPLAY[ p ][ y ][ 0 ] & 0xf, 4 );                                                         // EXTRACT PIXELS 60, 61, 62, 63
            machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y ][ 0 ] >> 4;                                                         // SHIFT RIGHT PIXELS 0 - 63 AND OR IN PIXELS 64, 65, 66, 67
            machine.DISPLAY[ p ][ y ][ 1 ] = ( machine.DISPLAY[ p ][ y ][ 1 ] >> 4 ) | temp;                                              // SHIFT LEFT PIXELS 64 - 127
        }
    }
}

void scroll_down( uint8_t n ) {
    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    for( int p = min_p; p < max_p; p++ ) {
        for( int y = 63; y >= 0; y-- ) {
            if( y >= n ) {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y - n ][ 0 ];
                machine.DISPLAY[ p ][ y ][ 1 ] = machine.DISPLAY[ p ][ y - n ][ 1 ];
            } else {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y ][ 1 ] = 0;
            }
        }
    }
}

void scroll_up( uint8_t n ) {
    int min_p, max_p;
    switch( machine.PLANES ) {
        case 0: min_p = 0; max_p = 0; break;
        case 1: min_p = 0; max_p = 1; break;
        case 2: min_p = 1; max_p = 2; break;
        case 3: min_p = 0; max_p = 2; break;
    }

    for( int p = min_p; p < max_p; p++ ) {
        for( int y = 0; y < 64; y++) {
            if( y < ( 64 - n ) ) {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y + n ][ 0 ];
                machine.DISPLAY[ p ][ y ][ 1 ] = machine.DISPLAY[ p ][ y + n ][ 1 ];
            } else {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y ][ 1 ] = 0;
            }
        }
    }
}

void DEBUG( const char *fmt,... ) {
    if( machine.debug ) {
        static char buffer[1024];
        va_list args;
        va_start (args, fmt);
        vsnprintf( buffer, 80, fmt, args);
        va_end(args);

        uart_outputstring( buffer );
    }
}

void opcode_0( uint16_t instruction ) {
    uint8_t N = instruction & 0x000f;                                                                                           // EXTRACT LOW NIBBLE

    switch( instruction ) {
        case 0x00E0:                                                                                                    // CLEAR THE DISPLAY
            switch( machine.PLANES ) {
                case 0: break;
                case 1: memset( &machine.DISPLAY[0][0][0], 0, PLANESIZE ); break;
                case 2: memset( &machine.DISPLAY[1][0][0], 0, PLANESIZE ); break;
                case 3: memset( machine.DISPLAY, 0, DISPLAYSIZE ); break;
            }
            DEBUG( "cls");
            break;
        case 0x00EE:                                                                                                    // RETURN USE STACKTOP ENTRY
            if( machine.STACKTOP == -1 ) {
                machine.crashed = STACKUFLOW;                                                                           // CHECK FOR STACK UNDERFLOW
            } else {
                machine.PC = machine.STACK[ machine.STACKTOP-- ];                                                       // POP PC
            }
            DEBUG("return");
            break;
        case 0xFB:                                                                                                      // SCROLL DISPLAY RIGHT 4 PIXELS IN SCHIP / XOCHIP MODE
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                    machine.MODE = SCHIP;
                    DEBUG("*** -> SCHIP ");
                case SCHIP:
                case XOCHIP:
                    scroll_right();
                    break;
            }
            DEBUG("scroll(R)");
            break;
        case 0xFC:                                                                                                      // SCROLL DISPLAY LEFT 4 PIXELS IN SCHIP / XOCHIP MODE
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                    machine.MODE = SCHIP;
                    DEBUG("*** -> SCHIP ");
                case SCHIP:
                case XOCHIP:
                    scroll_left();
                    break;
            }
            DEBUG("scroll(L)");
            break;
        case 0xFD:                                                                                                      // EXIT
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                    machine.MODE = SCHIP;
                    DEBUG("*** -> SCHIP ");
                case SCHIP:
                case XOCHIP:
                    machine.running = 0;
                    break;
            }
            DEBUG("exit");
            break;
        case 0xFE:                                                                                                      // SWITCH TO LORES IF IN SCHIP / XOCHIP MODE
            switch( machine.MODE ) {
                case CHIP8:
                    machine.MODE = CHIP48;
                    DEBUG("*** -> CHIP48 ");
                case CHIP48:
                case SCHIP:
                case XOCHIP:
                    machine.HIRES = 0;
                    break;
            }
            DEBUG("display(L)");
            break;
        case 0xFF:                                                                                                      // SWITCH TO HIRES IF IN SCHIP / XOCHIP MODE
            switch( machine.MODE ) {
                case CHIP8:
                    machine.MODE = CHIP48;
                    DEBUG("*** -> CHIP48 ");
                case CHIP48:
                case SCHIP:
                case XOCHIP:
                    machine.HIRES = 1;
                    break;
            }
            DEBUG("display(H)");
            break;
        default:
            if( ( instruction & 0xfff0 ) == 0x00c0 ) {                                                                  // SCROLL DISPLAY N DOWN IF IN SCHIP / XOCHIP MODE
                switch( machine.MODE ) {
                    case CHIP8:
                    case CHIP48:
                        machine.MODE = SCHIP;
                        DEBUG("*** -> SCHIP ");
                    case SCHIP:
                    case XOCHIP:
                        scroll_down( N );
                        break;
                }
                DEBUG("scroll(D by %x)",N );
            } else {
                if( ( instruction & 0xfff0 ) == 0x00d0 ) {                                                                  // SCROLL DISPLAY N UP IF IN XOCHIP MODE
                    switch( machine.MODE ) {
                        case CHIP8:
                        case CHIP48:
                        case SCHIP:
                            machine.MODE = XOCHIP;
                            DEBUG("*** -> XOCHIP ");
                        case XOCHIP:
                            scroll_up( N );
                            break;
                    }
                    DEBUG("scroll(D by %x)",N );
                } else {
                    machine.crashed = INVALIDINSN;
                }
            }
            break;
    }

}

void opcode_1( uint16_t instruction ) {
    uint16_t NNN = instruction & 0x0fff;                                                                                        // EXTRACT ADDRESS ( LOWER 12 BITS )

    machine.PC = NNN;                                                                                                           // JUMP TO NNN
    DEBUG("jump %03x",NNN);
}

void opcode_2( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t Yn = ( instruction & 0x00f0 ) >> 4;                                                                                 // EXTRACT Y REGISTER NUMBER
    uint8_t Y = machine.V[ Yn ];                                                                                                // EXTRACT Y REGISTER
    uint8_t N = instruction & 0x000f;                                                                                           // EXTRACT LOW NIBBLE
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE
    uint16_t NNN = instruction & 0x0fff;                                                                                        // EXTRACT ADDRESS ( LOWER 12 BITS )
    uint16_t I = machine.I;                                                                                                     // CURRENT I VALUE
    uint16_t temp;

    if( machine.STACKTOP == 15 ) {
        machine.crashed = STACKOFLOW;
    } else {
        machine.STACK[ ++machine.STACKTOP ] = machine.PC;                                                                       // CALL PUSH ADDRESS
        machine.PC = NNN;                                                                                                       // JUMP TO NNN
    }
    DEBUG("call %03x",NNN);
}

void opcode_3( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE
    uint16_t temp;

    DEBUG("V%1x == %02x ( %02x == %02x ) ? ",Xn,NN,X,NN);
    if( X == NN ) {                                                                                                             // SKIP IF VX == NN
        temp = ( FETCH( machine.PC ) == 0xf000 ) ? 4 : 2;
        switch( machine.MODE ) {
            default:
                machine.PC = PADDRESS( machine.PC, 2 );
                break;
            case XOCHIP:
                machine.PC = PADDRESS( machine.PC, temp );
                break;
        }
        DEBUG("Y");
    } else {
        DEBUG("N");
    }
}

void opcode_4( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE
    uint16_t temp;

    DEBUG("V%1x != %02x ( %02x != %02x ) ? ",Xn,NN,X,NN);
    if( X != NN ) {                                                                                                             // SKIP IF VX != NN
        temp = ( FETCH( machine.PC ) == 0xf000 ) ? 4 : 2;
        switch( machine.MODE ) {
            default:
                machine.PC = PADDRESS( machine.PC, 2 );
                break;
            case XOCHIP:
                machine.PC = PADDRESS( machine.PC, temp );
                break;
        }
        DEBUG("Y");
    } else {
        DEBUG("N");
    }
}

void opcode_5( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t Yn = ( instruction & 0x00f0 ) >> 4;                                                                                 // EXTRACT Y REGISTER NUMBER
    uint8_t Y = machine.V[ Yn ];                                                                                                // EXTRACT Y REGISTER
    uint8_t N = instruction & 0x000f;                                                                                           // EXTRACT LOW NIBBLE
    uint16_t I = machine.I;                                                                                                     // CURRENT I VALUE
    uint16_t temp;

        switch( N ) {
        case 0:
            DEBUG("V%1x == V%1x ( %02x != %02x ) ? ",Xn,Yn,X,Y);
            temp = temp = ( FETCH( machine.PC ) == 0xf000 ) ? 4 : 2;
            if( X == Y ) {                                                                                                             // SKIP IF VX == VY
                switch( machine.MODE ) {
                    default:
                        machine.PC = PADDRESS( machine.PC, 2 );
                        break;
                    case XOCHIP:
                        machine.PC = PADDRESS( machine.PC, temp );
                        break;
                }
                DEBUG("Y");
            } else {
                DEBUG("N");
            }
            break;
        case 2:
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                case SCHIP:
                    machine.MODE = XOCHIP;
                    DEBUG("*** -> XOCHIP ");
                case XOCHIP:
                    if( Xn > Yn ) {
                        for( uint8_t n = Yn; n >= Xn; n-- )
                            machine.MEMORY[ I++ ] = machine.V[ n ];
                    } else {
                        for( uint8_t n = Xn; n <= Yn; n-- )
                            machine.MEMORY[ I++ ] = machine.V[ n ];
                    }
                    break;
            }
            break;
        case 3:
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                case SCHIP:
                    machine.MODE = XOCHIP;
                    DEBUG("*** -> XOCHIP ");
                case XOCHIP:
                    if( Xn > Yn ) {
                        for( uint8_t n = Yn; n >= Xn; n-- )
                            machine.V[ n ] = machine.MEMORY[ I++ ];
                    } else {
                        for( uint8_t n = Xn; n <= Yn; n-- )
                            machine.V[ n ] = machine.MEMORY[ I++ ];
                    }
                    break;
            }
            break;
        default:
            machine.crashed = INVALIDINSN;
            break;
    }
}

void opcode_6( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE

    machine.V[ Xn ] = NN;                                                                                                       // VX = NN
    DEBUG("V%1x = %02x",Xn,NN);
}

void opcode_7( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE

    machine.V[ Xn ] += NN;                                                                                                      // VX += NN, VF NO CHANGE
    DEBUG("V%1x += %02x ( %02x + %02x = %02x )",Xn,NN,X,NN,machine.V[ Xn ]);
}

void opcode_8( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t Yn = ( instruction & 0x00f0 ) >> 4;                                                                                 // EXTRACT Y REGISTER NUMBER
    uint8_t Y = machine.V[ Yn ];                                                                                                // EXTRACT Y REGISTER
    uint8_t N = instruction & 0x000f;                                                                                           // EXTRACT LOW NIBBLE
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE
    uint16_t NNN = instruction & 0x0fff;                                                                                        // EXTRACT ADDRESS ( LOWER 12 BITS )
    uint16_t I = machine.I;                                                                                                     // CURRENT I VALUE
    uint16_t temp;

    switch( N ) {
        case 0:
            machine.V[ Xn ] = Y;                                                                                                // Vx = Vy
            DEBUG("V%1x = V%1x",Xn,Yn);
            break;
        case 1:
            machine.V[ Xn ] = X | Y;                                                                                            // Vx |= Vy
            if( machine.MODE == CHIP8 ) VF = 0;
            DEBUG("V%1x |= V%1x ( %02x | %02x = %02x )",Xn,Yn,X,Y,machine.V[Xn]);
            break;
        case 2:
            machine.V[ Xn ] = X & Y;                                                                                            // Vx &= Vy
            if( machine.MODE == CHIP8 ) VF = 0;
            DEBUG("V%1x &= V%1x ( %02x & %02x = %02x )",Xn,Yn,X,Y,machine.V[Xn]);
            break;
        case 3:
            machine.V[ Xn ] = X ^ Y;                                                                                            // Vx ^= Vy
            if( machine.MODE == CHIP8 ) VF = 0;
            DEBUG("V%1x ^= V%1x ( %02x ^ %02x = %02x )",Xn,Yn,X,Y,machine.V[Xn]);
            break;
        case 4:
            temp = X + Y;                                                                                                       // Vx += Vy
            machine.V[ Xn ] = temp & 0xff;
            VF = ( temp > 0xff ) ? 1 : 0;
            DEBUG("V%1x += V%1x ( %02x + %02x = (%1x)%02x )",Xn,Yn,X,Y,VF,machine.V[Xn]);
            break;
        case 5:
            temp = X - Y;                                                                                                       // Vx -= Vy
            machine.V[ Xn ] = temp & 0xff;
            VF = ( temp > 0xff ) ? 0 : 1;
            DEBUG("V%1x -= V%1x ( %02x - %02x = (%1x)%02x )",Xn,Yn,X,Y,VF,machine.V[Xn]);
            break;
        case 6:
            switch( machine.MODE ) {
                case CHIP8:
                case XOCHIP:
                    machine.V[ Xn ] = Y >> 1;
                    VF = _rv64_bext( Y, 0 );
                    DEBUG("V%1x = V%1x >> 1 ( %02x >> 1 = (%1x)%02x )",Xn,Yn,Y,VF,machine.V[Xn]);
                    break;
                case CHIP48:
                case SCHIP:
                    machine.V[ Xn ] = X >> 1;
                    VF = _rv64_bext( X, 0 );
                    DEBUG("V%1x >>= 1 ( %02x >> 1 = (%1x)%02x )",Xn,X,VF,machine.V[Xn]);
                    break;
            }
            break;
        case 7:
            temp = Y - X;
            machine.V[ Xn ] = temp & 0xff;
            VF = ( temp > 0xff ) ? 0 : 1;
            break;
        case 0xe:
            switch( machine.MODE ) {
                case CHIP8:
                case XOCHIP:
                    machine.V[ Xn ] = Y << 1;
                    VF = _rv64_bext( Y, 7 );
                    DEBUG("V%1x = V%1x << 1 ( %02x << 1 = (%1x)%02x )",Xn,Yn,Y,VF,machine.V[Xn]);
                    break;
                case CHIP48:
                case SCHIP:
                    machine.V[ Xn ] = X << 1;
                    VF = _rv64_bext( X, 7 );
                    DEBUG("V%1x <<= 1 ( %02x << 1 = (%1x)%02x )",Xn,X,VF,machine.V[Xn]);
                    break;
            }
            break;
        default:
            machine.crashed = INVALIDINSN;
    }
}

void opcode_9( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t Yn = ( instruction & 0x00f0 ) >> 4;                                                                                 // EXTRACT Y REGISTER NUMBER
    uint8_t Y = machine.V[ Yn ];                                                                                                // EXTRACT Y REGISTER
    uint16_t temp;

    DEBUG("V%1x != V%1x ( %02x != %02x ) ? ",Xn,Yn,X,Y);
    if( X != Y ) {                                                                                                             // SKIP IF VX == VY
        temp = ( FETCH( machine.PC ) == 0xf000 ) ? 4 : 2;
        switch( machine.MODE ) {
            default:
                machine.PC = PADDRESS( machine.PC, 2 );
                break;
            case XOCHIP:
                machine.PC = PADDRESS( machine.PC, temp );
                break;
        }
        DEBUG("Y");
    } else {
        DEBUG("N");
    }
}

void opcode_A( uint16_t instruction ) {
    uint16_t NNN = instruction & 0x0fff;                                                                                        // EXTRACT ADDRESS ( LOWER 12 BITS )

    machine.I = NNN;                                                                                                            // I = NNN
    DEBUG("I = %03x",NNN);
}

void opcode_B( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint16_t NNN = instruction & 0x0fff;                                                                                        // EXTRACT ADDRESS ( LOWER 12 BITS )

    switch( machine.MODE ) {
        case CHIP8:
        case CHIP48:
        case XOCHIP:
            machine.PC = PADDRESS( machine.PC, machine.V[ 0 ] );
            DEBUG("branch NNN + V0 ( %03x + %02x )",NNN,machine.V[ 0 ]);
            break;
        case SCHIP:
            machine.PC = PADDRESS( machine.PC, X );
            DEBUG("branch NNN + V%1x ( %03x + %02x )",Xn,NNN,X);
            break;
    }
}

void opcode_C( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE

    machine.V[ Xn ] = rng( 256 ) & NN;                                                                                          // Vx = RNG & NN
    DEBUG("V%1x = RNG & %02x",Xn,NN);
}

void opcode_D( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t Yn = ( instruction & 0x00f0 ) >> 4;                                                                                 // EXTRACT Y REGISTER NUMBER
    uint8_t Y = machine.V[ Yn ];                                                                                                // EXTRACT Y REGISTER
    uint8_t N = instruction & 0x000f;                                                                                           // EXTRACT LOW NIBBLE
    uint16_t I = machine.I;                                                                                                     // CURRENT I VALUE

    switch( machine.MODE ) {
        case CHIP8:
            if( N == 0 ) {
                machine.MODE = CHIP48;
                DEBUG("*** -> CHIP48 ");
            }
        default:
            VF = drawsprite( X, Y, machine.I, N );
            break;
    }
}

void opcode_E( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE
    uint16_t temp;

    temp = ( FETCH( machine.PC ) == 0xf000 ) ? 4 : 2;
    switch( NN ) {
        case 0x9E:
            DEBUG("key %1x down ? ",X & 0xf );
            if( _rv64_bext( machine.KEYS, X & 0xf ) ) {                                                                         // SKIP IF KEY X DOWN
                switch( machine.MODE ) {
                    default:
                        machine.PC = PADDRESS( machine.PC, 2 );
                        break;
                    case XOCHIP:
                        machine.PC = PADDRESS( machine.PC, temp );
                        break;
                }
                DEBUG("Y");
            } else {
                DEBUG("N");
            }
            break;
        case 0xA1:
            DEBUG("key %1x up ? ",X & 0xf );
            if( !_rv64_bext( machine.KEYS, X & 0xf ) ) {                                                                        // SKIP IF KEY X UP
                switch( machine.MODE ) {
                    default:
                        machine.PC = PADDRESS( machine.PC, 2 );
                        break;
                    case XOCHIP:
                        machine.PC = PADDRESS( machine.PC, temp );
                        break;
                }
                DEBUG("Y");
            } else {
                DEBUG("N");
            }
            break;
        default:
            machine.crashed = INVALIDINSN;
            break;
    }
}

void opcode_F( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t Yn = ( instruction & 0x00f0 ) >> 4;                                                                                 // EXTRACT Y REGISTER NUMBER
    uint8_t Y = machine.V[ Yn ];                                                                                                // EXTRACT Y REGISTER
    uint8_t N = instruction & 0x000f;                                                                                           // EXTRACT LOW NIBBLE
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE
    uint16_t NNN = instruction & 0x0fff;                                                                                        // EXTRACT ADDRESS ( LOWER 12 BITS )
    uint16_t I = machine.I;                                                                                                     // CURRENT I VALUE

    switch( NN ) {
        case 0x00:
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                case SCHIP:
                    machine.MODE = XOCHIP;
                    DEBUG("*** -> XOCHIP ");
                case XOCHIP:
                    machine.I = FETCH( machine.PC );
                    machine.PC = PADDRESS( machine.PC, 2 );
                    DEBUG("I = LONG( %04x )",machine.I );
                    break;
            }
            break;
        case 0x01:
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                case SCHIP:
                    machine.MODE = XOCHIP;
                    DEBUG("*** -> XOCHIP ");
                case XOCHIP:
                    machine.PLANES = Xn & 3;
                    DEBUG("set_planes %1x",Xn&3);
                    break;
            }
            break;
        case 0x02:
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                case SCHIP:
                    machine.MODE = XOCHIP;
                    DEBUG("*** -> XOCHIP ");
                case XOCHIP:
                    DEBUG("load_audio %04x",I);
                    break;
            }
            break;
        case 0x07:
            machine.V[ Xn ] = machine.timer;
            break;
        case 0x15:
            machine.timer = X;
            break;
        case 0x18:
            machine.audio_timer = X; beep( 3, 0, 49, (short)(X * 1000/60) );
            break;
        case 0x1e:
            machine.I = IADDRESS( I, X );
            VF = ( machine.I < ADDRESS( I, X ) ) ? 1 : 0;
            DEBUG("I += V%01x ( %04x + %02x = (%01x)%04x )",Xn,I,X,VF,machine.I);
            break;
        case 0x0a:
            if( machine.KEYS == 0 ) {
                machine.PC = PADDRESS( machine.PC, -2 );
            } else {
                machine.V[ Xn ] = _rv64_ctz( machine.KEYS );
            }
            DEBUG("wait_key");
            break;
        case 0x29:
            machine.I = IADDRESS( 0, 5 * ( X & 0xf ) );
            DEBUG("load_font %01x ( %04x )",X&0xf, machine.I);
            break;
        case 0x30:
            switch( machine.MODE ) {
                case CHIP8:
                    machine.MODE = CHIP48;
                    DEBUG("*** -> CHIP48");
                default:
                    machine.I = IADDRESS( 0x50, 10 * ( X & 0xf ) );
                    break;
            }
            DEBUG("load_big_font %01x ( %04x )",X&0xf, machine.I);
            break;
        case 0x33:
            machine.MEMORY[ IADDRESS( I, 0 ) ] = X / 100;
            machine.MEMORY[ IADDRESS( I, 1 ) ] = ( X % 100 ) / 10;
            machine.MEMORY[ IADDRESS( I, 2 ) ] = ( X % 10 );
            DEBUG("bcd V%01x @ %04x[ %02x -> %02x %02x %02x ]",Xn,I,X,machine.MEMORY[ IADDRESS( I, 0 ) ],machine.MEMORY[ IADDRESS( I, 1 ) ],machine.MEMORY[ IADDRESS( I, 2 ) ]);
            break;
        case 0x3a:
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                case SCHIP:
                    machine.MODE = XOCHIP;
                    DEBUG("*** -> XOCHIP ");
                case XOCHIP:
                    DEBUG("set_pitch %02x",X);
                    break;
            }
            break;
        case 0x55:
            for( int n = 0; n <= Xn; n++ )
                machine.MEMORY[ IADDRESS( I, n ) ] = machine.V[ n ];
            switch( machine.MODE ) {
                case CHIP8:
                    machine.I =  IADDRESS( I, Xn + 1 );
                    break;
                case CHIP48:
                    machine.I = IADDRESS( I, Xn );
                    break;
                case SCHIP:
                case XOCHIP:
                    break;
            }
            DEBUG("save V0 -> V%01x @ %04x ( I = %04x )",Xn,I,machine.I);
            break;
        case 0x65:
            for( int n = 0; n <= Xn; n++ )
                machine.V[ n ] = machine.MEMORY[ IADDRESS( I, n ) ];
            switch( machine.MODE ) {
                case CHIP8:
                    machine.I = IADDRESS( I, Xn + 1 );
                    break;
                case CHIP48:
                    machine.I = IADDRESS( I, Xn );
                    break;
                case SCHIP:
                case XOCHIP:
                    break;
            }
            DEBUG("load V0 -> V%01x @ %04x ( I = %04x )",Xn,I,machine.I);
            break;
        case 0x75:
            switch( machine.MODE ) {
                case CHIP8:
                    machine.MODE = CHIP48;
                    DEBUG("*** -> CHIP48 ");
                case CHIP48:
                    if( Xn > 7 ){
                        machine.MODE = SCHIP;
                        DEBUG("*** -> SCHIP ");
                    }
                default:
                    for( int n = 0; n <= Xn; n++ )
                        machine.FLAGS[ n ] = machine.V[ n ];
                    break;
            }
            break;
        case 0x85:
            switch( machine.MODE ) {
                case CHIP8:
                    machine.MODE = CHIP48;
                    DEBUG("*** -> CHIP48 ");
                case CHIP48:
                    if( Xn > 7 ){
                        machine.MODE = SCHIP;
                        DEBUG("*** -> SCHIP ");
                    }
                default:
                    for( int n = 0; n <= Xn; n++ )
                    machine.V[ n ] = machine.FLAGS[ n ];
                    break;
            }
            break;
        default:
            machine.crashed = INVALIDINSN;
            break;
    }
}

// JUMP TABLE FOR EACH OF THE OPCODES
void (*opcodes[])(uint16_t) = {
    opcode_0, opcode_1, opcode_2, opcode_3, opcode_4, opcode_5, opcode_6, opcode_7,
    opcode_8, opcode_9, opcode_A, opcode_B, opcode_C, opcode_D, opcode_E, opcode_F
};

void execute( void ) {
    uint16_t instruction = FETCH( machine.PC );                                                                                 // EXTRACT INSTRUCTION ( wrap PC if required )
    uint8_t O = ( instruction & 0xf000 ) >> 12;                                                                                 // EXTARCT FIRST NIBBLE, OPERAND

    DEBUG( "%03x : %04x : ", machine.PC, instruction );                                                                         // PRINT ADDRESS AND INSTRUCTION

    machine.lastPC = machine.PC; machine.lastinstruction = instruction;                                                         // STORE PC AND INSTRUCTION INCASE OF CRASH
    machine.PC = PADDRESS( machine.PC, 2 );                                                                                     // INCREMENT PC, WRAP AROUND IF REQUIRED

    opcodes[ O ]( instruction );                                                                                                // JUMP TO THE OPCODE
    DEBUG("\n");                                                                                                                // NEWLINE
}
