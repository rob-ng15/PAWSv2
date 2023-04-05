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

void b_extension_test( void ) {
    long result;

    printw("\n");
    printw("Bitmanipulation Extension Test");
    printw("\n");

    result = _rv64_adduw(0xfffffffffffffbff,0xfffffffffffffff7); report_test_2long_r1long( "add.uw",0xfffffffffffffbff,0xfffffffffffffff7,result,0xfffffbf6);
}

// https://github.com/riscv-software-src/riscv-tests/blob/master/isa/macros/scalar/test_macros.h
#define qNaN 0x7ff8000000000000
#define sNaN 0x7ff0000000000001

struct singe_inputs {
    int operation;
    union single_bitstream op1;
    union single_bitstream op2;
    union single_bitstream result;
};

int single_tests( void ) {
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

int main( void ) {
    initscr();
    start_color();
    autorefresh( TRUE );
    attron( A_BOLD | COLOR_PAIR(7) );
    printw("PAWS Risc-V Test Suite\n");
    printw("Ported from https://github.com/riscv-software-src/riscv-tests\n");
    attron( A_NORMAL | COLOR_PAIR(7) );

    b_extension_test(); sleep1khz( 4000, 0 );

    single_tests(); sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
