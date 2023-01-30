#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <PAWSintrinsics.h>
#include <PAWSlibrary.h>

//
// Fixed point, 32bit as 16.16 used in DOOM, using inline asm or accelerator
//
#define FRACBITS                16
#define FRACUNIT                (1<<FRACBITS)
#define MAXINT                  INT_MAX
#define MININT                  INT_MIN

typedef int fixed_t;

fixed_t FixedMul_long (fixed_t a, fixed_t b) {
    return ((long long) a * (long long) b) >> FRACBITS;
}

fixed_t FixedMul_asm (fixed_t a, fixed_t b) {
    fixed_t hi = _rv32_mulh (a, b);
    fixed_t lo = ((unsigned)(a * b)) >> 16;
    return _rv32_pack (lo, hi);
}

fixed_t FixedDiv_long (fixed_t a, fixed_t b) {
    // Check for overflow/underflow.
    if ((abs (a) >> 14) >= abs (b))
        return (a ^ b) < 0 ? MININT : MAXINT;

    return (fixed_t) ((((long long)a) << 16) / ((long long)b));
}

fixed_t FixedDiv_accel (fixed_t a, fixed_t b) {
    // Check for overflow/underflow.
    if ((abs (a) >> 14) >= abs (b))
        return (a ^ b) < 0 ? MININT : MAXINT;

    return (fixed_t)fixed_divide( a, b );
}

int main( void ) {
    fixed_t op1, op2;

    printf("Fixed Point Maths Test\n\n");

    for( int i = 0; i < 16; i ++ ) {
        op1 = rng(0xffff) << 16 + rng(0xffff);
        op2 = rng(0xffff) << 16 + rng(0xffff);

        printf("%08x x %08x = (long)%08x (asm)%08x\n",op1,op2,(int)FixedMul_long(op1,op2),(int)FixedMul_asm(op1,op2));
        printf("%08x / %08x = (long)%08x (acc)%08x\n",op1,op2,(int)FixedDiv_long(op1,op2),(int)FixedDiv_accel(op1,op2));
    }

    printf("\n\n%08x / %08x = (long)%08x (acc)%08x\n",op1,0,(int)FixedDiv_accel(op1,0),(int)FixedDiv_accel(op1,0));

    sleep1khz(8000,0);
}

// EXIT WILL RETURN TO BIOS
