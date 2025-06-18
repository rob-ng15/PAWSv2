// DEFINE I/O CLOCKS
$$ uart_in_clock_freq_mhz = 50

// ADDRESS WIDTH OF THE SDRAM ( 25 bits is 32Mb, 26 bits used as an extra 0 has to be inserted to correctly address 32 bits )
// CHIP SELECT is done by readflag/writeflag
$$ sdram_width = 26
$$ paws_ram = 25

// REGISTER AND MEMORY BUS DEFINITIONS
$$ reg_width = 64
$$ addr_width = 27

// NuCU COPPER BACKGROUND PROCESSOR DEFINITIONS
// blocks number of program entries, mem number of memory registers, stack number of rstack and dstack entries
// NUCUmem must be at least 8, which can be set by the CPU before NUCU program starts
$$ NUCUblocks = 128
$$ NUCUaddr = clog2(NUCUblocks)
$$ NUCUmem = 8
$$ NUCUmemaddr = clog2(NUCUmem)
$$ NUCUstack = 8
$$ NUCUstackaddr = clog2(NUCUstack)

// AUDIO CHANNEL MULTIPLEXER TIMER WIDTH, BIT SAMPLE WIDTH
$$ AUDIO_multi = 7
$$ AUDIO_bits = 4

// ON CPU INSTRUCTION CACHE DEFINITIONS
// L1 CACHE SIZES FOR HART ID 0 AND 1
// blocks must be a power of 2
// HART 0 - MAIN
$$ L10Iblocks = 4096
$$ L10Icount = clog2(L10Iblocks)
$$ L10Ipartaddresswidth = addr_width - 1 - L10Icount
$$ L10Ipartaddressstart = 1 + L10Icount
bitfield L10cacheI{ uint30 instruction, uint1 compressed, uint1 valid, uint$L10Ipartaddresswidth$ partaddress }

// HART 1 - SMT
$$ L11Iblocks = 512
$$ L11Icount = clog2(L11Iblocks)
$$ L11Ipartaddresswidth = addr_width - 1 - L11Icount
$$ L11Ipartaddressstart = 1 + L11Icount
bitfield L11cacheI{ uint30 instruction, uint1 compressed, uint1 valid, uint$L11Ipartaddresswidth$ partaddress }

// SDRAM CACHE DEFINITIONS
// CACHES SIZES - L2 2 x L2size for SDRAM CACHE
$$ L2size = 4096
$$ L2cacheaddrwidth = clog2(L2size)
$$ L2partaddresswidth = paws_ram - 2 - L2cacheaddrwidth
$$ L2partaddressstart = 2 + L2cacheaddrwidth
bitfield L2cache{ uint16 contents, uint1 needswrite, uint1 valid, uint$L2partaddresswidth$ partaddress }

$$ print('CACHE BLOCK CONFIGURATION')
$$ print("L1 BLOCKS: "..L10Iblocks.." WIDTH: "..L10Icount.." TAG SIZE: "..L10Ipartaddresswidth.." AT: "..L10Ipartaddressstart)
$$ print("L2 BLOCKS: "..L2size.." WIDTH: "..L2cacheaddrwidth.." TAG SIZE: "..L2partaddresswidth.." AT: "..L2partaddressstart)

// BIT WIDTH FOR CSR COUNTERS ( spec is 64 bit )
$$ CWIDTH = 40

$$if not SIMULATION then
// CLOCKS
import('../common/clock_PAWS-SYS.v')
import('../common/clock_PAWS-CPU.v')
//import('../common/clock_PAWS-SDRAM.v')
$$end

// HDMI for FPGA, VGA for SIMULATION
$$if HDMI then
$include('../common/hdmi.ice')
$$end

$$if VGA then
$include('vga.si')
$$end

// IO - UART, SDCARD, PS/2 KEYBOARD
$include('../common/uart.si')
$include('../common/sdcard_write.si')
$include('../common/ps2.si')

// SDRAM
$include('../common/sdram_interfaces.si')
$include('../common/sdram_controller_autoprecharge_r16_w16.si')
$include('../common/sdram_utils.si')

// CLEAN RESET - WIDTH 1v
$$ clean_reset_width = 1
$include('../common/clean_reset.si')

// Headers
$include('../definitions.si')
$include('../circuitry.si')

// Multiplexed Display Includes
$include('../background.si')
$include('../bitmap.si')
$include('../GPU.si')
$include('../character_map.si')
$include('../sprite_layer.si')
$include('../tile_map.si')
$include('../multiplex_display.si')
$include('../audio.si')
$include('../video_memmap.si')
$include('../io_memmap.si')
$include('../timers_random.si')

// CPU SPECIFICATION - RV64GCB
$$CPUISA = 0x4001102F
$include('../CPU_ALU.si')
$include('../CPU_CSR.si')
$include('../CPU_DECODE.si')
$include('../CPU_FPU.si')
$include('../CPU_TOP.si')

// MAIN PAWS.si
$include('../DMA.si')
$include('../MEMORY.si')
$include('../PAWS.si')

// I2C (EMARD FOR RTC)
append('../common/emard/i2c_master.v')
append('../common/emard/mcp7940n.v')
import('../common/emard/rtc-wrapper.v')

// I2C (Silice Test Version)
//$include('../common/mcp7940_i2c.si')
