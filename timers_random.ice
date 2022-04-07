// Create seconds and milliseconds since boot
// Create 1hz (1 second counter)
algorithm timesinceboot(
    output  uint16  counter1hz(0),
    output  uint20  counter1mhz(0)
) <autorun,reginputs> {
    uint5  counter25mhz = uninitialised;
    uint1   MIN <:: ( ~|counter25mhz );
    uint1   MAX <:: ( counter1mhz == 1000000 );

    always_after {
        counter25mhz = MIN ? 25 : counter25mhz - 1;
        counter1mhz = MAX ? 0 : counter1mhz + MIN;
        counter1hz = counter1hz + MAX;
    }
}

// Create 1hz (1 second counter)
algorithm pulse1hz(
    output  uint16  counter1hz,
    input   uint1   resetCounter
) <autorun,reginputs> {
    uint25  counter25mhz = uninitialised;
    uint1   MIN <:: ( ~|counter25mhz );

    always_after {
        counter1hz = resetCounter ? 0 : counter1hz + MIN;
        counter25mhz = resetCounter | MIN ? 25000000 : counter25mhz - 1;
    }
}

// Create 1khz (1 milli-second counter)
algorithm pulse1khz(
    output  uint16  counter1khz,
    input   uint16  resetCounter
) <autorun,reginputs> {
    uint15  counter25mhz = uninitialised;
    uint1   MIN <:: ( ~|counter25mhz );
    uint1   RESET <: ( |resetCounter );

    always_after {
        counter1khz = RESET ? resetCounter : ( ~|counter1khz ) ? 0 : counter1khz - MIN;
        counter25mhz = RESET | MIN ? 25000 : counter25mhz - 1;
    }
}

// 16 bit random number generator
// Translation into Silice of LFSR_Plus.v
algorithm random(
    output  uint16  g_noise_out,
    output  uint16  u_noise_out,
) <autorun,reginputs> {
    uint16  rand_out <:: rand_ff;
    uint16  rand_ff = 24b011000110111011010011101;
    uint18  rand_en_ff = 24b001100010011011101100101;
    uint16  temp_u_noise3 <:: { rand_out[15,1], rand_out[15,1], rand_out[2,13] };
    uint16  temp_u_noise2 <:: temp_u_noise3;
    uint16  temp_u_noise1 <:: temp_u_noise2;
    uint16  temp_u_noise0 <:: temp_u_noise1;

    always_after {
        g_noise_out = ( rand_en_ff[17,1] ) ? ( __signed(temp_u_noise3) + __signed(temp_u_noise2) + __signed(temp_u_noise1) + __signed(temp_u_noise0) + ( rand_en_ff[9,1] ? __signed(g_noise_out) : 0 ) ) :
                                             ( rand_en_ff[10,1] ) ? rand_out : g_noise_out;
        u_noise_out = ( rand_en_ff[17,1] ) ? rand_out : u_noise_out;
        rand_en_ff = { ( rand_en_ff[7,1] ^ rand_en_ff[0,1] ), rand_en_ff[1,17]};
        rand_ff = { ( rand_ff[5,1] ^ rand_ff[3,1] ^ rand_ff[2,1] ^ rand_ff[0,1] ), rand_ff[1,15] };
    }
}
