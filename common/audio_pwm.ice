// SL 12-2020
// https://github.com/sylefeb/Silice
// MIT license, see LICENSE_MIT in Silice repo root

// This is a nice trick to 'interpolate' through a 4-bits only DAC
// I got this from emard: https://github.com/emard/ulx3s-misc/blob/master/examples/audio/hdl/dacpwm.v

// For now fixed to 8 bits in to 4 bits DAC
// TODO make it more general

algorithm audio_pwm(
    input   uint8   wave,
    output  uint4   audio,
) <autorun,reginputs> {
    uint4   counter = 0;                            uint4 counterP1 <:: counter + 1;                    uint1 counterLOW <:: ( counter < pwm_threshold );

    uint4   dac_low <:: wave[4,4];                  // tracks higher bits
    uint4   dac_high <:: dac_low + 1;               // same plus on (we interpolate between dac_low and dac_high)
    uint4   pwm_threshold <:: wave[0,4];            // threshold for pwm ratio, using lower bits
                                                    //   threshold == 0 => always low, threshold == 15 almost always high
    always_after {
        if (counterLOW) {
          audio = dac_high;
        } else {
          audio = dac_low;
        }
        counter = counterP1;
    }
}
