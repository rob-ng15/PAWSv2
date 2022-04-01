// Runs at 25MHz
algorithm apu(
    input   uint3   waveform,
    input   uint12  frequency,
    input   uint16  duration,
    input   uint1   apu_write,
    input   uint8   staticGenerator,
    output  uint1   audio_active,
    output  uint8   audio_output,
    output  uint8   point,
    input   uint8   sine
) <autorun,reginputs> {
    waveform WAVEFORM( start <: apu_write, update <: COUNTER.updatepoint, selected_waveform <: waveform, staticGenerator <: staticGenerator, point :> point, sine <: sine );
    audiocounter COUNTER( selected_frequency <: frequency, selected_duration <: duration, start <: apu_write, active :> audio_active );

    always_after {
        audio_output = audio_active ? COUNTER.updatepoint ? WAVEFORM.audio_output : audio_output : 0;
    }
}

algorithm waveform(
    input   uint1   start,
    input   uint1   update,
    input   uint3   selected_waveform,
    input   uint8   staticGenerator,
    output  uint8   audio_output,
    output  uint8   point,
    input   uint8   sine
) <autorun,reginputs> {
    always_after  {
        point = start ? 0 : point + update;
        switch( selected_waveform ) {
            case 0: { audio_output = { point[7,1], 7b1111111 }; }                                   // SQUARE
            case 1: { audio_output = point; }                                                       // SAWTOOTH
            case 2: { audio_output = point[7,1] ? { point[0,7], 1b0 } : ~{ point[0,7], 1b0 }; }     // TRIANGLE
            case 3: { audio_output = sine; }                                                        // SINE
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
