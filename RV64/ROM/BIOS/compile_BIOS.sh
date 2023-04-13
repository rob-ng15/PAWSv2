#!/bin/bash

echo "COMPILING FOR INCLUSION IN THE BIOS"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PATH=$PATH:$DIR/../../tools/fpga-binutils/mingw32/bin/

ARCH="riscv64"
GCCVER=12.2.0
CPUOPTS="-march=rv64gc_zba_zbb_zbc_zbs_zbkb_zbkc__zbkx_zfh_zifencei_zicsr -mabi=lp64d -mshorten-memrefs -mrelax"

echo "using $ARCH"

# Following based on FemtoRV compile scripts https://github.com/BrunoLevy/learn-fpga/tree/master/FemtoRV

$ARCH-elf-gcc -fwhole-program -ffunction-sections -fdata-sections -fno-unroll-loops -Os -fno-builtin -fno-pic $CPUOPTS -c -o build/code.o c/BIOS.c
$ARCH-elf-gcc -Os -fno-pic $CPUOPTS -c -o build/crt0.o crt0.c
$ARCH-elf-ld -m elf64lriscv -b elf64-littleriscv -Tconfig_c.ld -o build/code.elf build/code.o
$ARCH-elf-objcopy -O binary build/code.elf ../BIOS.bin
