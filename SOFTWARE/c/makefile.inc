OS := $(shell uname -s)
IS_APPLE := $(shell echo $(OS)|grep -i darwin)

everything: all

ARCH=riscv64
AR=$(ARCH)-elf-ar
CC=$(ARCH)-elf-gcc
LD=$(ARCH)-elf-ld
NM=$(ARCH)--elf-nm
RANLIB=$(ARCH)--elf-ranlib

BUILD_DIR=build
OUTPUT_DIR=../../PAWS/

CFLAGS=-march=rv32imafc -mabi=ilp32f -ffunction-sections -fdata-sections -O2 -finline -fno-pic -fno-unroll-loops -ffast-math -Wno-stringop-overread
LFLAGS=--as-needed --gc-sections -m elf32lriscv -b elf32-littleriscv --no-relax
LCONFIG=-T../config_c_SDRAM.ld

PAWSBASE=../
INCLUDE=-I/usr/riscv32-elf/include/ -I$(PAWSBASE)/library/

LIBRARY=/usr/riscv64-elf/lib/rv32imafc/ilp32f/libc.a \
	/usr/riscv64-elf/lib/rv32imafc/ilp32f/libm.a \
	/usr/lib/gcc/riscv64-elf/11.1.0/rv32imafc/ilp32f/libgcc.a \

LIBPAWSSOURCE=	$(PAWSBASE)library/PAWS.h $(PAWSBASE)library/PAWSdefinitions.h $(PAWSBASE)library/PAWSlibrary.c
LIBPAWSOBJ=	$(PAWSBASE)library/build/libPAWS.o

NANOJPEGSOURCE= $(PAWSBASE)library/nanojpeg/nanojpeg.c
NANOJPEGOBJ=	$(PAWSBASE)library/build/nanojpeg.o

FATIOLIBSOURCE= $(wildcard $(PAWSBASE)library/fat_io_lib/*.c) $(wildcard $(PAWSBASE)library/fat_io_lib/*.h)
FATIOLIBOBJ=	$(PAWSBASE)library/build/fat_filelib.o \
		$(PAWSBASE)library/build/fat_access.o \
		$(PAWSBASE)library/build/fat_table.o \
		$(PAWSBASE)library/build/fat_write.o \
		$(PAWSBASE)library/build/fat_misc.o \
		$(PAWSBASE)library/build/fat_string.o \
		$(PAWSBASE)library/build/fat_cache.o \
		$(PAWSBASE)library/build/fat_format.o

LIBPAWS=$(LIBPAWSOBJ) $(NANOJPEGOBJ) $(FATIOLIBOBJ)

$(LIBPAWSOBJ) : $(LIBPAWSSOURCE)
	$(CC) $(CFLAGS) -finline -c $(PAWSBASE)library/PAWSlibrary.c -o $@

$(NANOJPEGOBJ) : $(NANOJPEGSOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(FATIOLIBOBJ) : $(FATIOLIBSOURCE)
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_filelib.c -o $(PAWSBASE)library/build/fat_filelib.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_access.c -o $(PAWSBASE)library/build/fat_access.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_table.c -o $(PAWSBASE)library/build/fat_table.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_write.c -o $(PAWSBASE)library/build/fat_write.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_misc.c -o $(PAWSBASE)library/build/fat_misc.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_string.c -o $(PAWSBASE)library/build/fat_string.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_cache.c -o $(PAWSBASE)library/build/fat_cache.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_format.c -o $(PAWSBASE)library/build/fat_format.o

$(BUILD_DIR)/crt0.o : ../crt0.c
	$(CC) $(CFLAGS) -c ../crt0.c -o $@
