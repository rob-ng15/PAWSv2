$$ uart_in_clock_freq_mhz = 25

// CLOCK SELECTION FLAGS
//$$ sdram_150_mhz = 1

$$if not SIMULATION then
// CLOCKS
$$if sdram_150_mhz then
import('../common/clock_PAWS-sdram150.v')
$$else
import('../common/clock_PAWS-sdram100.v')
$$end
import('../common/clock_PAWS-CPU.v')
$$end

// HDMI for FPGA, VGA for SIMULATION
$$if HDMI then
$include('../common/hdmi.ice')
$$end

$$if VGA then
$include('vga.ice')
$$end

// IO - UART, SDCARD and PS/2 KEYBOARD
$include('../common/uart.ice')
$include('../common/sdcard_write.si')
$include('../common/ps2.si')

// SDRAM
$include('../common/sdram_interfaces.ice')
$include('../common/sdram_controller_autoprecharge_r16_w16.ice')
$include('../common/sdram_utils.ice')

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

// CPU SPECIFICATION
$$CPUISA = 0x40001027
$include('../cpu_functionblocks.si')
$include('../ALU.si')
$include('../FPU.si')
$include('../CPU.si')

// MAIN
$include('../PAWS.si')

// I2C (EMARD FOR RTC)
append('../common/emard/i2c_master.v')
append('../common/emard/mcp7940n.v')
import('../common/emard/rtc-wrapper.v')

