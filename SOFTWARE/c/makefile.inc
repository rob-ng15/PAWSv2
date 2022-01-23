 OS := $(shell uname -s)
IS_APPLE := $(shell echo $(OS)|grep -i darwin)

ARCH=riscv64
AR=$(ARCH)-elf-ar
CC=$(ARCH)-elf-gcc
LD=$(ARCH)-elf-ld
NM=$(ARCH)--elf-nm
RANLIB=$(ARCH)--elf-ranlib

CFLAGS=-march=rv32imafc -mabi=ilp32f -ffunction-sections -fdata-sections -Ofast -fno-pic -fno-unroll-loops -Wno-stringop-overread
LFLAGS=--as-needed --gc-sections -m elf32lriscv -b elf32-littleriscv --no-relax
LCONFIG=-T../config_c_SDRAM.ld
INCLUDE=-I/usr/riscv32-elf/include/
LIBRARY=/usr/riscv64-elf/lib/rv32imafc/ilp32f/libc.a \
	/usr/riscv64-elf/lib/rv32imafc/ilp32f/libm.a \
	/usr/lib/gcc/riscv64-elf/11.1.0/rv32imafc/ilp32f/libgcc.a \

LIBPAWS=../library/build/libPAWS.o
LIBPAWSSOURCE=../library/nanojpeg.c ../library/PAWS.h ../library/PAWSdefinitions.h ../library/PAWSlibrary.c $(LIBPAWSC)
