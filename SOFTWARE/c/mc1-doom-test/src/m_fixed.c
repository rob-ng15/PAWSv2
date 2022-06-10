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
