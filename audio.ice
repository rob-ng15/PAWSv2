// Runs at 25MHz
algorithm apu(
    input   uint3   waveform,
    input   uint6   frequency,
    input   uint16  duration,
    input   uint1   apu_write,
    input   uint8   staticGenerator,
    output  uint1   audio_active,
    output  uint8   audio_output
) <autorun,reginputs> {
    brom uint12 frequency_table[64] = {
        0,
        2986, 2819, 2660, 2511, 2314, 2237, 2112, 1993, 1881, 1776, 1676, 1582,     // 1 = C 2 or Deep C
        1493, 1409, 1330, 1256, 1185, 1119, 1056, 996, 941, 888, 838, 791,          // 13 = C 3
        747, 705, 665, 628, 593, 559, 528, 498, 470, 444, 419, 395,                 // 25 = C 4 or Middle C
        373, 352, 333, 314, 296, 280, 264, 249, 235, 222, 209, 198,                 // 37 = C 5 or Tenor C
        187, 176, 166, 157, 148, 140, 132, 125, 118, 111, 105, 99,                  // 49 = C 6 or Soprano C
        93, 88, 83                                                                  // 61 = C 7 or Double High C
    };

    waveform WAVEFORM( staticGenerator <: staticGenerator );
    audiocounter COUNTER( active :> audio_active );

    frequency_table.addr := frequency;              COUNTER.start := 0;                                 audio_output := audio_active ? COUNTER.updatepoint ? WAVEFORM.audio_output : audio_output : 0;

    always_after {
        if( apu_write ) {
            WAVEFORM.point = 0;
            WAVEFORM.selected_waveform = waveform;
            COUNTER.selected_frequency = frequency_table.rdata;
            COUNTER.selected_duration = duration;
            COUNTER.start = 1;
        } else {
            WAVEFORM.point = WAVEFORM.point + COUNTER.updatepoint;
        }
    }
}

algorithm waveform(
    input   uint8   point,
    input   uint3   selected_waveform,
    input   uint8   staticGenerator,
    output  uint8   audio_output
) <autorun,reginputs> {
    brom uint8 sine[256] = {
        128,131,134,137,140,144,147,150,153,156,159,162,165,168,171,174,
        177,179,182,185,188,191,193,196,199,201,204,206,209,211,213,216,
        218,220,222,224,226,228,230,232,234,235,237,239,240,241,243,244,
        245,246,248,249,250,250,251,252,253,253,254,254,254,255,255,255,
        255,255,255,255,254,254,254,253,253,252,251,250,250,249,248,246,
        245,244,243,241,240,239,237,235,234,232,230,228,226,224,222,220,
        218,216,213,211,209,206,204,201,199,196,193,191,188,185,182,179,
        177,174,171,168,165,162,159,156,153,150,147,144,140,137,134,131,
        128,125,122,119,116,112,109,106,103,100,97,94,91,88,85,82,
        79,77,74,71,68,65,63,60,57,55,52,50,47,45,43,40,
        38,36,34,32,30,28,26,24,22,21,19,17,16,15,13,12,
        11,10,8,7,6,6,5,4,3,3,2,2,2,1,1,1,
        1,1,1,1,2,2,2,3,3,4,5,6,6,7,8,10,
        11,12,13,15,16,17,19,21,22,24,26,28,30,32,34,36,
        38,40,43,45,47,50,52,55,57,60,63,65,68,71,74,77,
        79,82,85,88,91,94,97,100,103,106,109,112,116,119,122,125
    };
    sine.addr := point;

    always_after  {
        switch( selected_waveform ) {
            case 0: { audio_output = { point[7,1], 7b1111111 }; }                                   // SQUARE
            case 1: { audio_output = point; }                                                       // SAWTOOTH
            case 2: { audio_output = point[7,1] ? { point[0,7], 1b0 } : ~{ point[0,7], 1b0 }; }     // TRIANGLE
            case 3: { audio_output = sine.rdata; }                                                  // SINE
            default: { audio_output = staticGenerator; }                                            // NOISE
        }
    }
}

algorithm audiocounter(
    input   uint1   start,
    input   uint12  selected_frequency,
    input   uint16  selected_duration,
    output  uint1   updatepoint,
    output  uint1   active(0)
) <autorun,reginputs> {
    uint12  counter25mhz = uninitialised;           uint16  counter1khz = uninitialised;                uint16  duration = uninitialised;
    uint1   updateduration <:: active & ( ~|counter1khz );

    active := ( |duration ); updatepoint := active & ( ~|counter25mhz );

    always_after {
        if( start ) {
            counter25mhz = 0;
            counter1khz = 25000;
            duration = selected_duration;
        } else {
            counter25mhz = updatepoint ? selected_frequency : counter25mhz - 1;
            counter1khz = updateduration ? 25000 : counter1khz - 1;
            duration = duration - updateduration;
        }
    }
}
