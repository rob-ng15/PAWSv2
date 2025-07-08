#!/bin/bash

echo "COMPILING FOR INCLUSION IN THE BIOS"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PATH=$PATH:$DIR/../../tools/fpga-binutils/mingw32/bin/

ARCH="riscv64-unknown"
GCCVER=13.2.0
CPUOPTS="-march=rv32imafc -mabi=ilp32f -mrelax"

echo "using $ARCH"

# Following based on FemtoRV compile scripts https://github.com/BrunoLevy/learn-fpga/tree/master/FemtoRV

$ARCH-elf-gcc -fwhole-program -ffunction-sections -fdata-sections -fno-unroll-loops -Os -fno-builtin -fno-pic $CPUOPTS -c -o build/code.o c/VBIOS.c
$ARCH-elf-gcc -fwhole-program -ffunction-sections -fdata-sections -fno-unroll-loops -Os -fno-builtin -fno-pic $CPUOPTS -S -o build/code.s c/VBIOS.c
$ARCH-elf-gcc -Os -fno-pic $CPUOPTS -c -o build/crt0.o crt0.c
$ARCH-elf-ld -m elf32lriscv -b elf32-littleriscv -Tconfig_c.ld  --relax-gp -o build/code.elf build/code.o /usr/lib/gcc/$ARCH-elf/$GCCVER/rv32imafc/ilp32f/libgcc.a
$ARCH-elf-objcopy -O binary build/code.elf ../VBIOS.bin
