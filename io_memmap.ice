algorithm io_memmap(
    // LEDS (8 of)
    output  uint8   leds,

$$if not SIMULATION then
    input   uint$NUM_BTNS$ btns,

    // GPIO
    input   uint28  gn,
    output  uint28  gp,

    // UART
    output  uint1   uart_tx,
    input   uint1   uart_rx,

    // USB for PS/2
    input   uint1   us2_bd_dp,
    input   uint1   us2_bd_dn,

    // SDCARD
    output  uint1   sd_clk,
    output  uint1   sd_mosi,
    output  uint1   sd_csn,
    input   uint1   sd_miso,
$$end

    // CLOCKS
    input   uint1   clock_25mhz,

    // Memory access
    input   uint12  memoryAddress,
    input   uint1   memoryWrite,
    input   uint1   memoryRead,

    input   uint16  writeData,
    output  uint16  readData,

    // SMT STATUS
    output  uint1   SMTRUNNING(0),
    output  uint32  SMTSTARTPC(0),

    // MINI DMA CONTROLLER
    output  uint32  DMASOURCE,
    output  uint32  DMADEST,
    output  uint32  DMACOUNT,
    output  uint3   DMAMODE(0),
    input   uint1   DMAACTIVE
) <autorun,reginputs> {
$$if not SIMULATION then
    // UART CONTROLLER, CONTAINS BUFFERS FOR INPUT/OUTPUT
    uint2   UARTinread = 0;                                                                                                         // 2 BIT LATCH ( bit 0 is the signal ) due to clock boundary change
    uint2   UARToutwrite = 0;                                                                                                       // 2 BIT LATCH ( bit 0 is the signal )
    uart_IN UART_IN <@clock_25mhz> ( uart_rx <: uart_rx, inread <: UARTinread[0,1] );
    uart_OUT UART_OUT <@clock_25mhz> ( uart_tx :> uart_tx, outwrite <: UARToutwrite[0,1] );

    // PS2 CONTROLLER, CONTAINS BUFFERS FOR INPUT/OUTPUT
    uint2   PS2inread = 0;                                                                                                          // 2 BIT LATCH ( bit 0 is the signal )
    ps2buffer PS2 <@clock_25mhz> ( us2_bd_dp <: us2_bd_dp, us2_bd_dn <: us2_bd_dn, inread <: PS2inread[0,1] );

    // SDCARD AND BUFFER
    simple_dualport_bram uint8 buffer_in[512] = uninitialized;                                          bufferaddrplus1 INPLUS1( address <: buffer_in.addr0 );      // READ FROM SDCARD
    simple_dualport_bram uint8 buffer_out[512] = uninitialized;                                         bufferaddrplus1 OUTPLUS1( address <: buffer_out.addr1 );    // WRITE TO SDCARD

    uint1   SDCARDreadsector = uninitialized;       uint1   SDCARDwritesector = uninitialized;
    uint32  SDCARDsectoraddress = uninitialized;
    sdcardcontroller SDCARD(
        sd_clk :> sd_clk,
        sd_mosi :> sd_mosi,
        sd_csn :> sd_csn,
        sd_miso <: sd_miso,
        readsector <: SDCARDreadsector,
        writesector <: SDCARDwritesector,
        sectoraddress <: SDCARDsectoraddress,
        buffer_in <:> buffer_in,
        buffer_out <:> buffer_out
    );
$$end
    // A READABLE ADDRESS AT ffee
    uint8   DMASET = uninitialized;

$$if not SIMULATION then
    // I/O FLAGS
    SDCARDreadsector := 0; SDCARDwritesector := 0; buffer_out.wenable1 := 0;
$$end

     always_before {
$$if not SIMULATION then
        // UPDATE LATCHES
        UARTinread = UARTinread[1,1]; UARToutwrite = UARToutwrite[1,1]; PS2inread = PS2inread[1,1];
$$end

        // READ IO Memory
        if( memoryRead ) {
            switch( memoryAddress[4,4] ) {
                $$if not SIMULATION then
                case 4h0: {
                    if( memoryAddress[1,1] ) {
                        readData = { 14b0, UART_OUT.outfull, UART_IN.inavailable };
                    } else {
                        readData = { 8b0, UART_IN.inchar }; UARTinread = 2b11;
                    }
                }
                case 4h1: {
                    if( memoryAddress[1,1] ) {
                        if( PS2.inavailable ) {
                            readData = PS2.inchar; PS2inread = 2b11;
                        } else {
                            readData = 0;
                        }
                    } else {
                        readData = PS2.inavailable;
                    }
                }
                case 4h2: { readData = PS2.outputascii ? { $16-NUM_BTNS$b0, btns[0,$NUM_BTNS$] } : { $16-NUM_BTNS$b0, btns[0,$NUM_BTNS$] } | PS2.joystick; }
                case 4h4: { readData = SDCARD.ready; }
                case 4h5: { readData = buffer_in.rdata0; buffer_in.addr0 = INPLUS1.addressplus1; }
                $$end
                case 4h3: { readData = leds; }
                case 4he: { readData = DMASET; }
                case 4hf: { readData = SMTRUNNING; }
                default: { readData = 0;}
            }
        }
    }
    always_after {
        // WRITE IO Memory
        if( memoryWrite ) {
            switch( memoryAddress[4,4] ) {
                $$if not SIMULATION then
                case 4h0: { UART_OUT.outchar = writeData[0,8]; UARToutwrite = 2b11; }
                case 4h1: { PS2.outputascii = writeData; }
                case 4h4: {
                    switch( memoryAddress[1,2] ) {
                        case 2h0: { SDCARDreadsector = 1; }
                        case 2h1: { SDCARDwritesector = 1; }
                        default: { SDCARDsectoraddress[ { memoryAddress[1,1], 4b0000 }, 16 ] = writeData; }
                    }
                }
                case 4h5: {
                    switch( memoryAddress[1,1] ) {
                        case 0: { buffer_in.addr0 = 0; buffer_out.addr1 = 511; }
                        case 1: { buffer_out.addr1 = OUTPLUS1.addressplus1; buffer_out.wdata1 = writeData; buffer_out.wenable1 = 1; }
                    }
                }
                $$end
                case 4h3: { leds = writeData; }
                case 4he: {
                    switch( memoryAddress[2,2] ) {
                        case 2b00: { DMASOURCE[ { memoryAddress[1,1], 4b0000 }, 16 ] = writeData; }
                        case 2b01: { DMADEST[ { memoryAddress[1,1], 4b0000 }, 16 ] = writeData; }
                        case 2b10: { DMACOUNT[ { memoryAddress[1,1], 4b0000 }, 16 ] = writeData; }
                        case 2b11: { if( memoryAddress[1,1] ) { DMASET = writeData; } else { DMAMODE = writeData; } }
                    }
                }
                case 4hf: {
                    switch( memoryAddress[2,1] ) {
                        case 1b0: { SMTSTARTPC[ { memoryAddress[1,1], 4b0000 }, 16 ] = writeData; }
                        case 1b1: { SMTRUNNING = writeData; }
                    }
                }
                default: {}
            }
        }
        if( DMAACTIVE ) { DMAMODE = 0; }
    }

    // DISBLE SMT ON STARTUP, KEYBOARD DEFAULTS TO JOYSTICK MODE
    if( ~reset ) {
        SMTRUNNING = 0;

        $$if not SIMULATION then
        PS2.outputascii = 0;
        $$end
    }

}

algorithm timers_memmap(
    // CLOCKS
    input   uint1   clock_25mhz,

    // Memory access
    input   uint5   memoryAddress,
    input   uint1   memoryWrite,
    input   uint1   memoryRead,
    input   uint16  writeData,
    output  uint16  readData,

    // RNG + CURSOR BLINK
    output  uint16  static16bit,
    output  uint1   cursor
) <autorun,reginputs> {
    // TIMERS and RNG
    timers_rng timers <@clock_25mhz> ( seconds :> cursor, g_noise_out :> static16bit );
    uint3   timerreset <:: memoryAddress[1,3] + 1;
    uint32  floatrng <:: { 1b0, 5b01111, &timers.u_noise_out[0,3] ? 3b110 : timers.u_noise_out[0,3], timers.g_noise_out[0,16], timers.u_noise_out[3,7] };

    // LATCH MEMORYWRITE
    uint1   LATCHmemoryWrite = uninitialized;

    always_before {
        // READ IO Memory
        if( memoryRead ) {
            switch( memoryAddress[1,4] ) {
                // RNG ( 2 interger, 1 float 0 <= fng < 1 ) and TIMERS
                case 4h0: { readData = timers.g_noise_out; }
                case 4h1: { readData = timers.u_noise_out; }
                case 4h2: { readData = floatrng[0,16]; }
                case 4h3: { readData = floatrng[16,16]; }
                case 4h4: { readData = timers.seconds; }
                case 4h5: { readData = timers.milliseconds[0,16]; }
                case 4h6: { readData = timers.milliseconds[16,9]; }
                case 4h8: { readData = timers.timer1hz0; }
                case 4h9: { readData = timers.timer1hz1; }
                case 4ha: { readData = timers.timer1khz0; }
                case 4hb: { readData = timers.timer1khz1; }
                case 4hc: { readData = timers.sleepTimer0; }
                case 4hd: { readData = timers.sleepTimer1; }
                // RETURN NULL VALUE
                default: { readData = 0; }
            }
        }
    }
    always_after {
        // WRITE IO Memory
        switch( { memoryWrite, LATCHmemoryWrite } ) {
            case 2b10: { timers.counter = writeData; timers.resetcounter = timerreset; }
            case 2b00: { timers.resetcounter = 0; }
            default: {}
        }
        LATCHmemoryWrite = memoryWrite;
    }
}

algorithm audio_memmap(
    // CLOCKS
    input   uint1   clock_25mhz,

    // Memory access
    input   uint4   memoryAddress,
    input   uint1   memoryWrite,
    input   uint1   memoryRead,
    input   uint16  writeData,
    output  uint1   readData,

    // AUDIO
    output  uint4   audio_l,
    output  uint4   audio_r,

    // RNG
    input  uint8   static8bit
) <autorun,reginputs> {
    // BLOCK STORAGE FOR "SAMPLES" - A SERIES OF NOTES TO BE PLAYED FOR A GIVEN NUMBER OF MILLISECONDS
    simple_dualport_bram uint6 samples_left[1024] = uninitialized;
    simple_dualport_bram uint6 samples_right[1024] = uninitialized;

    // POINTERS WITHIN THE BUFFERS
    uint11  MAXSAMPLES[2] = uninitialised;
    uint11  MAXS0P1 <:: MAXSAMPLES[0] + 1;          uint11  MAXS1P1 <:: MAXSAMPLES[1] + 1;

    // Left and Right audio channels
    audio apu_processor <@clock_25mhz> ( samples_left <:> samples_left, samples_right <:> samples_right, staticGenerator <: static8bit, audio_l :> audio_l, audio_r :> audio_r, samples_MAX_L <: MAXSAMPLES[0], samples_MAX_R <: MAXSAMPLES[1] );

    // LATCH MEMORYWRITE
    uint1   LATCHmemoryWrite = uninitialized;

    // SAMPLE MEMORY CONTROLS - SAMPLES TO BE WRITTEN ADJUSTED FROM 1 to 127 to 1 to 63
    uint7   writeSAMPLE <:: ( writeData == 1 ) ? 1 : writeData[ 1, 6 ];
    samples_left.addr1 := MAXSAMPLES[0]; samples_left.wdata1 := writeSAMPLE;                  samples_left.wenable1 := 0;
    samples_right.addr1 := MAXSAMPLES[1]; samples_right.wdata1 := writeSAMPLE;                samples_right.wenable1 := 0;

    always_before {
        // READ IO Memory
        if( memoryRead ) { readData = memoryAddress[1,1] ? apu_processor.audio_active_r : apu_processor.audio_active_l; }
    }
    always_after {
        // WRITE IO Memory
        switch( { memoryWrite, LATCHmemoryWrite } ) {
            case 2b10: {
                if( memoryAddress[3,1] ) {                                                                              // HANDLE WRITING OF SAMPLES TO MEMORY
                    switch( memoryAddress[0,2] ) {
                        default: { if( writeData[0,1] ) { MAXSAMPLES[0] = 0; }  if( writeData[1,1] ) { MAXSAMPLES[1] = 0; } }
                        case 1: { samples_left.wenable1 = 1; MAXSAMPLES[0] = MAXS0P1; }
                        case 2: { samples_right.wenable1 = 1; MAXSAMPLES[1] = MAXS1P1; }
                    }
                } else {
                    switch( memoryAddress[1,2] ) {
                        case 2h0: { apu_processor.waveform = writeData; }
                        case 2h1: { apu_processor.frequency = writeData; }
                        case 2h2: { apu_processor.duration = writeData; }
                        case 2h3: { apu_processor.apu_write = writeData; }
                    }
                }
            }
            case 2b00: {
                apu_processor.apu_write = 0;
            }
            default: {}
        }
        LATCHmemoryWrite = memoryWrite;
    }
}

// TIMERS and RNG Controllers
algorithm timers_rng(
    output  uint16  seconds,
    output  uint25  milliseconds,
    output  uint16  timer1hz0,
    output  uint16  timer1hz1,
    output  uint16  timer1khz0,
    output  uint16  timer1khz1,
    output  uint16  sleepTimer0,
    output  uint16  sleepTimer1,
    output  uint16  u_noise_out,
    output  uint16  g_noise_out,
    input   uint16  counter,
    input   uint3   resetcounter
) <autorun,reginputs> {
    // RNG random number generator
    random rng( u_noise_out :> u_noise_out,  g_noise_out :> g_noise_out );

    // 1hz timers (P1 used for systemClock, T1hz0 and T1hz1 for user purposes)
    timesinceboot P1( counter1hz :> seconds, counter1mhz :> milliseconds );
    pulse1hz T1hz0( counter1hz :> timer1hz0 );
    pulse1hz T1hz1( counter1hz :> timer1hz1 );

    // 1khz timers (sleepTimers used for sleep command, timer1khzs for user purposes)
    pulse1khz T0khz0( counter1khz :> timer1khz0 );
    pulse1khz T1khz1( counter1khz :> timer1khz1 );
    pulse1khz STimer0( counter1khz :> sleepTimer0 );
    pulse1khz STimer1( counter1khz :> sleepTimer1 );

    T1hz0.resetCounter := 0; T1hz1.resetCounter := 0;
    T0khz0.resetCounter := 0; T1khz1.resetCounter := 0;
    STimer0.resetCounter := 0; STimer1.resetCounter := 0;

    always_after {
        switch( resetcounter ) {
            default: {}
            case 1: { T1hz0.resetCounter = 1; }
            case 2: { T1hz1.resetCounter = 1; }
            case 3: { T0khz0.resetCounter = counter; }
            case 4: { T1khz1.resetCounter = counter; }
            case 5: { STimer0.resetCounter = counter; }
            case 6: { STimer1.resetCounter = counter; }
        }
    }
}

// AUDIO L&R Controller
algorithm audio(
    simple_dualport_bram_port0  samples_left,
    simple_dualport_bram_port0  samples_right,

    input   uint8   staticGenerator,
    input   uint3   waveform,
    input   uint6   frequency,
    input   uint16  duration,
    input   uint2   apu_write,
    output  uint4   audio_l,
    output  uint1   audio_active_l,
    output  uint4   audio_r,
    output  uint1   audio_active_r,

    input   uint11  samples_MAX_L,
    input   uint11  samples_MAX_R
) <autorun,reginputs> {
    // Left and Right audio channels
    apu apu_processor_L( staticGenerator <: staticGenerator, audio_active :> audio_active_l ); audio_pwm PWM_L( wave <: apu_processor_L.audio_output, audio :> audio_l );
    apu apu_processor_R( staticGenerator <: staticGenerator, audio_active :> audio_active_r ); audio_pwm PWM_R( wave <: apu_processor_R.audio_output, audio :> audio_r );

    uint1   SAMPLEMODE_L = uninitialised;                   uint1   SAMPLEMODE_R = uninitialised;
    uint11  SAMP0P1 <:: samples_left.addr0 + 1;             uint11  SAMP1P1 <:: samples_right.addr0 + 1;
    uint16  samples_DURATION_L = uninitialised;             uint16  samples_DURATION_R = uninitialised;

    apu_processor_L.apu_write := 0; apu_processor_R.apu_write := 0;

    always_after {
        // DISPATCH NEXT SAMPLE IF SAMPLE MODE ACTIVE LEFT/RIGHT / RESET SAMPLE POINTER IF NOT
        if( SAMPLEMODE_L ) {
            if( ~apu_processor_L.audio_active ) {
                // SEND NEXT SAMPLE TO LEFT
                apu_processor_L.waveform = 0;
                apu_processor_L.frequency = samples_left.rdata0;
                apu_processor_L.duration = samples_DURATION_L;
                apu_processor_L.apu_write = 1;
                if( SAMP0P1 == samples_MAX_L ) {
                    SAMPLEMODE_L = 0;
                } else {
                    samples_left.addr0 = SAMP0P1;
                }
            }
        } else {
            samples_left.addr0 = 0;
        }
        if( SAMPLEMODE_R ) {
            if( ~apu_processor_R.audio_active ) {
                // SEND NEXT SAMPLE TO RIGHT
                apu_processor_R.waveform = 0;
                apu_processor_R.frequency = samples_right.rdata0;
                apu_processor_R.duration = samples_DURATION_R;
                apu_processor_R.apu_write = 1;
                if( SAMP1P1 == samples_MAX_R ) {
                    SAMPLEMODE_R = 0;
                } else {
                    samples_right.addr0 = SAMP1P1;
                }
            }
        } else {
            samples_right.addr0 = 0;
        }

        // IF REQUEST TO START LEFT/RIGHT CHECK IF SAMPLE MODE REQUESTED AND START, OTHERWISE DISPATCH REQUESTED NOTE
        if( apu_write[0,1] ) {
            if( &waveform ) {
                samples_left.addr0 = 0;
                samples_DURATION_L = duration;
                SAMPLEMODE_L = 1;
            } else {
                apu_processor_L.waveform = waveform;
                apu_processor_L.frequency = frequency;
                apu_processor_L.duration = duration;
                apu_processor_L.apu_write = 1;
                SAMPLEMODE_L = 0;
            }
        }

        if( apu_write[1,1] ) {
            if( &waveform ) {
                samples_right.addr0 = 0;
                samples_DURATION_R = duration;
                SAMPLEMODE_R = 1;
            } else {
                apu_processor_R.waveform = waveform;
                apu_processor_R.frequency = frequency;
                apu_processor_R.duration = duration;
                apu_processor_R.apu_write = 1;
                SAMPLEMODE_R = 0;
            }
        }
    }
}

// UART BUFFER CONTROLLER
// 256 entry FIFO queue
algorithm fifo8(
    output  uint1   available,
    output  uint1   full,
    input   uint1   read,
    input   uint1   write,
    output  uint8   first,
    input   uint8   last
) <autorun,reginputs> {
    simple_dualport_bram uint8 queue[256] = uninitialized;
    uint1   update = uninitialized;
    uint8   top = 0;
    uint8   next = 0;

    available := ( top != next ); full := ( top + 1 == next );
    queue.addr0 := next; first := queue.rdata0;
    queue.wenable1 := 1;

    always_after {
        if( write ) {
            queue.addr1 = top; queue.wdata1 = last;
            update = 1;
        } else {
            if( update ) {
                top = top + 1;
                update = 0;
            }
        }
        next = next + read;
    }
}
algorithm uart_IN(
    // UART
    input   uint1   uart_rx,
    output  uint1   inavailable,
    output  uint8   inchar,
    input   uint1   inread
) <autorun> {
    uart_in ui; uart_receiver urecv( io <:> ui, uart_rx <: uart_rx );
    fifo8 IN(
        available :> inavailable,
        first :> inchar,
        read <: inread,
        last <: ui.data_out,
        write <: ui.data_out_ready
    );
}
algorithm uart_OUT(
    // UART
    output  uint1   uart_tx,
    output  uint1   outfull,
    input   uint8   outchar,
    input   uint1   outwrite
) <autorun> {
    uart_out uo; uart_sender usend( io <:> uo, uart_tx :> uart_tx );
    fifo8 OUT(
        full :> outfull,
        last <: outchar,
        write <: outwrite,
        first :> uo.data_in
    );
    OUT.read := OUT.available & !uo.busy;
    uo.data_in_ready := OUT.available & ( !uo.busy );
}

// PS2 BUFFER CONTROLLER
// 9 bit 256 entry FIFO buffer
algorithm fifo9(
    output  uint1   available,
    input   uint1   read,
    input   uint1   write,
    output  uint9   first,
    input   uint9   last
) <autorun> {
    simple_dualport_bram uint9 queue[256] = uninitialized;
    uint8   top = 0;                                uint8   next = 0;

    available := ( top != next );
    queue.addr0 := next; first := queue.rdata0;
    queue.wenable1 := 1;

    always_after {
        if( write ) { queue.addr1 = top; queue.wdata1 = last; }
        top = top + write;
        next = next + read;
    }
}
algorithm ps2buffer(
    // USB for PS/2
    input   uint1   us2_bd_dp,
    input   uint1   us2_bd_dn,
    output  uint9   inchar,
    output  uint1   inavailable,
    input   uint1   inread,
    input   uint1   outputascii,
    output  uint16  joystick
) <autorun> {
    // PS/2 input FIFO (256 character) - 9 bit to deal with special characters
    fifo9 FIFO( available :> inavailable, read <: inread, write <: PS2.asciivalid, first :> inchar, last <: PS2.ascii );

    // PS 2 KEYCODE TO ASCII CONVERTER AND JOYSTICK EMULATION MAPPER
    ps2ascii PS2( us2_bd_dp <: us2_bd_dp, us2_bd_dn <: us2_bd_dn, outputascii <: outputascii, joystick :> joystick );
}

// SDCARD CONTROLLER
algorithm sdcardcontroller(
    // SDCARD
    output  uint1   sd_clk,
    output  uint1   sd_mosi,
    output  uint1   sd_csn,
    input   uint1   sd_miso,

    input   uint1   readsector,
    input   uint1   writesector,
    input   uint32  sectoraddress,
    output  uint1   ready,

  simple_dualport_bram_port0 buffer_out,
  simple_dualport_bram_port1 buffer_in

) <autorun> {
    // SDCARD - Code for the SDCARD from @sylefeb
    sdcardio sdcio; sdcard sd( sd_clk :> sd_clk, sd_mosi :> sd_mosi, sd_csn :> sd_csn, sd_miso <: sd_miso, io <:> sdcio, buffer_in <:> buffer_in, buffer_out <:> buffer_out );

    // SDCARD Commands
    sdcio.read_sector := readsector;                sdcio.write_sector := writesector;
    sdcio.addr_sector := sectoraddress;             ready := sdcio.ready;
}
