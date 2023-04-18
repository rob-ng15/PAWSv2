#include <stdio.h>
#include <math.h>
#include <curses.h>
#include <PAWSlibrary.h>

union single_bitstream {
    float single_float;
    int bitstream;
};

void pass( void ) {
    attron( A_BOLD | COLOR_PAIR(2) ); printw(" PASS\n"); attron( A_NORMAL | COLOR_PAIR(7) );
}

void fail( void ) {
    attron( A_BOLD | COLOR_PAIR(1) ); printw(" FAIL\n"); attron( A_NORMAL | COLOR_PAIR(7) );
}

void report_test_2float_r1float( char *operation, float op1, float op2, union single_bitstream result, union single_bitstream expected ) {
    printw("%s %f, %f -> %f ( 0x%x )",operation,op1,op2,result.single_float,expected.bitstream);
    if( result.bitstream == expected.bitstream ) { pass(); } else { fail(); }
}
void report_test_1float_r1float( char *operation, float op1, union single_bitstream result, union single_bitstream expected ) {
    printw("%s %f -> %f ( 0x%x )",operation,op1,result.single_float,expected.bitstream);
    if( result.bitstream == expected.bitstream ) { pass(); } else { fail(); }
}
void report_test_1float_r1int( char *operation, float op1, union single_bitstream result, union single_bitstream expected ) {
    printw("%s %f -> 0x%x ( 0x%x )",operation,op1,result.bitstream,expected.bitstream);
    if( result.bitstream == expected.bitstream ) { pass(); } else { fail(); }
}
void report_test_2long_r1long( char *operation, long op1, long op2, long result, long expected ) {
    printw("%s 0x%lx, 0x%lx -> 0x%lx ( 0x%lx )",operation,op1,op2,result,expected);
    if( result == expected ) { pass(); } else { fail(); }
}
void report_test_2ulong_r1ulong( char *operation, unsigned long op1, unsigned long op2, unsigned long result, unsigned long expected ) {
    printw("%s 0x%ulx, 0x%ulx -> 0x%ulx ( 0x%ulx )",operation,op1,op2,result,expected);
    if( result == expected ) { pass(); } else { fail(); }
}
void report_test_2int_r1int( char *operation, int op1, int op2, int result, int expected ) {
    printw("%s 0x%x, 0x%x -> 0x%x ( 0x%x )",operation,op1,op2,result,expected);
    if( result == expected ) { pass(); } else { fail(); }
}
void report_test_2uint_r1uint( char *operation, unsigned int op1, unsigned int op2, unsigned int result, unsigned int expected ) {
    printw("%s 0x%ux, 0x%ux -> 0x%ux ( 0x%ux )",operation,op1,op2,result,expected);
    if( result == expected ) { pass(); } else { fail(); }
}

void b_extension_test( void ) {
    long result;

    printw("\n");
    printw("Bitmanipulation Extension Test");
    printw("\n");

    result = _rv64_adduw(0xfffffffffffffbff,0xfffffffffffffff7); report_test_2long_r1long( "add.uw",0xfffffffffffffbff,0xfffffffffffffff7,result,0xfffffbf6);
//    result = _rv64_clmul(0xa,0x6); report_test_2long_r1long( "clmul",0xa,0x6,result,0x3c);
//    result = _rv64_clmul(0x355,0x487); report_test_2long_r1long( "clmul",0x355,0x487,result,0xcf62b);
    result = _rv64_sh1add(0xffffffff12345678,0); report_test_2long_r1long( "sh1add",0xffffffff12345678,0,result,0xfffffffe2468acf0);
    result = _rv64_sh1adduw(0xffffffff12345678,0xffff000000000000); report_test_2long_r1long( "sh1add.uw",0xffffffff12345678,0xffff000000000000,result,0xffff00002468acf0);
    result = _rv64_sh2add(0xffffffff12345678,0); report_test_2long_r1long( "sh2add",0xffffffff12345678,0,result,0xfffffffc48d159e0);
    result = _rv64_sh2adduw(0xffffffff12345678,0xffff000000000000); report_test_2long_r1long( "sh2add.uw",0xffffffff12345678,0xffff000000000000,result,0xffff000048d159e0);
    result = _rv64_sh3add(0xffffffff12345678,0); report_test_2long_r1long( "sh3add",0xffffffff12345678,0,result,0xfffffff891a2b3c0);
    result = _rv64_sh3adduw(0xffffffff12345678,0xffff000000000000); report_test_2long_r1long( "sh3add.uw",0xffffffff12345678,0xffff000000000000,result,0xffff000091a2b3c0);
    result = _rv64_slliuw(0xffffffff12345678,8); report_test_2long_r1long( "slli.uw",0xffffffff12345678,8,result,0x1234567800);
}

void f_extension_test( void ) {
    union single_bitstream result, expected;

    printw("\n");
    printw("Single-Precision Floating-Point Test");
    printw("\n");

    union single_bitstream nINF, pINF, qNAN, sNAN;
    nINF.bitstream = 0xFF800000;
    pINF.bitstream = 0x7F800000;
    qNAN.bitstream = 0x7fc00000;
    sNAN.bitstream = 0x7f800001;

    result.single_float = _rv64_fadds( 2.5f, 1.0f ); expected.single_float = 3.5f; report_test_2float_r1float( "fadd.s", 2.5f, 1.0f, result, expected );
    result.single_float = _rv64_fadds( -1235.1f, 1.1f ); expected.single_float = -1234.0f; report_test_2float_r1float( "fadd.s", -1235.1f, 1.1f , result, expected );
    result.single_float = _rv64_fadds( 3.14159265f, 0.00000001f ); expected.single_float = 3.14159265f; report_test_2float_r1float( "fadd.s", 3.14159265f, 0.00000001f, result, expected );

    result.single_float = _rv64_fsubs( 2.5f, 1.0f ); expected.single_float = 1.5f; report_test_2float_r1float( "fsub.s", 2.5f, 1.0f, result, expected );
    result.single_float = _rv64_fsubs( -1235.1f, -1.1f ); expected.single_float = -1234.0f; report_test_2float_r1float( "fsub.s", -1235.1f, -1.1f, result, expected );
    result.single_float = _rv64_fsubs( 3.14159265f, 0.00000001f ); expected.single_float = 3.14159265f; report_test_2float_r1float( "fsub.s", 3.14159265f, 0.00000001f, result, expected );
    result.single_float = _rv64_fsubs( pINF.single_float, pINF.single_float ); report_test_2float_r1float( "fsub.s", pINF.single_float, pINF.single_float, result, qNAN );

    result.single_float = _rv64_fmuls( 2.5f, 1.0f ); expected.single_float = 2.5f; report_test_2float_r1float( "fmul.s", 2.5f, 1.0f, result, expected );
    result.single_float = _rv64_fmuls( -1235.1f, -1.1f ); expected.single_float = 1358.61f; report_test_2float_r1float( "fmul.s", -1235.1f, -1.1f, result, expected );
    result.single_float = _rv64_fmuls( 3.14159265f, 0.00000001f ); expected.single_float = 3.14159265e-8f; report_test_2float_r1float( "fmul.s", 3.14159265f, 0.00000001f, result, expected );

    result.single_float = _rv64_fdivs( 3.14159265f, 2.71828182f ); expected.single_float = 1.1557273520668288f; report_test_2float_r1float( "fdiv.s", 3.14159265f, 2.71828182, result, expected );
    result.single_float = _rv64_fdivs( -1234.0f, 1235.1f ); expected.single_float = -0.9991093838555584f; report_test_2float_r1float( "fdiv.s", -1234.0f, 1235.1f, result, expected );
    result.single_float = _rv64_fdivs( 3.14159265f, 1.0f ); expected.single_float = 3.14159265f; report_test_2float_r1float( "fdiv.s", 3.14159265f, 1.0f, result, expected );

    result.single_float = _rv64_fsqrts( 3.14159265f ); expected.single_float = 1.7724538498928541f; report_test_1float_r1float( "fsqrt.s", 3.14159265f, result, expected );
    result.single_float = _rv64_fsqrts( 10000.0f ); expected.single_float = 100.0f; report_test_1float_r1float( "fsqrt.s", 10000.0f, result, expected );
    result.single_float = _rv64_fsqrts( -1.0f ); report_test_1float_r1float( "fsqrt.s", -1.0f, result, qNAN );
    result.single_float = _rv64_fsqrts( 171.0f ); expected.single_float = 13.076696f; report_test_1float_r1float( "fsqrt.s", 171.0f, result, expected );
}

void m_extension_test( void ) {
    long result;
    int result_w;

    printw("\n");
    printw("Multiplication / Division Extension Test");
    printw("\n");

    result = _rv64_div(20,6); report_test_2long_r1long( "div",20,6,result,3);
    result = _rv64_div(-20,6); report_test_2long_r1long( "div",-20,6,result,-3);
    result = _rv64_div(20,-6); report_test_2long_r1long( "div",20,-6,result,-3);
    result = _rv64_div(-20,-6); report_test_2long_r1long( "div",-20,-6,result,3);
    result = _rv64_div(-1<<63,1); report_test_2long_r1long( "div",-1<<63,1,result,-1<<63);
    result = _rv64_div(-1<<63,-1); report_test_2long_r1long( "div",-1<<63,-1,result,-1<<63);
    result = _rv64_div(-1<<63,0); report_test_2long_r1long( "div",-1<<63,0,result,-1);
    result = _rv64_div(1,0); report_test_2long_r1long( "div",1,0,result,-1);
    result = _rv64_div(0,0); report_test_2long_r1long( "div",0,0,result,-1);

    result = _rv64_divu(20,6); report_test_2ulong_r1ulong( "divu",20,6,result,3);
    result = _rv64_divu(-20,6); report_test_2ulong_r1ulong( "divu",-20,6,result,3074457345618258599);
    result = _rv64_divu(20,-6); report_test_2ulong_r1ulong( "divu",20,-6,result,0);
    result = _rv64_divu(-20,-6); report_test_2ulong_r1ulong( "divu",-20,-6,result,0);
    result = _rv64_divu(-1<<63,1); report_test_2ulong_r1ulong( "divu",-1<<63,1,result,-1<<63);
    result = _rv64_divu(-1<<63,-1); report_test_2ulong_r1ulong( "divu",-1<<63,-1,result,0);
    result = _rv64_divu(-1<<63,0); report_test_2ulong_r1ulong( "divu",-1<<63,0,result,-1);
    result = _rv64_divu(1,0); report_test_2ulong_r1ulong( "divu",1,0,result,-1);
    result = _rv64_divu(0,0); report_test_2ulong_r1ulong( "divu",0,0,result,-1);
}

int main( void ) {
    initscr();
    start_color();
    autorefresh( TRUE );
    attron( A_BOLD | COLOR_PAIR(7) );
    printw("PAWS Risc-V Test Suite\n");
    printw("Ported from https://github.com/riscv-software-src/riscv-tests\n");
    attron( A_NORMAL | COLOR_PAIR(7) );

    b_extension_test(); sleep1khz( 4000, 0 );
    m_extension_test(); sleep1khz( 4000, 0 );
    f_extension_test(); sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
