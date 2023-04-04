#include <stdio.h>
#include <math.h>
#include <PAWSlibrary.h>

union single_bitstream {
    float single_float;
    int bitstream;
};

void report_test_2float_r1float( char *operation, float op1, float op2, union single_bitstream result, union single_bitstream expected ) {
    printf("%s %f, %f -> 0x%x ( 0x%x )",operation,op1,op2,result.bitstream,expected.bitstream);
    if( result.bitstream == expected.bitstream ) {
        printf(" PASS\n");
    } else {
        printf(" FAIL\n");
    }
}
void report_test_1float_r1float( char *operation, float op1, union single_bitstream result, union single_bitstream expected ) {
    printf("%s %f -> 0x%x ( 0x%x )",operation,op1,result.bitstream,expected.bitstream);
    if( result.bitstream == expected.bitstream ) {
        printf(" PASS\n");
    } else {
        printf(" FAIL\n");
    }
}
void report_test_1float_r1int( char *operation, float op1, union single_bitstream result, union single_bitstream expected ) {
    printf("%s %f -> 0x%x ( 0x%x )",operation,op1,result.bitstream,expected.bitstream);
    if( result.bitstream == expected.bitstream ) {
        printf(" PASS\n");
    } else {
        printf(" FAIL\n");
    }
}

void report_test_2long_r1long( char *operation, long op1, long op2, long result, long expected ) {
    printf("%s 0x%lx, 0x%lx -> 0x%lx ( 0x%lx )",operation,op1,op2,result,expected);
    if( result == expected ) {
        printf(" PASS\n");
    } else {
        printf(" FAIL\n");
    }
}
void b_extension_test( void ) {
    long result;

    printf("\n");
    printf("Bitmanipulation Extension Test");
    printf("\n");

    result = _rv64_adduw(0xfffffffffffffbff,0xfffffffffffffff7); report_test_2long_r1long( "add.uw",0xfffffffffffffbff,0xfffffffffffffff7,result,0x0000000000000000);
}

// https://github.com/riscv-software-src/riscv-tests/blob/master/isa/macros/scalar/test_macros.h
#define qNaNh 0x7e00
#define sNaNh 0x7c01
#define nINFf 0xFF800000
#define pINFf 0x7F800000
#define qNaNf 0x7fc00000
#define sNaNf 0x7f800001
#define qNaN 0x7ff8000000000000
#define sNaN 0x7ff0000000000001

struct singe_inputs {
    int operation;
    union single_bitstream op1;
    union single_bitstream op2;
    union single_bitstream result;
};

struct singe_inputs single_test_inputs[] = {
    { 1, 2.5f, 1.0f, 3.5f },
    { 1, -1235.1f, 1.1f, -1234.0f },
    { 1, 3.14159265f, 0.00000001f, 3.14159265f },
    { 2, 2.5f, 1.0f, 1.5f },
    { 2, -1235.1f, -1.1f, -1234.0f },
    { 2, 3.14159265f, 0.00000001f, 3.14159265f },
    { 2, pINFf, pINFf, qNaNf },
    { 3, 2.5f, 1.0f, 2.5f },
    { 3, -1235.1f, -1.1f, 1358.61f },
    { 3, 3.14159265f, 0.00000001f, 3.14159265e-8f },
    { 4, 3.14159265f, 2.71828182f, 1.1557273520668288f },
    { 4, -1234.0f, 1235.1f, -0.9991093838555584f },
    { 4, 3.14159265f, 1.0f, 3.14159265f },
    { 5, 3.14159265f, 0, 1.7724538498928541f },
    { 5, 10000.0f, 0, 100.0f },
    { 5,-1.0f, 0, qNaNf },
    { 5, 171.0f, 0, 13.076696f },
    { 6, 0, 0, 0 },
    { 6, 0, 0, 0 },
    { 6, 0, 0, 0 },
    { 6, 0, 0, 0 },
    { 6, 0, 0, 0 },
    { 6, 0, 0, 0 },
    { 6, 0, 0, 0 },
    { 6, 0, 0, 0 },
    { 6, 0, 0, 0 },
    { 6, 0, 0, 0 },
    { -1, -1, -1, -1 }
};
int single_test_inputs_overrides[][3] = {
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {pINFf, pINFf, qNaNf},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,qNaNf},
    {0,0,0},
    {0xff800000,0,1 << 0},
    {0xbf800000,0,1 << 1},
    {0x807fffff,0,1 << 2},
    {0x80000000,0,1 << 3},
    {0,0,1 << 4},
    {0x007fffff,0,1 << 5},
    {0x3f800000,0,1 << 6},
    {0x7f800000,0,1 << 7},
    {0x7f800001,0,1 << 8},
    {0x7fc00000,0,1 << 9},
};


int single_tests( void ) {
    int test = 0;
    union single_bitstream result;

    printf("\n");
    printf("Single-Precision Floating-Point Test");
    printf("\n");

    while( single_test_inputs[ test ].operation != -1 ) {
        if( single_test_inputs_overrides[test][0] ) { single_test_inputs[test].op1.bitstream = single_test_inputs_overrides[test][0]; }
        if( single_test_inputs_overrides[test][1] ) { single_test_inputs[test].op2.bitstream = single_test_inputs_overrides[test][1]; }
        if( single_test_inputs_overrides[test][2] ) { single_test_inputs[test].result.bitstream = single_test_inputs_overrides[test][2]; }
        test++;
    }

    test = 0;
    while( single_test_inputs[ test ].operation != -1 ) {
        switch( single_test_inputs[ test ].operation ) {
            case 1:
                result.single_float = _rv64_fadds(single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float);
                report_test_2float_r1float("fadd.s",single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float,result,single_test_inputs[test].result);
                break;
            case 2:
                result.single_float = _rv64_fsubs(single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float);
                report_test_2float_r1float("fsub.s",single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float,result,single_test_inputs[test].result);
                break;
            case 3:
                result.single_float = _rv64_fmuls(single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float);
                report_test_2float_r1float("fmul.s",single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float,result,single_test_inputs[test].result);
                break;
            case 4:
                result.single_float = _rv64_fdivs(single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float);
                report_test_2float_r1float("fdiv.s",single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float,result,single_test_inputs[test].result);
                break;
            case 5:
                result.single_float = _rv64_fsqrts(single_test_inputs[test].op1.single_float);
                report_test_1float_r1float("fsqrt.s",single_test_inputs[test].op1.single_float,result,single_test_inputs[test].result);
                break;
            case 6:
                result.bitstream = _rv64_fclasss(single_test_inputs[test].op1.single_float);
                report_test_1float_r1int("fclass.s",single_test_inputs[test].op1.single_float,result,single_test_inputs[test].result);
                break;
        }
        sleep1khz( 500, 0 ); test++;
    }
}

int main( void ) {
    // CODE GOES HERE
    printf("PAWS Risc-V Test Suite\n");
    printf("Ported from https://github.com/riscv-software-src/riscv-tests\n");

    b_extension_test(); sleep1khz( 4000, 0 );

    single_tests(); sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
