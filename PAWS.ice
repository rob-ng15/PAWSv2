$$if ICARUS or VERILATOR then
// PLL for simulation
algorithm pll(
  output  uint1 video_clock,
  output! uint1 sdram_clock,
  output! uint1 clock_decode,
  output  uint1 compute_clock
) <autorun> {
  uint3 counter = 0;
  uint8 trigger = 8b11111111;
  sdram_clock   := clock;
  clock_decode   := clock;
  compute_clock := ~counter[0,1]; // x2 slower
  video_clock   := counter[1,1]; // x4 slower
  while (1) {
        counter = counter + 1;
        trigger = trigger >> 1;
  }
}
$$end

algorithm main(
    // LEDS (8 of)
    output  uint8   leds,
$$if not SIMULATION then
    input   uint$NUM_BTNS$ btns,

    // UART
    output  uint1   uart_tx,
    input   uint1   uart_rx,

    // GPIO
    input   uint28  gn,
    output  uint28  gp,

    // USB PS/2
    input   uint1   us2_bd_dp,
    input   uint1   us2_bd_dn,

    // AUDIO
    output  uint4   audio_l,
    output  uint4   audio_r,

    // SDCARD
    output  uint1   sd_clk,
    output  uint1   sd_mosi,
    output  uint1   sd_csn,
    input   uint1   sd_miso,
$$end

$$if HDMI then
    // HDMI OUTPUT
    output! uint4   gpdi_dp,
$$end
$$if VGA then
    // VGA OUTPUT
    output! uint$color_depth$ video_r,
    output! uint$color_depth$ video_g,
    output! uint$color_depth$ video_b,
    output  uint1 video_hs,
    output  uint1 video_vs,
$$end
$$if VERILATOR then
    output  uint1 video_clock,
$$end
    // SDRAM
    output! uint1  sdram_cle,
    output! uint2  sdram_dqm,
    output! uint1  sdram_cs,
    output! uint1  sdram_we,
    output! uint1  sdram_cas,
    output! uint1  sdram_ras,
    output! uint2  sdram_ba,
    output! uint13 sdram_a,
$$if VERILATOR then
    output! uint1  sdram_clock, // sdram controller clock
    input   uint16 sdram_dq_i,
    output! uint16 sdram_dq_o,
    output! uint1  sdram_dq_en,
$$else
    output uint1  sdram_clk,  // sdram chip clock != internal sdram_clock
    inout  uint16 sdram_dq,
$$end
) <@clock_system> {
    uint1   clock_system = uninitialized;
    uint1   clock_io = uninitialized;
    uint1   clock_cpu = uninitialized;
    uint1   clock_decode = uninitialized;
    uint1   gpu_clock = uninitialized;
$$if VERILATOR then
    $$clock_25mhz = 'video_clock'
    // --- PLL
    pll clockgen<@clock,!reset>(
      video_clock   :> video_clock,
      sdram_clock   :> sdram_clock,
      clock_decode   :> clock_decode,
      compute_clock :> clock_system,
      compute_clock :> clock_cpu,
      compute_clock :> clock_io,
      video_clock :> gpu_clock
    );
$$else
    $$clock_25mhz = 'clock'
    // CLOCK/RESET GENERATION
    // CPU + MEMORY
    uint1   sdram_clock = uninitialized;
    uint1   pll_lock_SYSTEM = uninitialized;
    ulx3s_clk_risc_ice_v_SYSTEM clk_gen_SYSTEM (
        clkin    <: $clock_25mhz$,
        clkSYSTEM  :> clock_system,
        clkIO :> clock_io,
        clkSDRAM :> sdram_clock,
        clkSDRAMcontrol :> sdram_clk,
        locked   :> pll_lock_SYSTEM
    );
    uint1   pll_lock_CPU = uninitialized;
    ulx3s_clk_risc_ice_v_CPU clk_gen_CPU (
        clkin    <: $clock_25mhz$,
        clkCPU  :> clock_cpu,
        clkDECODE  :> clock_decode,
        clkGPU :> gpu_clock,
        locked   :> pll_lock_CPU
    );
$$end

    // SDRAM Reset
    uint1   sdram_reset = uninitialized; clean_reset sdram_rstcond<@sdram_clock,!reset> ( out :> sdram_reset );

    // SDRAM chip controller by @sylefeb
    sdram_r16w16_io sio_fullrate; sdram_r16w16_io sio_halfrate;
    sdram_half_speed_access sdaccess <@sdram_clock,!sdram_reset> ( sd <:> sio_fullrate, sdh <:> sio_halfrate );
    sdram_controller_autoprecharge_r16_w16 sdram32MB <@sdram_clock,!sdram_reset> (
        sd        <:> sio_fullrate,
        sdram_cle :>  sdram_cle,
        sdram_dqm :>  sdram_dqm,
        sdram_cs  :>  sdram_cs,
        sdram_we  :>  sdram_we,
        sdram_cas :>  sdram_cas,
        sdram_ras :>  sdram_ras,
        sdram_ba  :>  sdram_ba,
        sdram_a   :>  sdram_a,
  $$if VERILATOR then
        dq_i       <: sdram_dq_i,
        dq_o       :> sdram_dq_o,
        dq_en      :> sdram_dq_en,
  $$else
        sdram_dq  <:> sdram_dq,
  $$end
    );

    // SDRAM ( via CACHE ) and BRAM (for BIOS AND FAST BRAM )
    // byteaccess controls byte read/writes
    uint1   byteaccess <:: ( ~|CPU.accesssize[0,2] );
    cachecontroller DRAM <@clock_system,!reset> (
        sio <:> sio_halfrate,
        byteaccess <: byteaccess,
        cacheselect <: CPU.cacheselect,
        address <: CPU.address[0,26],
        writedata <: CPU.writedata
    );
    bramcontroller RAM <@clock_system,!reset> (
        byteaccess <: byteaccess,
        address <: CPU.address[0,15],
        writedata <: CPU.writedata
    );

    // MEMORY MAPPED I/O + SMT CONTROLS
    io_memmap IO_Map <@clock_io,!reset> (
        leds :> leds,
$$if not SIMULATION then
        gn <: gn,
        gp :> gp,
        btns <: btns,
        uart_tx :> uart_tx,
        uart_rx <: uart_rx,
        us2_bd_dp <: us2_bd_dp,
        us2_bd_dn <: us2_bd_dn,
        sd_clk :> sd_clk,
        sd_mosi :> sd_mosi,
        sd_csn :> sd_csn,
        sd_miso <: sd_miso,
$$end
        clock_25mhz <: $clock_25mhz$,

        memoryAddress <: CPU.address[0,12],
        writeData <: CPU.writedata,
        DMAACTIVE <: CPU.DMAACTIVE
    );

$$if SIMULATION then
    uint4 audio_l(0);
    uint4 audio_r(0);
$$end
    timers_memmap TIMERS_Map <@clock_io,!reset> (
        clock_25mhz <: $clock_25mhz$,
        memoryAddress <: CPU.address[0,5],
        writeData <: CPU.writedata
    );

    audio_memmap AUDIO_Map <@clock_io,!reset> (
        clock_25mhz <: $clock_25mhz$,
        memoryAddress <: CPU.address[0,3],
        writeData <: CPU.writedata,
        audio_l :> audio_l,
        audio_r :> audio_r,
        static4bit <: TIMERS_Map.static16bit[0,4]
    );

    video_memmap VIDEO_Map <@clock_io,!reset> (
        video_clock <: $clock_25mhz$,
        memoryAddress <: CPU.address[0,12],
        writeData <: CPU.writedata,
$$if HDMI then
        gpdi_dp :> gpdi_dp,
$$end
$$if VGA then
        video_r  :> video_r,
        video_g  :> video_g,
        video_b  :> video_b,
        video_hs :> video_hs,
        video_vs :> video_vs,
$$end
        static6bit <: TIMERS_Map.static16bit[0,6],
        blink <: TIMERS_Map.cursor
    );

    PAWSCPU CPU <@clock_cpu,!reset> (
        clock_CPUdecoder <: clock_decode,
        SMTRUNNING <: IO_Map.SMTRUNNING,
        SMTSTARTPC <: IO_Map.SMTSTARTPC[0,27],
        DMASOURCE <: IO_Map.DMASOURCE,
        DMADEST <: IO_Map.DMADEST,
        DMACOUNT <: IO_Map.DMACOUNT,
        DMAMODE <: IO_Map.DMAMODE,
        readdata <: readdata
    );

    // IDENTIFY ADDRESS BLOCK
    uint1   SDRAM <: CPU.address[26,1];
    uint1   BRAM <: ~SDRAM & ~CPU.address[15,1];
    uint1   IOmem <: ~SDRAM & ~BRAM;
    uint1   TIMERS <: IOmem & ( ~|CPU.address[12,2] );
    uint1   VIDEO <: IOmem & ( CPU.address[12,2] == 2h1 );
    uint1   AUDIO <: IOmem & ( CPU.address[12,2] == 2h2 );
    uint1   IO <: IOmem & ( &CPU.address[12,2] );

    // READ FROM SDRAM / BRAM / IO REGISTERS
    uint16  readdata <: SDRAM ? DRAM.readdata :
                BRAM ? RAM.readdata :
                TIMERS ? TIMERS_Map.readData :
                VIDEO ? VIDEO_Map.readData :
                AUDIO ? AUDIO_Map.readData :
                IO? IO_Map.readData : 0;

    // SDRAM -> CPU BUSY STATE
    CPU.memorybusy := DRAM.busy | ( ( CPU.readmemory | CPU.writememory ) & ( BRAM | SDRAM ) );

    always_before {
        DRAM.readflag = SDRAM & CPU.readmemory;
        RAM.readflag = BRAM & CPU.readmemory;
        AUDIO_Map.memoryRead = AUDIO & CPU.readmemory;
        IO_Map.memoryRead = IO & CPU.readmemory;
        TIMERS_Map.memoryRead = TIMERS & CPU.readmemory;
        VIDEO_Map.memoryRead = VIDEO & CPU.readmemory;
    }
    always_after {
        DRAM.writeflag = SDRAM & CPU.writememory;
        RAM.writeflag = BRAM & CPU.writememory;
        AUDIO_Map.memoryWrite = AUDIO & CPU.writememory;
        IO_Map.memoryWrite = IO & CPU.writememory;
        TIMERS_Map.memoryWrite = TIMERS & CPU.writememory;
        VIDEO_Map.memoryWrite = VIDEO & CPU.writememory;
    }
}

// RAM - BRAM controller
// MEMORY IS 16 BIT, 8 bit WRITES ARE READ MODIFY WRITE
algorithm bramcontroller(
    input   uint15  address,
    input   uint1   byteaccess,

    input   uint1   writeflag,
    input   uint16  writedata,

    input   uint1   readflag,
    output  uint16  readdata
) <autorun,reginputs> {
$$if not SIMULATION then
    // RISC-V FAST BRAM and BIOS
    bram uint16 ram[16384] = {
        $include('ROM/BIOS.inc')
        , pad(uninitialized)
    };
$$else
    // RISC-V FAST BRAM and BIOS FOR VERILATOR - TEST FOR SMT AND FPU
    bram uint16 ram[16384] = {
        $include('ROM/VBIOS.inc')
        , pad(uninitialized)
    };
$$end

    uint1   update = uninitialized;

    // FLAGS FOR BRAM ACCESS
    ram.wenable := 0; ram.addr := address[1,14]; readdata := ram.rdata;
    ram.wdata := byteaccess ? ( address[0,1] ? { writedata[0,8], ram.rdata[0,8] } : { ram.rdata[8,8], writedata[0,8] } ) : writedata;

    always_after {
        if( writeflag ) {
            ram.wenable = update | ~byteaccess;
            if( byteaccess ) { update = 1; }
        } else {
            ram.wenable = update;
            update = 0;
        }
    }
}

// 32Mb of SDRAM using @sylefeb controller
// Controlled by a 16bit EVICTION CACHE FOR DATA AND INSTRUCTIONS
// Cache-coherency is maintained
// Controller is 16bit, the natural width of the SDRAM on the ULX3s
// An eviction cache was chosen as easy to implement as a directly mapped cache
// Writes to SDRAM only if required when evicting a cache entry

// DATA CACHE SIZE IS NUMBER OF 16bit ENTRIES
$$ size = 4096
$$ cacheaddrwidth = clog2(size)
$$ partaddresswidth = 25 - cacheaddrwidth
$$ partaddressstart = 1 + cacheaddrwidth
bitfield cachetag{ uint1 needswrite, uint1 valid, uint$partaddresswidth$ partaddress }

// INSTRUCTION CACHE IS NUMBER OF 16bit ENTRIES
$$ Isize = 8192
$$ Icacheaddrwidth = clog2(Isize)
$$ Ipartaddresswidth = 25 - Icacheaddrwidth
$$ Ipartaddressstart = 1 + Icacheaddrwidth
bitfield Icachetag{ uint1 valid, uint$Ipartaddresswidth$ partaddress }

algorithm cachecontroller(
    sdram_user      sio,
    input   uint1   cacheselect,
    input   uint26  address,
    input   uint1   byteaccess,
    input   uint1   writeflag,
    input   uint16  writedata,
    input   uint1   readflag,
    output  uint16  readdata,
    output  uint1   busy(0)
) <autorun,reginputs> {
    // DATA CACHE for SDRAM - CACHE SIZE DETERMINED BY size DEFINED ABOVE, MUST BE A POWER OF 2
    // DATA CACHE ADDRESS IS LOWER bits of the address, dropping the BYTE address bit
    // DATA CACHE TAG IS REMAINING bits of the 26 bit address + 1 bit for valid flag + 1 bit for needwritetosdram flag
    simple_dualport_bram uint16 cache[$size$] = uninitialized;
    simple_dualport_bram uint$partaddresswidth+2$ tags[$size$] = uninitialized;

    // INSTRUCTION CACHE for SDRAM
    // DEFINED AS ABOVE EXCEPT NO NEED FOR needwritetosdram flag
    simple_dualport_bram uint16 Icache[$Isize$] = uninitialized;
    simple_dualport_bram uint$Ipartaddresswidth+1$ Itags[$Isize$] = uninitialized;

    // CACHE WRITERS
    cachewriter CW( cache <:> cache, tags <:> tags, address <: address );
    Icachewriter ICW( Icache <:> Icache, Itags <:> Itags, address <: address );

    // SDRAM CONTROLLER
    sdramcontroller SDRAM( sio <:> sio, writedata <: cache.rdata0 );

    // CACHE TAG match flags
    uint1   cachetagmatch <:: ( { cachetag(tags.rdata0).valid, cachetag(tags.rdata0).partaddress } == { 1b1, address[$partaddressstart$,$partaddresswidth$] } );
    uint1   Icachetagmatch <:: ( Itags.rdata0 == { 1b1, address[$Ipartaddressstart$,$Ipartaddresswidth$] } );

    // VALUE TO WRITE TO CACHE ( deals with correctly mapping 8 bit writes and 16 bit writes, using sdram or cache as base )
    uint16  writethrough <:: ( byteaccess ) ? ( address[0,1] ? { writedata[0,8], cachetagmatch ? cache.rdata0[0,8] : SDRAM.readdata[0,8] } :
                                                                { cachetagmatch ? cache.rdata0[8,8] : SDRAM.readdata[8,8], writedata[0,8] } ) : writedata;

    // MEMORY ACCESS FLAGS
    uint1   doread = uninitialized;                 uint1   dowrite = uninitialized;
    uint1   doreadsdram <:: ( doread | ( dowrite & byteaccess ) );

    //  FOR QUICK RETURN IF READING FROM LAST DATA CACHE ADDRESS
    uint25  lastaddress = uninitialized;            uint1   lastdcacheaddress <:: cacheselect & ( lastaddress == address[1,25] );

    // SDRAM ACCESS
    SDRAM.readflag := 0; SDRAM.writeflag := 0;

    // FLAGS FOR CACHE ACCESS
    cache.addr0 := address[1,$cacheaddrwidth$]; tags.addr0 := address[1,$cacheaddrwidth$];
    CW.needwritetosdram := dowrite;  CW.writedata := dowrite ? writethrough : SDRAM.readdata; CW.update := 0;

    Icache.addr0 := address[1,$Icacheaddrwidth$]; Itags.addr0 := address[1,$Icacheaddrwidth$];
    ICW.writedata := dowrite ? writethrough : SDRAM.readdata; ICW.update := 0;

    // 16 bit READ
    readdata := ( ~cacheselect & Icachetagmatch ) ? Icache.rdata0 : ( doread & lastdcacheaddress ) | cachetagmatch ? cache.rdata0 : SDRAM.readdata;

    while(1) {
        doread = readflag; dowrite = writeflag;
        if( doread & lastdcacheaddress ) {                                                                                                          // USING LAST READ DATA CACHE ADDRESS
            busy = 0;
        } else {
            if( doread | dowrite ) {
                busy = 1;                                                                                                                           // MARK BUSY,
                ++:                                                                                                                                 // WAIT FOR CACHE
                if( doread & ( ( ~cacheselect & Icachetagmatch ) | ( cacheselect & cachetagmatch ) ) ) {                                                // READ IN CACHE
                    busy = 0;
                } else {
                    if( cacheselect & cachetagmatch ) {                                                                                                 // IN CACHE
                        CW.update = dowrite;                                                                                                            // UPDATE IF WRITE
                        ICW.update = Icachetagmatch & dowrite;                                                                                          // UPDATE ICACHE IF NEEDED
                        busy = 0;
                    } else {
                        if( cachetag(tags.rdata0).needswrite ) {                                                                                        // CHECK IF CACHE LINE IS OCCUPIED
                            while( SDRAM.busy ) {} SDRAM.address = { cachetag(tags.rdata0).partaddress, address[1,$cacheaddrwidth$], 1b0 };             // EVICT FROM CACHE TO SDRAM
                            SDRAM.writeflag = 1;
                        }
                        if( doreadsdram ) {                                                                                                             // NEED TO READ SDRAM
                            while( SDRAM.busy ) {} SDRAM.address = address; SDRAM.readflag = 1; while( SDRAM.busy ) {}                                  // READ FOR READ OR 8 BIT WRITE
                            CW.update = cacheselect;                                                                                                    // UPDATE THE CACHE
                            ICW.update = ~cacheselect;                                                                                                  // UPDATE ICACHE IF NEEDED
                        } else {
                            CW.update = dowrite;                                                                                                        // UPDATE CACHE FOR 16 BIT WRITE
                            ICW.update = Icachetagmatch & dowrite;                                                                                      // UPDATE ICACHE IF NEEDED
                        }
                        busy = 0;
                    }
                }
                if( cacheselect ) { lastaddress = address[1,25]; }                                                                                  // LATCH LAST DATA CACHE ADDRESS
            } else {
                busy = 0;
            }
        }
    }
}

// WRITE TO DATA CACHE
algorithm cachewriter(
    input   uint26  address,
    input   uint1   needwritetosdram,
    input   uint16  writedata,
    input   uint1   update,
    simple_dualport_bram_port1 cache,
    simple_dualport_bram_port1 tags
) <autorun,reginputs> {
    cache.wenable1 := update; tags.wenable1 := update;
    cache.addr1 := address[1,$cacheaddrwidth$]; cache.wdata1 := writedata;
    tags.addr1 := address[1,$cacheaddrwidth$]; tags.wdata1 := { needwritetosdram, 1b1, address[$partaddressstart$,$partaddresswidth$] };
}

// WRITE TO INSTRCUTION CACHE
algorithm Icachewriter(
    input   uint26  address,
    input   uint16  writedata,
    input   uint1   update,
    simple_dualport_bram_port1 Icache,
    simple_dualport_bram_port1 Itags
) <autorun,reginputs> {
    Icache.wenable1 := update; Itags.wenable1 := update;
    Icache.addr1 := address[1,$Icacheaddrwidth$]; Icache.wdata1 := writedata;
    Itags.addr1 := address[1,$Icacheaddrwidth$]; Itags.wdata1 := { 1b1, address[$Ipartaddressstart$,$Ipartaddresswidth$] };
}

algorithm sdramcontroller(
    sdram_user      sio,
    input   uint26  address,
    input   uint1   writeflag,
    input   uint16  writedata,
    input   uint1   readflag,
    output  uint16  readdata,
    output  uint1   busy(0)
) <autorun,reginputs> {
    // MEMORY ACCESS FLAGS
    sio.addr := { address[1,25], 1b0 }; sio.in_valid := ( readflag | writeflag );
    sio.data_in := writedata; sio.rw := writeflag;
    readdata := sio.data_out;

    always_after {
        busy = ( sio.done ) ? 0 : ( readflag | writeflag ) ? 1 : busy;
    }
}
