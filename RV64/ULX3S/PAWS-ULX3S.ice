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
$$ NUCUblocks = 1024
$$ NUCUaddr = clog2(NUCUblocks)
$$ NUCUmem = 8
$$ NUCUmemaddr = clog2(NUCUmem)
$$ NUCUstack = 8
$$ NUCUstackaddr = clog2(NUCUstack)

// AUDIO CHANNEL MULTIPLEXER TIMER WIDTH, BIT SAMPLE WIDTH
$$ AUDIO_multi = 7
$$ AUDIO_bits = 4

// ON CPU INSTRUCTION CACHE DEFINITIONS
// L1 CACHE SIZE
$$ L1Iblocks = 4096
$$ L1Icount = clog2(L1Iblocks)
$$ L1Ipartaddresswidth = addr_width - 1 - L1Icount
$$ L1Ipartaddressstart = 1 + L1Icount
bitfield L1cacheI{ uint30 instruction, uint1 compressed, uint1 valid, uint$L1Ipartaddresswidth$ partaddress }

// SDRAM CACHE DEFINITIONS
// CACHES SIZES - L2 2 x L2size for SDRAM CACHE
$$ L2size = 4096
$$ L2cacheaddrwidth = clog2(L2size)
$$ L2partaddresswidth = paws_ram - 2 - L2cacheaddrwidth
$$ L2partaddressstart = 2 + L2cacheaddrwidth
bitfield L2cache{ uint16 contents, uint1 needswrite, uint1 valid, uint$L2partaddresswidth$ partaddress }

$$ print('CACHE BLOCK CONFIGURATION')
$$ print("L1 BLOCKS: "..L1Iblocks.." WIDTH: "..L1Icount.." TAG SIZE: "..L1Ipartaddresswidth.." AT: "..L1Ipartaddressstart)
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
$include('../VID_BACKGROUND.si')
$include('../VID_BITMAP.si')
$include('../VID_SPRITES.si')
$include('../VID_TEXT.si')
$include('../VID_TILES.si')
$include('../VID_MULTIPLEX.si')
$include('../VID_TOP.si')

// IO DEVICES
$include('../IO_AUDIO.si')
$include('../IO_TIMERS.si')
$include('../IO_TOP.si')

// CPU SPECIFICATION - RV64GCB
$$CPUISA = 0x4001102F
$include('../CPU_ALU.si')
$include('../CPU_CSR.si')
$include('../CPU_DECODE.si')
$include('../CPU_FPU.si')
$include('../CPU_TOP.si')

// MAIN PAWS.si
$include('../DMA.si')
$include('../GPU.si')
$include('../MEMORY.si')
$include('../PAWS.si')

// I2C (EMARD FOR RTC)
append('../common/emard/i2c_master.v')
append('../common/emard/mcp7940n.v')
import('../common/emard/rtc-wrapper.v')

// I2C (Silice Test Version)
//$include('../common/mcp7940_i2c.si')
