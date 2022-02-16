#include <PAWSlibrary.h>
#include <stdio.h>

// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Fixed point arithemtics, implementation.
//
//-----------------------------------------------------------------------------

//
// Fixed point, 32bit as 16.16.
//
#define FRACBITS                16
#define FRACUNIT                (1<<FRACBITS)

typedef int fixed_t;

//
// FixedMul - Fixed point multiplication.
// We inline this function for better performance.
//

static inline fixed_t FixedMul (fixed_t a, fixed_t b)
{
    return ((long long) a * (long long) b) >> FRACBITS;
}

static inline fixed_t FixedMul_ASM (fixed_t a, fixed_t b)
{
//  Risc-V Assembler Code for fixed_t multiply
//  Gives graphical glitches
    fixed_t hi, lo;
    asm volatile (
        "mul    %[lo],%[a],%[b]    \n"  // GET LOWER 32 BITS OF a*b
        "mulh   %[hi],%[a],%[b]    \n"  // GET UPPER 32 BITS OF a*b
        "bgez   %[hi],1f           \n"  // IS RESULT NEGATIVE?
        "sub    %[lo],x0,%[lo]     \n"  // YES, ABS OF FRACTIONAL PART
        "1:                        \n"
        "slli   %[hi],%[hi],16     \n"  // ALIGN UPPER PART TO LEFT
        "srli   %[lo],%[lo],16     \n"  // ALIGN LOWER PART TO RIGHT
        "or     %[hi],%[hi],%[lo]  \n"  // OR IN THE FRACTIONAL BITS
        : [hi] "=r" (hi),
          [lo] "=r" (lo)
        : [a]  "r"  (a),
          [b]  "r"  (b)
    );
    return( hi );
}

int main( void ) {
    fixed_t a, b;

    // GUESS AT 16.5 * 16.5
    a = 0x00108000; b = 0x00108000;
    printf("Using long a * b = %08x * %08x = %08x\n",a,b,FixedMul(a,b));
    printf("Using asm a * b = %08x * %08x = %08x\n\n",a,b,FixedMul_ASM(a,b));

    // GUESS AT -1 * 16.5
    a = 0xffff0000; b = 0x00108000;
    printf("Using long a * b = %08x * %08x = %08x\n",a,b,FixedMul(a,b));
    printf("Using asm a * b = %08x * %08x = %08x\n\n",a,b,FixedMul_ASM(a,b));

    // GUESS AT -1.5 * 16.5
    a = 0xffff8000; b = 0x00108000;
    printf("Using long a * b = %08x * %08x = %08x\n",a,b,FixedMul(a,b));
    printf("Using asm a * b = %08x * %08x = %08x\n\n",a,b,FixedMul_ASM(a,b));

    sleep1khz( 8000, 0 );
}

// EXIT WILL RETURN TO BIOS
