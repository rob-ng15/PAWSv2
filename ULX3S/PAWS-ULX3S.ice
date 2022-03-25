$$ uart_in_clock_freq_mhz = 25
//$$ sdram_150_mhz = 1
//$$ gpu_50_mhz = 1

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
$include('../common/sdcard_write.ice')
$include('../common/ps2.ice')

// SDRAM
$include('../common/sdram_interfaces.ice')
$include('../common/sdram_controller_autoprecharge_r16_w16.ice')
$include('../common/sdram_utils.ice')
$include('../common/clean_reset.ice')

// Headers
$include('../definitions.ice')
$include('../circuitry.ice')

// Multiplexed Display Includes
$include('../background.ice')
$include('../bitmap.ice')
$include('../GPU.ice')
$include('../character_map.ice')
$include('../sprite_layer.ice')
$include('../terminal.ice')
$include('../tile_map.ice')
$include('../multiplex_display.ice')
$include('../common/audio_pwm.ice')
$include('../audio.ice')
$$if gpu_50_mhz then
$include('../video_memmap-50MHzGPU.ice')
$$else
$include('../video_memmap.ice')
$$end
$include('../io_memmap.ice')
$include('../timers_random.ice')

// CPU SPECIFICATION
$$CPUISA = 0x40001027
$include('../cpu_functionblocks.ice')
$include('../ALU.ice')
$include('../FPU.ice')
$include('../CPU.ice')

// MAIN
$include('../PAWS.ice')

