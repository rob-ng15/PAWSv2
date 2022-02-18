# PAWSv2 a Risc-V RVIMAFCB CPU

* Written in Silice
    * Inspired by ICE-V by [ICE-V](https://github.com/sylefeb/Silice/tree/master/projects/ice-v) by @sylefeb

A simple (no interrupts or exceptions/traps), Risc-V RV32IMAFC CPU, with a pseudo SMT (dual thread) capability. The display is similar to the 8-bit era machines, along with audio, SDCARD read support, UART and PS/2 keyboard input.

I set about creating this project as I wanted an easier way to write programs to test my design. I'm old enough that C was my main programming language, so a CPU that had gcc support was the obvious way forward, and Risc-V is a nice elegant design that wasn't too hard for a beginner at FPGA programming to implement.

The display hardware does as much as possible to compensate for the slow CPU speed.

## Quick Instructions

Copy the files from the directory "SOFTWARE/PAWS" to an SDCARD and insert into the ULX3S. Flash the bitstream from "ULX3S/BUILD_ulx3s/PAWSv2.bit" to the ULX3S

```
fujprog PAWSv2.bit
```

This will initialise the BIOS and allow the selection of a PAW file to load and run.

## BIOS

![BIOS](documentation/Screenshots/BIOS.jpg)

* BIOS
    * Initialises the screen and CPU
    * Loads compiled PAW files from the SDCARD to SDRAM and executes
        * FAT32 on SDCARD read via SPI

## Overview

* CPU
    * 50MHz clock
        * Instructions take varying number of clock cycles for execution
            * 4 Pipeline Stages
                * Fetch ( starts partial decode )
                * Decode
                * Execute
                * Dispatch
        * RV32I instruction set
            * AUIPC
            * LUI
            * JAL and JALR
            * BRANCH
            * LOAD
            * STORE
            * INTEGER OPERATION WITH IMMEDIATE PARAMETER
                * ADDI
                * SLTI[U]
                * ANDI ORI XORI
                * SLI SRLI SRAI
            * INTEGER OPERATION WITH REGISTER PARAMETER
                * ADD SUB
                * SLT[U]
                * AND OR XOR
                * SLL SRL SRA
            * CSR ( Limited )
                * READ of TIMER[H], CYCLE[H] and INSTRET[H]
                * Floating-point FCSR register
        * RV32M instruction set
            * HARDWARE MULTIPLICATION AND DIVISION UNITS
                * MUL MULH[[S]U]
                * DIV[U] REM[U]
        * RV32A instruction set
        * RV32F instruction set
        * RV32C instruction expansion
            * COMPRESSED ( 16 bit ) INSTRUCTION SUPPORT
                * Expanded to 32 bit instruction
                * Faster than 32 bit instructions due to less memory fetching
        * RV32B instruction set
            * BIT MANIPULATION INSTRUCTIONS
                * Zba
                    * SH1ADD SH2ADD SH3ADD
                * Zbb
                    * ANDN ORN XNOR
                    * CLZ CPOP CTZ
                    * MAX[U] MIN[U]
                    * ORC.B REV8
                    * ROL ROR[I]
                    * SEXT.B SEXT.H ZEXT.H
                * Zbc
                    * CLMUL CLMULH CLMULR
                * Zbs
                    * BCLR[I] BEXT[I] BINV[I] BSET[I]

* MEMORY
    * 32K of RAM
        * FAST BRAM - used for BIOS
        * STACK POINTER AT TOP OF BRAM
    * 32K ( 8K x 32 bit ) of I/O Registers with 16 bit read / write
    * 32MB of SDRAM
        * 16K Instruction and 8K Data Cache

* DISPLAY
    * HDMI 640 x 480 ouput with 127 colours + transparency
        * Background with pattern generator
            * Co-Processor to switch pattern generator parameters mid-frame
        * 2 x Tilemap Layers ( full colour )
            * 42 x 32 ( 40 x 30 visible ) with ( 0 - 15 ) pixel scrolling
            * Tiles can be reflected or rotated
        * 2 x Sprite Layers ( full colour )
            * 16 sprites per layer
            * Sprites can be reflected or rotated, and doubled in size
        * Bitmap ( 320 x 240 double sized pixels ) with GPU
            * Accelerated drawing of lines, rectangles, circles ( optional fill ), triangles, quadrilaterals
            * Single and full colour hardware blitters
            * DMA assisted software blitter
            * Vector block drawer
        * Character Map
            * 80 x 60 with normal/bold font
        * Terminal
            * 80 x 8

![BIOS](documentation/Screenshots/COLOURS.jpg)
<br>128 Colour Display
* 4 Colour Modes
    * Default PAWSv2 as shown
    * GRRGGBB mode
    * PAWSv1 with upper 63 colours as shades of grey
    * Greayscale with 127 shades of grey

![BIOS](documentation/Screenshots/3DMAZE.jpg)
<br>Demonstration game for the GPU and the hardware accelerated drawing of 2D Shapes

![BIOS](documentation/Screenshots/ASTEROIDS.jpg)
<br>Demonstration game for the sprites

![BIOS](documentation/Screenshots/INVADERS.jpg)
<br>Demonstration game for the blitter and sprites

![BIOS](documentation/Screenshots/OUTRUN.jpg)
<br>Test for the floating point unit and the GPU

* PERIPHERALS
    * UART ( via US1 on the ULX3S )
        * 115200 baud
    * PS/2 keyboard, with joystick emulator
    * LEDS ( 8 on board leds )
    * BUTTONS ( 6 on board buttons )
    * TIMERS ( 1hz and 1khz )
    * STEREO AUDIO
    * SDCARD via SPI
        * FAT32 read only support
        * With Linux compatible file I/O plus PAWS file selector
    * MINI-DMA CONTROLLER
