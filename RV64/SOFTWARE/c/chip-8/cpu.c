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
// PIXELS ARE STORED IN 2 UNSIGNED LONG IN ORDER 63 - 0, 127 - 64
// CHIP 8 USES 0 - 127
// NOTE SCROLL LEFT AND SCROLL RIGHT ARE THE OPPOSITE WAY AROUND
int set_pixel( int p, int x, int y ) {
    int max_x = ( 64 << machine.HIRES ) - 1;                                                                                    // MAX COORDINATE MASK FOR WRAPPING
    int max_y = ( 32 << machine.HIRES ) - 1;                                                                                    // MAX COORDINATE MASK FOR WRAPPING

    if( ( machine.MODE != XOCHIP ) && ( ( x > max_x ) || ( y > max_y ) ) ) {
        return( 0 );                                                                                                            // OUT OF RANGE AND NOT XO-CHIP, NO ACTION
    }

    x = ( x & max_x ) << ( 1 - machine.HIRES );                                                                                 // WRAP AND ADJUST IF LORES ( double from 0 - 63 to 0 - 126 )
    y = ( y & max_y ) << ( 1 - machine.HIRES );

    int section = ( x > 63 ) ? 1 : 0;                                                                                           // 1st OR 2nd 64 BITS?
    int bit = ( x & 63 );                                                                                                       // LIMIT TO 0 - 63
    int bitnext = ( x + 1 ) & 63;                                                                                               // ADJACENT BIT

    int temp = _rv64_bext( machine.DISPLAY[ p ][ y ][ _rv64_bext( x, 6 ) ], bit );                                              // SAVE CURRENT PIXEL

    switch( machine.HIRES ) {
        case 0:                                                                                                                 // LORES SET 2x2 PIXELS
            machine.DISPLAY[ p ][ y ][ section ] = _rv64_binv( machine.DISPLAY[ p ][ y ][ section ], bit );
            machine.DISPLAY[ p ][ y ][ section ] = _rv64_binv( machine.DISPLAY[ p ][ y ][ section ], bitnext );
            machine.DISPLAY[ p ][ y + 1 ][ section ] = _rv64_binv( machine.DISPLAY[ p ][ y + 1 ][ section ], bit );
            machine.DISPLAY[ p ][ y + 1 ][ section ] = _rv64_binv( machine.DISPLAY[ p ][ y + 1 ][ section ], bitnext );
            break;
        case 1:                                                                                                                 // HIRES SET 1 PIXEL
            machine.DISPLAY[ p ][ y ][ section ] = _rv64_binv( machine.DISPLAY[ p ][ y ][ section ], bit );
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
            for( int y = 0; ( y < n ); y++ ) {                                                                                  // N != 0, DRAW STANDARD 8 x n SPRITE
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
            machine.DISPLAY[ p ][ y ][ 0 ] = ( machine.DISPLAY[ p ][ y ][ 0 ] >> 4 ) |                                          // SHIFT RIGHT PIXELS 0 - 63 AND OR IN PIXELS 64, 65, 66, 67
                                             _rv64_ror( machine.DISPLAY[ p ][ y ][ 1 ] & 0xf, 4 );
            machine.DISPLAY[ p ][ y ][ 1 ] = ( machine.DISPLAY[ p ][ y ][ 1 ] >> 4 );                                           // SHIFT RIGHT PIXELS 64 - 127
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
            machine.DISPLAY[ p ][ y ][ 1 ] = ( machine.DISPLAY[ p ][ y ][ 1 ] << 4 ) |                                          // SHIFT LEFT PIXELS 64 - 127 AND OR IN PIXELS 64, 65, 66, 67
                                             ( _rv64_rol( machine.DISPLAY[ p ][ y ][ 0 ], 4 ) & 0xf );
            machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y ][ 0 ] << 4;                                               // SHIFT LEFT PIXELS 0 - 63
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
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y - n ][ 0 ];                                            // COPY ROWS FROM ABOVE
                machine.DISPLAY[ p ][ y ][ 1 ] = machine.DISPLAY[ p ][ y - n ][ 1 ];
            } else {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y ][ 1 ] = 0;                                            // ERASE TOP ROWS
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
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y + n ][ 0 ];                                            // COPY ROWS FROM BELOW
                machine.DISPLAY[ p ][ y ][ 1 ] = machine.DISPLAY[ p ][ y + n ][ 1 ];
            } else {
                machine.DISPLAY[ p ][ y ][ 0 ] = machine.DISPLAY[ p ][ y ][ 1 ] = 0;                                            // ERASE BOTTOM ROWS
            }
        }
    }
}

void cls( void ) {                                                                                                              // CLEAR SCREEN OBEYING PLANES
    switch( machine.PLANES ) {
        case 0: break;
        case 1: memset( &machine.DISPLAY[0][0][0], 0, PLANESIZE ); break;
        case 2: memset( &machine.DISPLAY[1][0][0], 0, PLANESIZE ); break;
        case 3: memset( machine.DISPLAY, 0, DISPLAYSIZE ); break;
    }
}

void cls_all( void ) {                                                                                                          // CLEAR SCREEN FOR XOCHIP RESOLUTION CHANGE
    memset( machine.DISPLAY, 0, DISPLAYSIZE );
}

void DEBUG( const char *fmt,... ) {                                                                                             // OUTPUT INSTRUCTION IF DEBUG IS ACTIVE
    if( machine.debug ) {
        static char buffer[1024];
        va_list args;
        va_start (args, fmt);
        vsnprintf( buffer, 80, fmt, args);
        va_end(args);

        uart_outputstring( buffer );
    }
}

uint16_t skip( void ) {                                                                                                         // CHECK IF SKIPPING 2 OR 4 BYTES
    uint16_t temp;                                                                                                              // XOCHIP SKIPS 4 IF 0xf000 IS FIRST 2

    switch( machine.MODE ) {
        default:
            return( 2 );
            break;
        case XOCHIP:
            temp = FETCH( machine.PC );
            if( temp == 0xf000 )
                return( 4 );
            else
                return( 2 );
            break;
    }
}

void opcode_0( uint16_t instruction ) {
    uint8_t N = instruction & 0x000f;                                                                                           // EXTRACT LOW NIBBLE

    switch( instruction ) {
        case 0x00E0:                                                                                                            // CLEAR THE DISPLAY
            cls();
            DEBUG( "cls");
            break;
        case 0x00EE:                                                                                                            // RETURN, USE STACKTOP ENTRY
            if( machine.STACKTOP == -1 ) {
                machine.crashed = STACKUFLOW;                                                                                   // CHECK FOR STACK UNDERFLOW
            } else {
                machine.PC = machine.STACK[ machine.STACKTOP-- ];                                                                // POP PC
            }
            DEBUG("return");
            break;
        case 0xFB:                                                                                                              // SCROLL DISPLAY RIGHT 4 PIXELS IN SCHIP / XOCHIP MODE
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
        case 0xFC:                                                                                                              // SCROLL DISPLAY LEFT 4 PIXELS IN SCHIP / XOCHIP MODE
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
        case 0xFD:                                                                                                              // EXIT
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
        case 0xFE:                                                                                                              // SWITCH TO LORES IF IN CHIP48 / SCHIP / XOCHIP MODE
            switch( machine.MODE ) {
                case CHIP8:
                    machine.MODE = CHIP48;
                    DEBUG("*** -> CHIP48 ");
                case CHIP48:
                case SCHIP:
                    machine.HIRES = 0;
                    break;
                case XOCHIP:
                    machine.HIRES = 0;
                    cls_all();
                    break;
            }
            DEBUG("display(L)");
            break;
        case 0xFF:                                                                                                              // SWITCH TO HIRES IF IN CHIP48 / SCHIP / XOCHIP MODE
            switch( machine.MODE ) {
                case CHIP8:
                    machine.MODE = CHIP48;
                    DEBUG("*** -> CHIP48 ");
                case CHIP48:
                case SCHIP:
                    machine.HIRES = 1;
                    break;
                case XOCHIP:
                    machine.HIRES = 1;
                    cls_all();
                    break;
            }
            DEBUG("display(H)");
            break;
        default:
            if( ( instruction & 0xfff0 ) == 0x00c0 ) {                                                                          // SCROLL DISPLAY N DOWN IF IN SCHIP / XOCHIP MODE
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
                if( ( instruction & 0xfff0 ) == 0x00d0 ) {                                                                      // SCROLL DISPLAY N UP IF IN XOCHIP MODE
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
                    DEBUG("scroll(U by %x)",N );
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
    uint16_t NNN = instruction & 0x0fff;                                                                                        // EXTRACT ADDRESS ( LOWER 12 BITS )

    if( machine.STACKTOP == 15 ) {
        machine.crashed = STACKOFLOW;
    } else {
        machine.STACK[ ++machine.STACKTOP ] = machine.PC;                                                                       // CALL, PUSH ADDRESS
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
        DEBUG("[ %04x] ",FETCH( machine.PC ));
        temp = skip(); machine.PC = PADDRESS( machine.PC, temp );
        DEBUG("Y SKIP %1x",temp);
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
        DEBUG("[ %04x] ",FETCH( machine.PC ));
        temp = skip(); machine.PC = PADDRESS( machine.PC, temp );
        DEBUG("Y SKIP %1x",temp);
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
            if( X == Y ) {                                                                                                      // SKIP IF VX == VY
                DEBUG("[ %04x] ",FETCH( machine.PC ));
                temp = skip(); machine.PC = PADDRESS( machine.PC, temp );
                DEBUG("Y SKIP %1x",temp);
            } else {
                DEBUG("N");
            }
            break;
        case 2:
            DEBUG("save V%1x -> V%01x @ %04x ( I stays )",Xn,Yn,I);
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                case SCHIP:
                    machine.MODE = XOCHIP;
                    DEBUG("*** -> XOCHIP ");
                case XOCHIP:                                                                                                    // XOCHIP SAVES Vx to Vy ( reverse order possible ) TO MEMORY @ I
                    if( Xn > Yn ) {
                        for( uint8_t n = Yn; n >= Xn; n-- )
                            machine.MEMORY[ I++ ] = machine.V[ n ];
                    } else {
                        memcpy( &machine.MEMORY[ I ], &machine.V[ Xn], ( Yn - Xn ) + 1 );
                        //for( uint8_t n = Xn; n <= Yn; n++ )
                        //    machine.MEMORY[ I++ ] = machine.V[ n ];
                    }
                    break;
            }
            break;
        case 3:
            DEBUG("load V%1x -> V%01x @ %04x ( I stays )",Xn,Yn,I);
            switch( machine.MODE ) {
                case CHIP8:
                case CHIP48:
                case SCHIP:
                    machine.MODE = XOCHIP;
                    DEBUG("*** -> XOCHIP ");
                case XOCHIP:                                                                                                // XOCHIP LOADS Vx to Vy ( reverse order possible ) FROM MEMORY @ I
                    if( Xn > Yn ) {
                        for( uint8_t n = Yn; n >= Xn; n-- )
                            machine.V[ n ] = machine.MEMORY[ I++ ];
                    } else {
                        memcpy( &machine.V[ Xn], &machine.MEMORY[ I ], ( Yn - Xn ) + 1 );
                        //for( uint8_t n = Xn; n <= Yn; n++ )
                        //    machine.V[ n ] = machine.MEMORY[ I++ ];
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
            switch( machine.MODE ) {
                case CHIP8:
                    VF = 0;
                    break;
                default:
                    break;
            }
            DEBUG("V%1x |= V%1x ( %02x | %02x = %02x )",Xn,Yn,X,Y,machine.V[Xn]);
            break;
        case 2:
            machine.V[ Xn ] = X & Y;                                                                                            // Vx &= Vy
            switch( machine.MODE ) {
                case CHIP8:
                    VF = 0;
                    break;
                default:
                    break;
            }
            DEBUG("V%1x &= V%1x ( %02x & %02x = %02x )",Xn,Yn,X,Y,machine.V[Xn]);
            break;
        case 3:
            machine.V[ Xn ] = X ^ Y;                                                                                            // Vx ^= Vy
            switch( machine.MODE ) {
                case CHIP8:
                    VF = 0;
                    break;
                default:
                    break;
            }
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
            DEBUG("V%1x = V%1x - V%1x( %02x - %02x = (%1x)%02x )",Xn,Yn,Xn,Y,X,VF,machine.V[Xn]);
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
        DEBUG("[ %04x] ",FETCH( machine.PC ));
        temp = skip(); machine.PC = PADDRESS( machine.PC, temp );
        DEBUG("Y SKIP %1x",temp);
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
            machine.PC = PADDRESS( NNN, machine.V[ 0 ] );
            DEBUG("branch %03x + V0 ( %03x + %02x )",NNN,machine.V[ 0 ]);
            break;
        case SCHIP:
            machine.PC = PADDRESS( NNN, X );
            DEBUG("branch %03x + V%1x ( %03x + %02x )",Xn,NNN,X);
            break;
    }
}

void opcode_C( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE

    machine.V[ Xn ] = rng( 256 ) & NN;                                                                                          // Vx = RNG & NN
    DEBUG("V%1x = RNG & %02x ( %02x )",Xn,NN,machine.V[ Xn ]);
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
            await_vblank();                                                                                                     // WAIT FOR VBLANK FOR CHIP8
            if( N == 0 ) {
                machine.MODE = CHIP48;
                DEBUG("*** -> CHIP48 ");
            }
        default:
            VF = drawsprite( X, Y, machine.I, N );
            break;
    }
    DEBUG("draw %04x @ ( %02x, %02x) x %1x",I,X,Y, N ? N : 16 );
}

void opcode_E( uint16_t instruction ) {
    uint8_t Xn = ( instruction & 0x0f00 ) >> 8;                                                                                 // EXTRACT X REGISTER NUMBER
    uint8_t X = machine.V[ Xn ];                                                                                                // EXTRACT X REGISTER
    uint8_t NN = instruction & 0x00ff;                                                                                          // EXTRACT LOW BYTE
    uint16_t temp;

    DEBUG("[ %04x] ",FETCH( machine.PC ));
    switch( NN ) {
        case 0x9E:
            DEBUG("key %1x down ? ",X & 0xf );
            if( _rv64_bext( machine.KEYS, X & 0xf ) ) {                                                                         // SKIP IF KEY X DOWN
                DEBUG("[ %04x] ",FETCH( machine.PC ));
                temp = skip(); machine.PC = PADDRESS( machine.PC, temp );
                DEBUG("Y SKIP %1x",temp);
            } else {
                DEBUG("N");
            }
            break;
        case 0xA1:
            DEBUG("key %1x up ? ",X & 0xf );
            if( !_rv64_bext( machine.KEYS, X & 0xf ) ) {                                                                        // SKIP IF KEY X UP
                DEBUG("[ %04x] ",FETCH( machine.PC ));
                temp = skip(); machine.PC = PADDRESS( machine.PC, temp );
                DEBUG("Y SKIP %1x",temp);
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
                case XOCHIP:                                                                                                    // XOCHIP FETCH 16BIT I VALUE
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
                case XOCHIP:                                                                                                    // XOCHIP SET PLANES VALUE ( 0 - 3 )
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
                case XOCHIP:                                                                                                    // XOCHIP UPLOAD 16 BYTE BIT SAMPLE
                    bitsample_upload_128( 3, &machine.MEMORY[ I ] );
                    DEBUG("load_audio %04x",I);
                    break;
            }
            break;
        case 0x07:
            machine.V[ Xn ] = machine.timer;                                                                                    // COPY TIMER TO Vx
            DEBUG("V%1x = timer",Xn);
            break;
        case 0x0a:
            if( machine.KEYS == 0 ) {                                                                                           // WAIT FOR KEY PRESS AND RELEASE
                machine.PC = PADDRESS( machine.PC, -2 );
            } else {
                machine.V[ Xn ] = _rv64_ctz( machine.KEYS );                                                                    // RETURNS LOWEST KEY PRESSED
                while( machine.KEYS != 0 );
            }
            DEBUG("wait_key");
            break;
        case 0x15:
            machine.timer = X;                                                                                                  // SET TIMER TO Vx
            DEBUG("timer = %02x",NN);
            break;
        case 0x18:
            machine.audio_timer = X;                                                                                            // SET AUDIO TIMER
            if( X ) {
                beep( 3, ( machine.MODE == XOCHIP ) ? WAVE_BITS : WAVE_SQUARE, machine.PITCH, (short)(X * 1000/60) );
            } else {
                beep( 3, 0, 0, 0 );
            }
            DEBUG("audio_timer = %02x",NN);
            break;
        case 0x1e:
            machine.I = IADDRESS( I, X );                                                                                       // I += Vx
            VF = ( machine.I < I ) ? 1 : 0;                                                                                     // SET CARRY FLAG
            DEBUG("I += V%01x ( %04x + %02x = (%01x)%04x )",Xn,I,X,VF,machine.I);
            break;
        case 0x29:
            machine.I = IADDRESS( 0, 5 * ( X & 0xf ) );                                                                         // SET ADDRESS TO SMALL FONT
            DEBUG("load_font %01x ( %04x )",X&0xf, machine.I);
            break;
        case 0x30:
            switch( machine.MODE ) {
                case CHIP8:
                    machine.MODE = CHIP48;
                    DEBUG("*** -> CHIP48");
                default:
                    machine.I = IADDRESS( 0x50, 10 * ( X & 0xf ) );                                                             // SET ADDRESS TO LARGE FONT CHIP48 / SCHIP / XOCHIP
                    break;
            }
            DEBUG("load_big_font %01x ( %04x )",X&0xf, machine.I);
            break;
        case 0x33:                                                                                                              // STORE BINARY CODED DECIMAL TO I
            machine.MEMORY[ IADDRESS( I, 0 ) ] = ( X > 200 ) ? 2 : ( X > 100 ) ? 1 : 0;                                         // 100s
            machine.MEMORY[ IADDRESS( I, 1 ) ] = ( X - 100 * machine.MEMORY[ IADDRESS( I, 0 ) ] ) / 10;                         // 10s
            machine.MEMORY[ IADDRESS( I, 2 ) ] = ( X - 100 * machine.MEMORY[ IADDRESS( I, 0 ) ] - 10 * machine.MEMORY[ IADDRESS( I, 1 ) ] );
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
                    DEBUG("set_pitch %02x",X);                                                                                  // COCHIP SET AUDIO PITCH TO Vx
                    machine.PITCH = ( X >> 1 );
                    break;
            }
            break;
        case 0x55:
            memcpy( &machine.MEMORY[ I ], machine.V, Xn + 1 );                                                                  // SAVE V0 - Vx TO MEMORY @ I
            switch( machine.MODE ) {
                case CHIP8:
                case XOCHIP:
                    machine.I =  IADDRESS( I, Xn + 1 );                                                                         // CHIP8 / XOCHIP, INCREMENT I BY Xn + 1
                    break;
                case CHIP48:
                    machine.I = IADDRESS( I, Xn );                                                                              // CHIP48 INCREMENT I BY Xn
                    break;
                case SCHIP:
                    break;                                                                                                      // SCHIP DO NOT INCREMENT I
            }
            DEBUG("save V0 -> V%01x @ %04x ( I = %04x )",Xn,I,machine.I);
            break;
        case 0x65:
            memcpy( machine.V, &machine.MEMORY[ I ], Xn + 1 );                                                                  // LOAD V0 - Vx FROM MEMORY @ I
            switch( machine.MODE ) {
                case CHIP8:
                case XOCHIP:
                    machine.I = IADDRESS( I, Xn + 1 );                                                                          // CHIP8 / XOCHIP, INCREMENT I BY Xn + 1
                    break;
                case CHIP48:
                    machine.I = IADDRESS( I, Xn );                                                                              // CHIP48 INCREMENT I BY Xn
                    break;
                case SCHIP:
                    break;                                                                                                      // SCHIP DO NOT INCREMENT I
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
                        machine.MODE = XOCHIP;
                        DEBUG("*** -> XOCHIP ");
                    }
                default:
                    memcpy( machine.FLAGS, machine.V, Xn + 1 );                                                                 // SAVE V0 - Vx TO FLAGS
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
                        machine.MODE = XOCHIP;
                        DEBUG("*** -> XOCHIP ");
                    }
                default:
                    memcpy( machine.V, machine.FLAGS, Xn + 1 );                                                                 // LOAD V0 - Vx FROM FLAGS
                    break;
            }
            break;
        default:
            machine.crashed = INVALIDINSN;
            break;
    }
}

void (*opcodes[])(uint16_t) = {                                                                                                 // JUMP TABLE FOR EACH OF THE OPCODES
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
