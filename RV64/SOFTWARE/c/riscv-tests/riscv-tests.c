#include <stdio.h>
#include <math.h>
#include <PAWSlibrary.h>

// https://github.com/riscv-software-src/riscv-tests/blob/master/isa/macros/scalar/test_macros.h
#define qNaNh 0x7e00
#define sNaNh 0x7c01
#define nINFf 0xFF800000
#define pINFf 0x7F800000
#define qNaNf 0x7fc00000
#define sNaNf 0x7f800001
#define qNaN 0x7ff8000000000000
#define sNaN 0x7ff0000000000001

union single_bitstream {
    float single_float;
    int bitstream;
};

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
    { 2, pINFf, nINFf, qNaNf },
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
    { -1, -1, -1, -1 }
};
int single_test_inputs_overrides[][3] = {
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {pINFf, nINFf, qNaNf},
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
    {0,0,0},
    {0,0,0},
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
                result.single_float = single_test_inputs[test].op1.single_float + single_test_inputs[test].op2.single_float;
                printf("fadd.s %8.8f + %8.8f\n",
                        single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float);
                printf("    -> %8.8f ( %8.8f )\n",result.single_float,single_test_inputs[test].result.single_float);
                printf("    -> 0x%8x ( 0x%8x )",result.bitstream,single_test_inputs[test].result.bitstream);
                break;
            case 2:
                result.single_float = single_test_inputs[test].op1.single_float - single_test_inputs[test].op2.single_float;
                printf("fsub.s %8.8f - %8.8f\n",
                        single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float);
                printf("    -> %8.8f ( %8.8f )\n",result.single_float,single_test_inputs[test].result.single_float);
                printf("    -> 0x%8x ( 0x%8x )",result.bitstream,single_test_inputs[test].result.bitstream);
                break;
            case 3:
                result.single_float = single_test_inputs[test].op1.single_float * single_test_inputs[test].op2.single_float;
                printf("fmul.s %8.8f * %8.8f\n",
                        single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float);
                printf("    -> %8.8f ( %8.8f )\n",result.single_float,single_test_inputs[test].result.single_float);
                printf("    -> 0x%8x ( 0x%8x )",result.bitstream,single_test_inputs[test].result.bitstream);
                break;
            case 4:
                result.single_float = single_test_inputs[test].op1.single_float / single_test_inputs[test].op2.single_float;
                printf("fdiv.s %8.8f / %8.8f\n",
                        single_test_inputs[test].op1.single_float,single_test_inputs[test].op2.single_float);
                printf("    -> %8.8f ( %8.8f )\n",result.single_float,single_test_inputs[test].result.single_float);
                printf("    -> 0x%8x ( 0x%8x )",result.bitstream,single_test_inputs[test].result.bitstream);
                break;
            case 5:
                result.single_float = sqrtf(single_test_inputs[test].op1.single_float);
                printf("fsqrt.s %8.8f\n",
                        single_test_inputs[test].op1.single_float);
                printf("    -> %8.8f ( %8.8f )\n",result.single_float,single_test_inputs[test].result.single_float);
                printf("    -> 0x%8x ( 0x%8x )",result.bitstream,single_test_inputs[test].result.bitstream);
                break;
        }
        switch( single_test_inputs[ test ].operation ) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                if( result.single_float == single_test_inputs[test].result.single_float )
                    printf(" PASS\n");
                else
                    printf(" FAIL\n");
                break;
        }
        printf("\n");
        test++;
    }
}

int main( void ) {
    // CODE GOES HERE
    printf("PAWS Risc-V Test Suite\n");
    printf("Ported from https://github.com/riscv-software-src/riscv-tests\n");

    single_tests();

    sleep1khz( 4000, 0 );
}

// EXIT WILL RETURN TO BIOS
