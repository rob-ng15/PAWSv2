// OUTPUT KEY PRESS AND KEY RELEASE EVENTS - RETURNS { press, extended keycode, keycode }
// TO OBTAIN ASCII, NEEDS DECODING IN THE LIBRARY
algorithm ps2ascii(
    input   uint1   us2_bd_dp,
    input   uint1   us2_bd_dn,
    input   uint1   outputkeycodes,
    output  uint10  keycode,
    output  uint1   keycodevalid,
    output  uint16  joystick
) <autorun> {
    // MODIFIER KEYS + JOYSTICK MODE
    uint1   lctrl = 0;                              uint1   rctrl = 0;
    uint1   lalt = 0;                               uint1   ralt = 0;
    uint1   lwin = 0;                               uint1   rwin = 0;
    uint1   application = 0;
    uint1   left = 0;                               uint1   right = 0;
    uint1   up = 0;                                 uint1   down = 0;
    uint1   npleft = 0;                             uint1   npright = 0;
    uint1   npup = 0;                               uint1   npdown = 0;
    uint1   npleftup = 0;                           uint1   npleftdown = 0;
    uint1   nprightup = 0;                          uint1   nprightdown = 0;

    uint1   startbreak = 0;                         uint1   startmulti = 0;

    // PS2 KEYBOARD CODE READER
    ps2 PS2( ps2clk_ext <: us2_bd_dp, ps2data_ext <: us2_bd_dn );

    uint10  newkeycode = uninitialised;

    newkeycode := 0; keycodevalid := 0;
    joystick := { application, nprightup, nprightdown, npleftdown, npleftup, rctrl, rwin, ralt, lalt, npright | right, npleft | left, npdown | down, npup | up, lwin, lctrl, 1b0 };

    always_after {
        if( PS2.valid ) {
            switch( PS2.data ) {
                case 8he0: { startmulti = 1; }
                case 8hf0: { startbreak = 1; }
                default: {
                    switch( { startmulti, startbreak } ) {
                        case 2b00: {
                            // KEY PRESS - SINGLE
                            switch( PS2.data ) {
                                case 8h14: { lctrl = 1; }
                                case 8h11: { lalt = 1; }
                                case 8h69: { npleftdown = 1; }
                                case 8h72: { npdown = 1; }
                                case 8h7a: { nprightdown = 1; }
                                case 8h6b: { npleft = 1; }
                                case 8h74: { npright = 1; }
                                case 8h6c: { npleftup = 1; }
                                case 8h75: { npup = 1; }
                                case 8h7d: { nprightup = 1; }
                                default: {}
                            }
                            newkeycode = { 2b10, PS2.data };
                        }
                        case 2b01: {
                            // KEY RELEASE - SINGLE
                            switch( PS2.data ) {
                                case 8h14: { lctrl = 0; }
                                case 8h69: { npleftdown = 0; }
                                case 8h72: { npdown = 0; }
                                case 8h7a: { nprightdown = 0; }
                                case 8h6b: { npleft = 0; }
                                case 8h74: { npright = 0; }
                                case 8h6c: { npleftup = 0; }
                                case 8h75: { npup = 0; }
                                case 8h7d: { nprightup = 0; }
                                default: {}
                            }
                            newkeycode = { 2b00, PS2.data }; startbreak = 0;
                        }
                        case 2b10: {
                            // MULTICODE KEY PRESS
                            switch( PS2.data ) {
                                case 8h14: { rctrl = 1; }
                                case 8h11: { ralt = 1; }
                                case 8h1f: { lwin = 1; }
                                case 8h27: { rwin = 1; }
                                case 8h2f: { application = 1; }
                                case 8h6b: { left = 1; }
                                case 8h75: { up = 1; }
                                case 8h72: { down = 1; }
                                case 8h74: { right = 1; }
                                default: {}
                            }
                            newkeycode = { 2b11, PS2.data }; startmulti = 0;
                        }
                        case 2b11: {
                            // MULTICODE KEY RELEASE
                            switch( PS2.data ) {
                                case 8h6b: { left = 0; }
                                case 8h75: { up = 0; }
                                case 8h72: { down = 0; }
                                case 8h74: { right = 0; }
                                case 8h14: { rctrl = 0; }
                                case 8h11: { ralt = 0; }
                                case 8h1f: { lwin = 0; }
                                case 8h27: { rwin = 0; }
                                case 8h2f: { application = 0; }
                                default: {}
                            }
                            newkeycode = { 2b01, PS2.data }; startmulti = 0; startbreak = 0;
                        }
                    }
                }
            }
        }
        // NEW KEYCODE RECEIVED
        if( |newkeycode ) {
            keycode = newkeycode; keycodevalid = outputkeycodes;
        }
    }
}

// PS/2 PORT - READS KEYCODE FROM PS/2 KEYBOARD
// MODIFIED FROM ORIGINAL CODE https://github.com/hoglet67/Ice40Beeb converted to Silice by @lawrie, optimised and simplified by @rob-ng15
//  ZX Spectrum for Altera DE1
//
//  Copyright (c) 2009-2011 Mike Stirling
//
//  All rights reserved
//
//  Redistribution and use in source and synthezised forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
//  * Redistributions in synthesized form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//  * Neither the name of the author nor the names of other contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written agreement from the author.
//
//  * License is granted for non-commercial use only.  A fee may not be charged
//    for redistributions as source code or in synthesized/hardware form without
//    specific prior written agreement from the author.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
//  PS/2 interface (input only)
//  Based loosely on ps2_ctrl.vhd (c) ALSE. http://www.alse-fr.com

algorithm ps2(
    input   uint1   ps2data_ext,
    input   uint1   ps2clk_ext,
    output  uint1   valid,
    output  uint1   error,
    output  uint8   data
) < autorun> {
    uint4 clk_filter = 4b1111;                      uint1 ps2_clk_in = 1;                               uint1 clk_edge = 0;
    uint4 bit_count = 0;                            uint9 shift_reg = 0;                                uint1 parity = 0;

    valid := 0;                                     error := 0;                                         clk_edge := 0;

    always_after {
        // Filter the PS/2 clock
        clk_filter = { ps2clk_ext, clk_filter[1,3] };
        switch( clk_filter ) {
            case 4b1100: { ps2_clk_in = 1; }
            case 4b0011: {
                if( ps2_clk_in ) {
                    clk_edge = 1;
                }
                ps2_clk_in = 0;
            }
            default: {}
        }

        // Process the PS/2 data bit
        if( clk_edge ) {
            switch( bit_count ) {
                case 0: {
                    parity = 0;
                    //bit_count = bit_count + ( ~ps2data_ext );
                    if( ~ps2data_ext ) {
                        // Start bit
                        bit_count = bit_count + 1;
                    }
                }
                default: {
                    bit_count = bit_count + 1;
                    shift_reg = { ps2data_ext, shift_reg[1,8] };
                    parity = parity ^ ps2data_ext;
                }
                case 10: {
                    if( ps2data_ext ) {
                        if( parity ) {
                            data = shift_reg[0,8];
                            valid = 1;
                        } else {
                            error = 1;
                        }
                        bit_count = 0;
                    }
                }
            }
        }
    }
}
