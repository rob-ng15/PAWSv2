// DEFINE I/O CLOCKS
$$ uart_in_clock_freq_mhz = 50

// REGISTER AND MEMORY BUS DEFINITIONS
$$ reg_width = 32
$$ addr_width = 27

// ON CPU INSTRUCTION CACHE DEFINITIONS

// L0 CACHE SIZES FOR HART ID 0 AND 1
// MAX size is 64 due to bram limits ( 32 is 1k )
// size and blocks must be a power of 2
$$ L0Isize = 32
$$ L0Icacheaddrwidth = clog2(L0Isize)

// HART 0 - MAIN
$$ L00Iblocks = 8
$$ L00Icount = clog2(L00Iblocks)
$$ L00Ipartaddresswidth = addr_width - 1 - L00Icount - L0Icacheaddrwidth
$$ L00Ipartaddressstart = 1 + L00Icount + L0Icacheaddrwidth
bitfield L00cacheI{ uint$L00Ipartaddresswidth$ tag, uint30 instruction, uint1 compressed, uint1 valid }

// HART 1 - SMT
$$ L01Iblocks = 2
$$ L01Icount = clog2(L01Iblocks)
$$ L01Ipartaddresswidth = addr_width - 1 - L01Icount - L0Icacheaddrwidth
$$ L01Ipartaddressstart = 1 + L01Icount + L0Icacheaddrwidth
bitfield L01cacheI{ uint$L01Ipartaddresswidth$ tag, uint30 instruction, uint1 compressed, uint1 valid }

// SDRAM CACHE DEFINITIONS

// ADDRESS WIDTH OF THE SDRAM ( 26 bits is 32Mb )
// CHIP SELECT is done by readflag/writeflag
$$ sdram_addr_width = 26

// CACHES SIZES - L1 2 x L1size for DATA
$$if VERILATOR then
$$ L1size = 128
$$else
$$ L1size = 4096
$$end
$$ L1cacheaddrwidth = clog2(L1size)
$$ L1partaddresswidth = sdram_addr_width - 2 - L1cacheaddrwidth
$$ L1partaddressstart = 2 + L1cacheaddrwidth
bitfield L1cachetag{ uint1 needswrite, uint1 valid, uint$L1partaddresswidth$ partaddress }

// BIT WIDTH FOR CSR COUNTERS ( spec is 64 bit )
$$ CWIDTH = 40
$$ CREMAIN = CWIDTH - 32

$$if not SIMULATION then
// CLOCKS
import('../common/clock_PAWS-sdram100.v')
import('../common/clock_PAWS-CPU.v')
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
$include('../terminal.si')
$include('../tile_map.si')
$include('../multiplex_display.si')
$include('../common/audio_pwm.si')
$include('../audio.si')
$include('../video_memmap.si')
$include('../io_memmap.si')
$include('../timers_random.si')

// CPU SPECIFICATION - RV32IMAFCB
$$CPUISA = 0x40011027
$include('../cpu_functionblocks.si')
$include('../ALU.si')
$include('../FPU32.si')
$include('../CPU.si')

// MAIN PAWS.si
$include('../PAWS.si')

// I2C (EMARD FOR RTC)
append('../common/emard/i2c_master.v')
append('../common/emard/mcp7940n.v')
import('../common/emard/rtc-wrapper.v')

// I2C (Silice Test Version)
//$include('../common/mcp7940_i2c.si')
