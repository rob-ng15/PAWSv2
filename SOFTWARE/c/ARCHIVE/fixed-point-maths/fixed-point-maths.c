#include <stdio.h>
#include <string.h>

#include <PAWSintrinsics.h>
#include <PAWSlibrary.h>

//
// Fixed point, 32bit as 16.16.
//
#define FRACBITS                16
#define FRACUNIT                (1<<FRACBITS)

typedef int fixed_t;

fixed_t FixedMul_long (fixed_t a, fixed_t b) {
    return ((long long) a * (long long) b) >> FRACBITS;
}

fixed_t FixedMul_asm (fixed_t a, fixed_t b) {
    fixed_t hi = _rv32_mulh (a, b);
    fixed_t lo = ((unsigned)(a * b)) >> 16;
    return _rv32_pack (lo, hi);
}

int main( void ) {
    fixed_t op1, op2;

    printf("Fixed Point Maths Test\n\n");

    for( int i = 0; i < 16; i ++ ) {
        op1 = rng(0xffff) << 16 + rng(0xffff);
        op2 = rng(0xffff) << 16 + rng(0xffff);

        printf("%08x x %08x = (long)%08x (asm)%08x\n",op1,op2,(int)FixedMul_long(op1,op2),(int)FixedMul_asm(op1,op2));
    }

    sleep1khz(8000,0);
}

// EXIT WILL RETURN TO BIOS
