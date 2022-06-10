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
//
//-----------------------------------------------------------------------------

#ifndef __M_RANDOM__
#define __M_RANDOM__

#include "doomtype.h"

// Returns a number from 0 to 255,
// uses hardware registers
extern int volatile *TIMER_REGS;
extern int rndindex;

static inline int M_Random (void) {
    unsigned char volatile *TIMER_REGS_B = (unsigned char volatile *)TIMER_REGS;
    return TIMER_REGS_B[ 0x00 ];
}

// As M_Random, but used only by the play simulation.
static inline int P_Random (void){
    unsigned char volatile *TIMER_REGS_B = (unsigned char volatile *)TIMER_REGS;
    rndindex = (rndindex+1)&0xff;
    return TIMER_REGS_B[ 0x02 ];
}

// Fix randoms for demos.
static inline void M_ClearRandom (void) {}

#endif  // __M_RANDOM__
