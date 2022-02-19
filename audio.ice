// Runs at 25MHz
algorithm apu(
    input   uint3   waveform,
    input   uint6   frequency,
    input   uint16  duration,
    input   uint1   apu_write,
    input   uint4   staticGenerator,
    output  uint1   audio_active,
    output  uint4   audio_output
) <autorun,reginputs> {
    brom uint16 frequency_table[64] = {
        0,
        23889, 22548, 21283, 20088, 18511, 17897, 16892, 15944, 15049, 14205, 13407, 12655,     // 1 = C 2 or Deep C
        11945, 11274, 10642, 10044, 9481, 8949, 8446, 7972, 7525, 7102, 6704, 6327,             // 13 = C 3
        5972, 5637, 5321, 5022, 4740, 4474, 4223, 3986, 3762, 3551, 3352, 3164,                 // 25 = C 4 or Middle C
        2986, 2819, 2660, 2511, 2370, 2237, 2112, 1993, 1881, 1776, 1676, 1582,                 // 37 = C 5 or Tenor C
        1493, 1409, 1330, 1256, 1185, 1119, 1056, 996, 941, 888, 838, 791,                      // 49 = C 6 or Soprano C
        747, 705, 665                                                                           // 61 = C 7 or Double High C
    };

    waveform WAVEFORM( staticGenerator <: staticGenerator );
    audiocounter COUNTER( active :> audio_active );

    frequency_table.addr := frequency; COUNTER.start := 0;

    always_before {
        if( audio_active ) {
            if( COUNTER.updatepoint ) { audio_output = WAVEFORM.audio_output; }
        } else {
            audio_output = 0;
        }
    }
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
    input   uint5   point,
    input   uint3   selected_waveform,
    input   uint4   staticGenerator,
    output  uint4   audio_output
) <autorun,reginputs> {
    brom uint4 level[] = {
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,     // SQUARE WAVE ( 0 )
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

        1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7,                     // SAWTOOTH WAVE ( 1 )
        8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,

        1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,               // TRIANGLE WAVE ( 2 )
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 1,

        8, 9, 11, 12, 13, 14, 14, 15, 15, 15, 14, 14, 13, 12, 11, 9,        // SINE WAVE ( 3 )
        8, 7, 5, 4, 3, 2, 2, 1, 1, 1, 2, 2, 3, 4, 5, 7
    };
    level.addr := { selected_waveform[0,2], point };
    audio_output :=  selected_waveform[2,1] ? staticGenerator : level.rdata;
}

algorithm audiocounter(
    input   uint1   start,
    input   uint16  selected_frequency,
    input   uint16  selected_duration,
    output  uint1   updatepoint,
    output  uint1   active(0)
) <autorun,reginputs> {
    uint16  counter25mhz = uninitialised;           uint16  counter1khz = uninitialised;                uint16  duration = uninitialised;
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
