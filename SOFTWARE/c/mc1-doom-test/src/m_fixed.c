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
//      Fixed point implementation.
//
//-----------------------------------------------------------------------------

#include <stdlib.h>

#include "doomtype.h"

#include "m_fixed.h"

#include <PAWSlibrary.h>

//
// FixedDiv
//

// FIXED POINT DIVISION ACCELERATOR
fixed_t FixedDiv (fixed_t a, fixed_t b)
{
    unsigned char volatile *FIXED_REGS_B = (unsigned char volatile *)FIXED_REGS;                 // BYTE ACCESS TO START / STATUS REGISTER

    // Check for overflow/underflow.
    if ((abs (a) >> 14) >= abs (b))
        return (a ^ b) < 0 ? MININT : MAXINT;

    FIXED_REGS[0] = a; FIXED_REGS[1] = b; FIXED_REGS_B[0x08] = 1; while( FIXED_REGS_B[0x08] );
    return( FIXED_REGS[0] );
}

// 2 PARALLEL FIXED POINT DIVISION ACCELERATORS
void P_FixedDiv (fixed_t a1, fixed_t b1, fixed_t *r1, fixed_t a2, fixed_t b2, fixed_t *r2 ) {
    unsigned char volatile *FIXED_REGS_B = (unsigned char volatile *)FIXED_REGS;                 // BYTE ACCESS TO START / STATUS REGISTERS

    FIXED_REGS[0] = a1; FIXED_REGS[1] = b1; FIXED_REGS_B[0x08] = 1;
    FIXED_REGS[4] = a2; FIXED_REGS[5] = b1; FIXED_REGS_B[0x18] = 1;
    while( FIXED_REGS_B[0x08] | FIXED_REGS_B[0x18] );

    if ((abs (a1) >> 14) >= abs (b1)) { *r1 = (a1 ^ b1) < 0 ? MININT : MAXINT; } else { *r1 = FIXED_REGS[0]; }
    if ((abs (a2) >> 14) >= abs (b2)) { *r2 = (a2 ^ b2) < 0 ? MININT : MAXINT; } else { *r1 = FIXED_REGS[4]; }
}
